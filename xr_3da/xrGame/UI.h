#ifndef __XR_UI_H__
#define __XR_UI_H__
#pragma once

#include "UICursor.h"
#include "UIZoneMap.h"
#include "UIWeapon.h"
#include "UIHealth.h"
#include "UIGroup.h"
#include "UIFragList.h"

#define UI_BASE_WIDTH	800
#define UI_BASE_HEIGHT	600

// refs
class CHUDManager;
class CWeapon;
#define MAX_GROUPS		10
#define MAX_UIMESSAGES	7
#define LIFE_TIME		7.f		// sec
#define HIDE_TIME		2.5f	// sec 

struct SUIMessage
{
	LPSTR sender;
	LPSTR msg;
	DWORD color;
	int life_time;
	SUIMessage(LPCSTR S, LPCSTR M, DWORD clr, float lt)
	{
		sender		= xr_strdup(S);
		msg			= xr_strdup(M);
		life_time	= int(lt*1000);
		color		= clr;
	}
	~SUIMessage()
	{
		_FREE(sender);
		_FREE(msg);
	}
};

DEFINE_SVECTOR(SUIMessage*,MAX_UIMESSAGES,UIMsgSVec,UIMsgIt);

class CUI{
	CUICursor			UICursor;
	CUIZoneMap			UIZoneMap;
	CUIWeapon			UIWeapon;
	CUIHealth			UIHealth;
	CUISquad			UISquad;

	CUIFragList			UIFragList;

	bool				bActive;

	CHUDManager*		m_Parent;

	// messages
	float				msgs_offs;
	float				menu_offs;
	UIMsgSVec			messages;

	// 
	bool				bShift;
	bool				bDrawFragList;

	// group management
	bool				bSelGroups			[MAX_GROUPS];

	void				ResetSelected		();
	bool				FindGroup			(int idx);
	void				SelectGroup			(int idx);
	void				SetState			(EGroupState st);
	void				SetFlag				(EGroupTriggers tr, BOOL f);
	void				InvertFlag			(EGroupTriggers tr);
public:
						CUI					(CHUDManager* p);
	virtual				~CUI				();

	bool				Render				();
	void				OnFrame				();

	bool				OnKeyboardPress		(int dik);
	bool				OnKeyboardRelease	(int dik);
	bool				OnMouseMove			(int dx, int dy);

	// --- depends on game type
	// frag		(g_fraglimit)
	// time		(g_timelimit)
	// frag-list.....

	void				AddMessage			(LPCSTR S, LPCSTR M, DWORD C=0xffffffff, float life_time=LIFE_TIME);

	void				Activate			();
	void				Deactivate			();
	bool				IsActive			(){return bActive;}
};

#endif // __XR_UI_H__
