/*
** $Id: lstate.c,v 2.12 2004/08/30 13:44:44 roberto Exp $
** Global State
** See Copyright Notice in lua.h
*/


#include <stddef.h>

#define lstate_c
#define LUA_CORE

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "llex.h"
#include "lmem.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"


/*
** macro to allow the inclusion of user information in Lua state
*/
#ifndef LUA_USERSTATE
#define EXTRASPACE	0
#else
union UEXTRASPACE {L_Umaxalign a; LUA_USERSTATE b;};
#define EXTRASPACE (sizeof(union UEXTRASPACE))
#endif


#define state_size(x)	(sizeof(x) + EXTRASPACE)
#define tostate(l)	(cast(lua_State *, cast(lu_byte *, l) + EXTRASPACE))
#define fromstate(l)	(cast(lu_byte *, (l)) - EXTRASPACE)


/*
** Main thread combines a thread state and the global state
*/
typedef struct LG {
  lua_State l;
  global_State g;
} LG;
  


static void stack_init (lua_State *L1, lua_State *L) {
  L1->stack = luaM_newvector(L, BASIC_STACK_SIZE + EXTRA_STACK, TValue);
  L1->stacksize = BASIC_STACK_SIZE + EXTRA_STACK;
  L1->top = L1->stack;
  L1->stack_last = L1->stack+(L1->stacksize - EXTRA_STACK)-1;
  L1->base_ci = luaM_newvector(L, BASIC_CI_SIZE, CallInfo);
  L1->ci = L1->base_ci;
  L1->ci->func = L1->top;
  setnilvalue(L1->top++);  /* `function' entry for this `ci' */
  L1->base = L1->ci->base = L1->top;
  L1->ci->top = L1->top + LUA_MINSTACK;
  L1->size_ci = BASIC_CI_SIZE;
  L1->end_ci = L1->base_ci + L1->size_ci;
}


static void freestack (lua_State *L, lua_State *L1) {
  luaM_freearray(L, L1->base_ci, L1->size_ci, CallInfo);
  luaM_freearray(L, L1->stack, L1->stacksize, TValue);
}


/*
** open parts that may cause memory-allocation errors
*/
static void f_luaopen (lua_State *L, void *ud) {
  Udata *u;  /* head of udata list */
  global_State *g = G(L);
  UNUSED(ud);
  u = cast(Udata *, luaM_malloc(L, sizeudata(0)));
  u->uv.len = 0;
  u->uv.metatable = NULL;
  g->firstudata = obj2gco(u);
  luaC_link(L, obj2gco(u), LUA_TUSERDATA);
  setbit(u->uv.marked, FIXEDBIT);
  setbit(L->marked, FIXEDBIT);
  stack_init(L, L);  /* init stack */
  sethvalue(L, gt(L), luaH_new(L, 0, 4));  /* table of globals */
  hvalue(gt(L))->metatable = luaH_new(L, 0, 0);  /* globals metatable */
  sethvalue(L, registry(L), luaH_new(L, 4, 4));  /* registry */
  luaS_resize(L, MINSTRTABSIZE);  /* initial size of string table */
  luaT_init(L);
  luaX_init(L);
  luaS_fix(luaS_newliteral(L, MEMERRMSG));
  g->GCthreshold = 4*g->totalbytes;
  g->prevestimate = g->estimate = g->totalbytes;
}


static void preinit_state (lua_State *L, global_State *g) {
  L->l_G = g;
  L->tt = LUA_TTHREAD;
  L->marked = luaC_white(g);
  L->stack = NULL;
  L->stacksize = 0;
  L->errorJmp = NULL;
  L->hook = NULL;
  L->hookmask = 0;
  L->basehookcount = 0;
  L->allowhook = 1;
  resethookcount(L);
  L->openupval = NULL;
  L->size_ci = 0;
  L->nCcalls = 0;
  L->isSuspended = 0;
  L->base_ci = L->ci = NULL;
  L->errfunc = 0;
  setnilvalue(gt(L));
}


