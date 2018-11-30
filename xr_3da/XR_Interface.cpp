// XR_Interface.cpp: implementation of the CHUD class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_smallfont.h"
#include "xr_gamefont.h"
#include "xr_creator.h"
#include "x_ray.h"
#include "xr_ini.h"

DWORD psHUD_Flags = 0;

const float fade_speed = 8.0f;

void CContextMenu::Load(CInifile* INI, string &SECT)
{
	CInifile::Sect& S = INI->ReadSection(SECT);
	for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) 
	{
		char	Event[128],Param[128];
		Event[0]=0; Param[0]=0;
		sscanf	(I->second.c_str(),"%[^,],%s",Event,Param);
		MenuItem	Item;
		Item.Name	= I->first;
		Item.Event	= Engine.Event.Create(Event);
		Item.Param	= Param;
		Items.push_back(Item);
	}
}
void CContextMenu::Render(CFontBase* F, DWORD cT, DWORD cI, float s)
{
	F->Size		(0.05f);
	F->Color	(cT);
	F->OutNext	("%s",Name.c_str());
	F->Color	(cI);
	F->Size		(0.03f);
	for (int i=0; i<Items.size(); i++)
		F->OutNext("%d. %s", i, (char*)Items[i].Name.c_str());
}
void CContextMenu::Select(int I)
{
	if (I>=0 && I<Items.size())
	{
		MenuItem& M = Items[I];
		Engine.Event.Signal(M.Event, DWORD(M.Param.c_str()));
	}
}

//////////////////////////////////////////////////////////////////////
void CInfo::Load	(CInifile* INI, string& S)
{
	if (!INI->SectionExists(S)) return;

	// settings
	string& L = INI->ReadSTRING(S,"params");
	DWORD r,g,b;
	sscanf(L.c_str(),"%f,%f,%f,%d,%d,%d",&px,&py,&s,&r,&g,&b);
	c = D3DCOLOR_XRGB(r,g,b);

	// lines
	for (int i=0; ;i++)
	{
		char n[128];
		sprintf(n,"line%d",i);
		if (!INI->LineExists(S,string(n))) break;

		string& ln = INI->ReadSTRING(S,string(n));
		text.push_back(ln.substr(1,ln.length()-2));
	}
}

void CInfo::Render	(CFontBase* F)
{
	if (text.empty()) return;
	F->Color	(c);
	F->Size		(s);
	float		iy = py;
	for (int i=0; i<text.size(); i++) {
		F->Out(px,iy,(char*)text[i].c_str());
		iy += s*2.5f;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHUD::CHUD()
{
	pFont		= new CFontGame;
	bMenuActive	= FALSE;
	alpha		= 0;
	Current		= 0;

	eMenuChange	= Engine.Event.Handler_Attach("HUD:SETMENU",this);

	FrontView	= 0;

	/*
	Device.seqDevCreate.Add(this);
	Device.seqDevDestroy.Add(this);

	if (Device.bReady) OnDeviceCreate();
	*/
}

CHUD::~CHUD()
{
	/*
	Device.seqDevDestroy.Remove(this);
	Device.seqDevCreate.Remove(this);
	*/

	Engine.Event.Handler_Detach(eMenuChange,this);

	_DELETE(pFont);
}

void CHUD::Load	()
{
	CInifile MENU	("menu.ltx");
	CInifile::Sect& S = MENU.ReadSection("menus");
	for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) 
	{
		CContextMenu	M;
		M.Name			= I->first;
		M.Load			(&MENU, I->second);
		Menus.push_back	(M);
	}
	DWORD	mask = D3DCOLOR_RGBA(255,255,255,0);
	pos_x	= MENU.ReadFLOAT("settings","pos_x");
	pos_y	= MENU.ReadFLOAT("settings","pos_y");
	c_title	= MENU.ReadCOLOR("settings","c_title");	c_title	&= mask;
	c_items	= MENU.ReadCOLOR("settings","c_items");	c_items	&= mask;

	Info.Load		(&MENU,string("info"));
}

void CHUD::OnMove()
{
	VERIFY(pCreator);

	if (Current)
	{
		float amount = fade_speed*Device.fTimeDelta;
		if (bMenuActive) {
			alpha += amount;
			if (alpha>1) alpha = 1;
		} else {
			alpha -= amount;
			if (alpha<=0) {
				alpha	= 0;
				Current = 0;
			}
		}
	}
}

void CHUD::Render()
{
	if (Current) {
		DWORD A = uROUND(alpha*255.f);
		clamp(A,0ul,255ul);
		A = D3DCOLOR_RGBA(0,0,0,A);
		pApp->pFont->OutSet	(pos_x,pos_y);
		Current->Render(pApp->pFont,c_title|A,c_items|A,0);
	}
	if (psHUD_Flags&HUD_INFO)	Info.Render		(pFont);
	pFont->OnRender	();
}

void CHUD::OnEvent(EVENT E, DWORD P1, DWORD P2)
{
	if (E==eMenuChange) {
		char* name = (char*)P1;
		for (int i=0; i<Menus.size(); i++)
		{
			if (0==stricmp(name,Menus[i].Name.c_str()))
			{
				Current = &Menus[i];
				return;
			}
		}
		return;
	}
}

void CHUD::CM_Activate		()
{
	bMenuActive = TRUE;
	Engine.Event.Signal		(eMenuChange,(DWORD)"main");
	iCapture();
}
void CHUD::CM_Deactivate	()
{
	if (bMenuActive) {
		bMenuActive = FALSE;
		iRelease();
	}
}

void CHUD::OnDeviceCreate()
{
}

void CHUD::OnDeviceDestroy()
{
} 

void CHUD::OnKeyboardPress(int dik)
{
	switch (dik)
	{
	case DIK_0:	Current->Select(0); break;
	case DIK_1:	Current->Select(1); break;
	case DIK_2:	Current->Select(2); break;
	case DIK_3:	Current->Select(3); break;
	case DIK_4:	Current->Select(4); break;
	case DIK_5:	Current->Select(5); break;
	case DIK_6:	Current->Select(6); break;
	case DIK_7:	Current->Select(7); break;
	case DIK_8:	Current->Select(8); break;
	case DIK_9:	Current->Select(9); break;
	}
}

void CHUD::OnKeyboardRelease(int dik)
{
	switch (dik)
	{
	case DIK_LALT: CM_Deactivate(); break;
	}
}