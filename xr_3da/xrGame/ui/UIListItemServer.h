#pragma once

#include "UIListItem.h"


struct LIST_SRV_SIZES {
	int icon;
	int server;
	int map;
	int game; 
	int players;
	int ping;
};

struct LIST_SRV_INFO{
	shared_str server;
	shared_str map;
	shared_str game;
	shared_str players;
	shared_str ping;
};

struct LIST_SRV_ITEM {
	u32				color;
	CGameFont*		font;
	LIST_SRV_SIZES	size;
	LIST_SRV_INFO	info;
};

class CUIListItemServer : public CUIListItem {
public:
	// CUISimpleWindow
	using CUIWindow::Init;

	virtual void Draw();

	// own
	virtual void Init(LIST_SRV_ITEM& params, int x, int y, int width, int height);
	CUIListItemServer();

protected:
			void SetTextColor(u32 color);
			void SetFont(CGameFont* pFont);
	LIST_SRV_SIZES m_sizes;

	CUIStatic m_icon;
	CUIStatic m_server;
	CUIStatic m_map;
	CUIStatic m_game;
	CUIStatic m_players;
	CUIStatic m_ping;

};