static void close_state (lua_State *L) {
  global_State *g = G(L);
  luaF_close(L, L->stack);  /* close all upvalues for this thread */
  luaC_freeall(L);  /* collect all objects */
  lua_assert(g->rootgc == NULL);
  lua_assert(g->strt.nuse == 0);
  luaM_freearray(L, G(L)->strt.hash, G(L)->strt.size, TString *);
  luaZ_freebuffer(L, &g->buff);
  freestack(L, L);
  lua_assert(g->totalbytes == sizeof(LG));
  (*g->realloc)(g->ud, fromstate(L), state_size(LG), 0);
}


lua_State *luaE_newthread (lua_State *L) {
  lua_State *L1 = tostate(luaM_malloc(L, state_size(lua_State)));
  L1->next = L->next;  /* link new thread after `L' */
  L->next = obj2gco(L1);
  preinit_state(L1, G(L));
  stack_init(L1, L);  /* init stack */
  setobj2n(L, gt(L1), gt(L));  /* share table of globals */
  L1->hookmask = L->hookmask;
  L1->basehookcount = L->basehookcount;
  L1->hook = L->hook;
  resethookcount(L1);
  lua_assert(iswhite(obj2gco(L1)));
  return L1;
}


void luaE_freethread (lua_State *L, lua_State *L1) {
  luaF_close(L1, L1->stack);  /* close all upvalues for this thread */
  lua_assert(L1->openupval == NULL);
  freestack(L, L1);
  luaM_free(L, fromstate(L1), state_size(lua_State));
}


LUA_API lua_State *lua_newstate (lua_Alloc f, void *ud) {
  lua_State *L;
  global_State *g;
  void *l = (*f)(ud, NULL, 0, state_size(LG));
  if (l == NULL) return NULL;
  L = tostate(l);
  g = &((LG *)L)->g;
  L->next = NULL;
  g->currentwhite = bitmask(WHITE0BIT);
  preinit_state(L, g);
  g->realloc = f;
  g->ud = ud;
  g->mainthread = L;
  g->GCthreshold = 0;  /* mark it as unfinished state */
  g->strt.size = 0;
  g->strt.nuse = 0;
  g->strt.hash = NULL;
  setnilvalue(registry(L));
  luaZ_initbuffer(L, &g->buff);
  g->panic = NULL;
  g->gcstate = GCSpause;
  g->gcgenerational = 0;
  g->rootgc = obj2gco(L);
  g->sweepstrgc = 0;
  g->sweepgc = &g->rootgc;
  g->firstudata = NULL;
  g->gray = NULL;
  g->grayagain = NULL;
  g->weak = NULL;
  g->tmudata = NULL;
  setnilvalue(gkey(g->dummynode));
  setnilvalue(gval(g->dummynode));
  g->dummynode->next = NULL;
  g->totalbytes = sizeof(LG);
  if (luaD_rawrunprotected(L, f_luaopen, NULL) != 0) {
    /* memory allocation error: free partial state */
    close_state(L);
    L = NULL;
  }
  else
    lua_userstateopen(L);
  return L;
}


static void callallgcTM (lua_State *L, void *ud) {
  UNUSED(ud);
  luaC_callGCTM(L);  /* call GC metamethods for all udata */
}


LUA_API void lua_close (lua_State *L) {
  lua_lock(L);
  L = G(L)->mainthread;  /* only the main thread can be closed */
  luaF_close(L, L->stack);  /* close all upvalues for this thread */
  luaC_separateudata(L, 1);  /* separate udata that have GC metamethods */
  L->errfunc = 0;  /* no error function during GC metamethods */
  do {  /* repeat until no more errors */
    L->ci = L->base_ci;
    L->base = L->top = L->ci->base;
    L->nCcalls = 0;
  } while (luaD_rawrunprotected(L, callallgcTM, NULL) != 0);
  lua_assert(G(L)->tmudata == NULL);
  close_state(L);
}

