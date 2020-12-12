/*
======================================================================
blotch.c

Stick a colored spot on a surface.

Allen Hastings, Arnie Cachelin, Stuart Ferguson, Ernie Wright
6 April 00

** Note:  The first release of LW 6.0 will crash on exit if this
plug-in's interface has been opened during that session.  The crash
is related to xpanel destroy processing.  It shouldn't cause loss of
data, and it should be resolved in later builds.
====================================================================== */

#include <lwserver.h>
#include <lwshader.h>
#include <lwsurf.h>
#include <lwhost.h>
#include <lwxpanel.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifndef PI
#define PI 3.1415926535897932384
#endif


/* our instance data */
/*
typedef struct st_XRShader {
	int		sh_idx;
	char	name[ 80 ];
	char	desc[ 80 ];
} XRShader;

typedef char sh_name[64];
typedef vector<sh_name> NamesList;
typedef NamesList::iterator NamesIt;

typedef struct st_EShaderList{
	NamesList	Names;
	sh_name		Get(int idx){ return (idx>=0)?Names[idx]:"(none)"; }
	int			Get(sh_name n){ 
					NamesIt n_it=find(Names.begin(),Names.end(),sh_name); 
					return (n_it==Names.end())?-1:n_it-Names.begin(); 
				}
	void		Clear	(){Names.clear();}
	void		Add		(sh_name n){Names.push_back(strlwr(n));}
}EShaderList;

EShaderList EShaders;
*/
/*
======================================================================
Create()

Handler callback.  Allocate and initialize instance data.

The create function allocates a blotch struct and returns the pointer
as the instance.  Note that "Blotch *" is used throughout instead of
"LWInstance".  This works since a LWInstance type is a generic pointer
and can safely be replaced with any specific pointer type.  Instance
variables are initialized to some default values.
====================================================================== */

XCALL_( static LWInstance )
Create( void *priv, LWSurfaceID surf, LWError *err )
{
   XRShader *inst;

   inst = calloc( 1, sizeof( XRShader ));
   if ( !inst ) {
      *err = "Couldn't allocate memory for instance.";
      return NULL;
   }

   strcpy(inst->name, "default");

   EShaders.Clear();
   EShaders.Add("default");
   EShaders.Add("font");
   EShaders.Add("wallmark");

   return inst;
}


/*
======================================================================
Destroy()

Handler callback.  Free resources allocated by Create().
====================================================================== */

XCALL_( static void )
Destroy( XRShader *inst )
{
   free( inst );
}


/*
======================================================================
Copy()

Handler callback.  Copy instance data.
====================================================================== */

XCALL_( static LWError )
Copy( XRShader *to, XRShader *from )
{
   XCALL_INIT;

   *to = *from;
   return NULL;
}


/*
======================================================================
Load()

Handler callback.  Read instance data.  Shader instance data is stored
in the SURF chunks of object files, but it isn't necessary to know
that to read and write the data.
====================================================================== */

XCALL_( static LWError )
Load( XRShader *inst, const LWLoadState *ls )
{
   LWLOAD_STR( ls, inst->name, sizeof( inst->name ));   // vmap name

   return NULL;
}


/*
======================================================================
Save()

Handler callback.  Write instance data.  The I/O functions in lwio.h
include one for reading and writing floats, but not doubles.  We just
transfer our double-precision data to a float variable before calling
the LWSAVE_FP() macro.
====================================================================== */

XCALL_( static LWError )
Save( XRShader *inst, const LWSaveState *ss )
{
   LWSAVE_STR( ss, inst->name );   // vmap name

   return NULL;
}


/*
======================================================================
DescLn()

Handler callback.  Write a one-line text description of the instance
data.  Since the string must persist after this is called, it's part
of the instance.
====================================================================== */

XCALL_( static const char * )
DescLn( XRShader *inst )
{
   sprintf( inst->desc, "XR Shader (%s)", inst->name );
      
   return inst->desc;
}


/*
======================================================================
Init()

Handler callback, called at the start of rendering.  We do a little
precalculation here.
====================================================================== */

XCALL_( static LWError )
Init( XRShader *inst, int mode )
{

   return NULL;
}


/*
======================================================================
Cleanup()

Handler callback, called at the end of rendering.  We don't have
anything to do, but it's here in case we want to add something later.
====================================================================== */

XCALL_( static void )
Cleanup( XRShader *inst )
{
   return;
}


/*
======================================================================
NewTime()

Handler callback, called at the start of each sampling pass.
====================================================================== */

XCALL_( static LWError )
NewTime( XRShader *inst, LWFrame f, LWTime t )
{
   return NULL;
}


/*
======================================================================
Flags()

Handler callback.  Blotch alters the color of the surface, but nothing
else, so we return just the color bit.
====================================================================== */

XCALL_( static unsigned int )
Flags( XRShader *inst )
{
   return LWSHF_COLOR;
}


/*
======================================================================
Evaluate()

Handler callback.  This is where the blotchiness actually happens.  We
compute the distance from the spot to be shaded to the center of the
blotch and blend some of the blotch color with the color already
computed for that spot.
====================================================================== */

