#include "stdafx.h"
#include "UITDMFragList.h"
#include "hudmanager.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUITDMFragList::CUITDMFragList()
{
	m_u8Team = 0;
}
//--------------------------------------------------------------------

void CUITDMFragList::Init(u8 Team, u32 X, u32 Y, u32 Width, u32 Height, EUIItemAlign Align)
{
	inherited::Init		("ui\\ui_hud_frame",X, Y, Width, Height, Align);

	m_u8Team = Team;
}
//--------------------------------------------------------------------
IC bool	pred_player		(LPVOID v1, LPVOID v2)
{
	return ((game_cl_GameState::Player*)v1)->kills>((game_cl_GameState::Player*)v2)->kills;
}
void CUITDMFragList::OnFrame()
{
	inherited::OnFrame();
	xr_map<u32,game_cl_GameState::Player>::iterator I=Game().players.begin();
	xr_map<u32,game_cl_GameState::Player>::iterator E=Game().players.end();

	// create temporary map (sort by kills)
	items.clear			();
	for (;I!=E;++I)		items.push_back(&I->second);
	std::sort			(items.begin(),items.end(),pred_player);

	// out info
	CGameFont* H		= HUD().pFontSmall;
	H->OutSet			(float(list_rect.lt.x),float(list_rect.lt.y));
	// global info
	if (Game().fraglimit)	H->OutNext	("Frag Limit:  %3d",Game().fraglimit);
	else					H->OutNext	("Frag Limit:  unlimited");
	if (Game().timelimit)	H->OutNext	("Time remain: %3d (sec)",(Game().timelimit-(Level().timeServer()-Game().start_time))/1000);
	else					H->OutNext	("Time remain: unlimited");
	H->OutNext	("Team DeathMatch");

	H->OutSkip			(1.5f);
	int k=1;
	for (ItemIt mI=items.begin(); items.end() != mI; ++mI){
		game_cl_GameState::Player* P = (game_cl_GameState::Player*)*mI;
		if (P->team != m_u8Team) continue;
		u32	color = 0;
		if (P->flags&GAME_PLAYER_FLAG_LOCAL)	color = 0xf0a0ffa0; //H->SetColor(0xf0a0ffa0);
		else									color = 0xf0a0a0ff; //H->SetColor(0xe0a0eea0);		
		H->SetColor(color);
		H->OutNext		("%3d: %-20s %-20d %-5d",k++,P->name,P->kills, P->ping);
	}
}
//--------------------------------------------------------------------
