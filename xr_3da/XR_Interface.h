// XR_Interface.h: interface for the CXR_Interface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XR_INTERFACE_H__14F97BE0_CB90_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_XR_INTERFACE_H__14F97BE0_CB90_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

// refs
class CFontGame;
class CFontBase;

ENGINE_API extern DWORD psHUD_Flags;
#define HUD_CROSSHAIR			(1<<0)
#define HUD_CROSSHAIR_DIST		(1<<1)
#define HUD_WEAPON				(1<<2)
#define HUD_INFO				(1<<3)

class ENGINE_API CContextMenu
{
	friend class CHUD;
private:
	struct MenuItem
	{
		string			Name;
		EVENT			Event;
		string			Param;
	};
private:
	string				Name;
	vector<MenuItem>	Items;
public:
	void		Load	(CInifile* INI, string& S);
	void		Render	(CFontBase* F, DWORD cT, DWORD cI, float s);
	void		Select	(int num);
};
class ENGINE_API CInfo
{
	friend class CHUD;
private:
	float	px,py,s;
	DWORD	c;
	vector<string>		text;
public:
	void		Load	(CInifile* INI, string& S);
	void		Render	(CFontBase* F);
};

class ENGINE_API CHUD :
	public pureDeviceCreate,
	public pureDeviceDestroy,
	public CController,
	public CEventBase
{
	CFontGame*					pFont;
	vector<CContextMenu>		Menus;
	float						pos_x,pos_y;
	DWORD						c_title,c_items;

	BOOL						bMenuActive;

	Shader*						FrontView;

	float						alpha;
	CContextMenu*				Current;

	EVENT						eMenuChange;

	CInfo						Info;
public:
	virtual		void			OnDeviceCreate		();
	virtual		void			OnDeviceDestroy		();
	virtual		void			OnEvent				(EVENT E, DWORD P1, DWORD P2);
	virtual		void			OnKeyboardPress		(int dik);
	virtual		void			OnKeyboardRelease	(int dik);

	void		Load			();
	
	void		Render			();
	void		OnMove			();

	void		CM_Activate		();
	void		CM_Deactivate	();

	CHUD();
	~CHUD();
};

#endif // !defined(AFX_XR_INTERFACE_H__14F97BE0_CB90_11D3_B4E3_4854E82A090D__INCLUDED_)