XCALL_( static void )
Evaluate( XRShader *inst, LWShaderAccess *sa )
{
/*   double d, r2, a;
   int i;

   // Compute the distance from the center of the blotch to the spot
   // in object coordinates.  Exit early if the spot is clearly
   // outside the blotch radius.

   r2 = 0;
   for ( i = 0; i < 3; i++ ) {
      d = sa->oPos[ i ] - inst->center[ i ];
      d = d * d;
      if ( d > inst->r2 ) return;
      r2 += d;
   }
   if ( r2 > inst->r2 ) return;

   d = sqrt( r2 );
   if ( d > inst->radius ) return;

   // Using the distance in 'd', compute where this spot falls in the
   // blotch's soft edge.  The blotch is given by a cosine density
   // function scaled by the softness factor.  Where the density is
   // greater than 1.0, it clips. 

   d = pow( 0.5 * ( 1.0 + cos( d * inst->piOverR )), inst->softness );

   // Finally, blend the blotch color into the existing color using
   // the computed density. 

   a = 1.0 - d;
   for ( i = 0; i < 3; i++ )
      sa->color[ i ] = sa->color[ i ] * a + inst->color[ i ] * d;
*/
}


/*
======================================================================
Handler()

Handler activation function.  Check the version and fill in the
callback fields of the handler structure.
====================================================================== */

XCALL_( static int )
Handler( long version, GlobalFunc *global, LWShaderHandler *local,
   void *serverData)
{
   if ( version != LWSHADER_VERSION ) return AFUNC_BADVERSION;

   local->inst->create   = Create;
   local->inst->destroy  = Destroy;
   local->inst->load     = Load;
   local->inst->save     = Save;
   local->inst->copy     = Copy;
   local->inst->descln   = DescLn;
   local->rend->init     = Init;
   local->rend->cleanup  = Cleanup;
   local->rend->newTime  = NewTime;
   local->evaluate       = Evaluate;
   local->flags          = Flags;

   return AFUNC_OK;
}


/*
======================================================================
popName_VMAP()

Return the name of a vmap, given an index.  An xpanel callback for the
vmap popup list, also used by Load() and Save().
====================================================================== */

XCALL_( static const char * )
popName_SH( void *data, int idx )
{
   if ( idx == -1 ) return "(none)";

   return objf->vmapName( LWVMAP_WGHT, idx - 1 );
}


/* interface stuff ----- */

static LWXPanelFuncs *xpanf;
static LWColorActivateFunc *colorpick;
static LWInstUpdate *lwupdate;

enum { ID_NAME = 0x8001 };


/*
======================================================================
ui_get()

Xpanels callback for LWXP_VIEW panels.  Returns a pointer to the data
for a given control value.
====================================================================== */

void *ui_get( XRShader *dat, unsigned long vid )
{
   void *result = NULL;

   if ( dat )
      switch ( vid ) {
         case ID_NAME:
            result = &dat->sh_idx;
            break;
      }

   return result;
}


/*
======================================================================
ui_set()

Xpanels callback for LWXP_VIEW panels.  Store a value in our instance
data.
====================================================================== */

int ui_set( XRShader *dat, unsigned long vid, void *value )
{
   int rc = 0;

   if ( dat )
      switch ( vid ) {
         case ID_NAME:
            dat->sh_idx = *(( int * ) value );
         if(dat->sh_idx)
            strncpy(dat->vmName, popName_VMAP(dat,dat->vmapIdx) ,sizeof( dat->vmName ));
            rc = 1;
            break;
      }

   return rc;
}


/*
======================================================================
ui_chgnotify()

XPanel callback.  XPanels calls this when an event occurs that affects
the value of one of your controls.  We use the instance update global
to tell Layout that our instance data has changed.
====================================================================== */

void ui_chgnotify( LWXPanelID panel, unsigned long cid, unsigned long vid,
   int event )
{
   void *dat;

   if ( event == LWXPEVENT_VALUE )
      if ( dat = xpanf->getData( panel, 0 ))
         lwupdate( LWSHADER_HCLASS, dat );
}


/*
======================================================================
get_panel()

Create and initialize an LWXP_VIEW panel.  Called by Interface().
====================================================================== */
#define STR_Type_TEXT  "Shader Name"


LWXPanelID get_panel( XRShader *inst )
{
   static LWXPanelControl xctl[] = {
      { ID_NAME,     STR_Type_TEXT, "iPopChoice" },
      { 0 }
   };
   static LWXPanelDataDesc xdata[] = {
      { ID_NAME,     STR_Type_TEXT, "integer" },
      { 0 }
   };
   
   LWXPanelID panel;

   if ( panel = xpanf->create( LWXP_VIEW, xctl )) {
      //xpanf->hint( panel, 0, xhint );
      xpanf->describe( panel, xdata, ui_get, ui_set );
      xpanf->viewInst( panel, inst );
      xpanf->setData( panel, 0, inst );
   }

   return panel;
}


/*
======================================================================
Interface()

The interface activation function.
====================================================================== */

XCALL_( int )
Interface( long version, GlobalFunc *global, LWInterface *local,
   void *serverData )
{
   if ( version != LWINTERFACE_VERSION ) return AFUNC_BADVERSION;

   colorpick = global( LWCOLORACTIVATEFUNC_GLOBAL, GFUSE_TRANSIENT );
   lwupdate  = global( LWINSTUPDATE_GLOBAL,        GFUSE_TRANSIENT );
   xpanf     = global( LWXPANELFUNCS_GLOBAL,       GFUSE_TRANSIENT );
   if ( !colorpick || !lwupdate || !xpanf ) return AFUNC_BADGLOBAL;

   local->panel   = get_panel( local->inst );
   local->options = NULL;
   local->command = NULL;

   return local->panel ? AFUNC_OK : AFUNC_BADGLOBAL;
}


ServerRecord ServerDesc[] = {
   { LWSHADER_HCLASS, "XRShader", Handler },
   { LWSHADER_ICLASS, "XRShader", Interface },
   { NULL }
};
