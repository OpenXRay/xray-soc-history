#include "stdafx.h"
#include "uizonemap.h"

#include "hudmanager.h"


#include "InfoPortion.h"
#include "Pda.h"

#include "Grenade.h"
#include "level.h"
#include "game_cl_base.h"

#include "actor.h"
#include "ai_space.h"
#include "game_graph.h"

#include "ui/UIMap.h"
#include "ui/UIXmlInit.h"
//////////////////////////////////////////////////////////////////////////

CUIZoneMap::CUIZoneMap()
{}

CUIZoneMap::~CUIZoneMap()
{
	
}

void CUIZoneMap::Init()
{
	string256			gameLtxPath;
	FS.update_path		(gameLtxPath, CONFIG_PATH, "game.ltx");
	CInifile			gameLtx		(gameLtxPath);


	CUIXml uiXml;
	bool xml_result			= uiXml.Init(CONFIG_PATH, UI_PATH, "zone_map.xml");
	R_ASSERT3(xml_result, "xml file not found", "zone_map.xml");

	// load map background
	CUIXmlInit xml_init;
	xml_init.InitStatic(uiXml, "minimap:background", 0, &m_background);

	xml_init.InitStatic(uiXml, "minimap:background:level_frame", 0, &m_clipFrame);
	m_background.AttachChild(&m_clipFrame);

	xml_init.InitStatic(uiXml, "minimap:center", 0, &m_center);
	

	
	m_activeMap = xr_new<CUIMiniMap>();
	m_activeMap->SetAutoDelete(true);
	m_activeMap->Init(Level().name(),gameLtx,"hud\\default");
	m_clipFrame.AttachChild(m_activeMap);
	m_activeMap->SetClipRect( m_clipFrame.GetAbsoluteRect() );
	
	Fvector2	wnd_size;
	float zoom_factor = float(m_clipFrame.GetWndRect().width())/100.0f;
	wnd_size.x	= m_activeMap->BoundRect().width()*zoom_factor;
	wnd_size.y	= m_activeMap->BoundRect().height()*zoom_factor;
	m_activeMap->SetWndSize(wnd_size);
//	m_activeMap->SetZoomFactor( float(m_clipFrame.GetWndRect().width())/100.0f );
	m_activeMap->EnableHeading(true);  
	xml_init.InitStatic(uiXml, "minimap:compass", 0, &m_compass);
	m_compass.EnableHeading(true);
//	m_background.AttachChild(&m_compass);

	m_clipFrame.AttachChild(&m_center);
	m_center.SetWndPos(m_clipFrame.GetWidth()/2,m_clipFrame.GetHeight()/2);
}

void CUIZoneMap::Render			()
{
	m_background.Draw();
	m_compass.Draw();
}

void CUIZoneMap::SetHeading		(float angle)
{
	m_activeMap->SetHeading(angle);
	m_compass.SetHeading(angle);
};

void CUIZoneMap::UpdateRadar		(CActor* Actor)
{
	m_background.Update();
	m_activeMap->SetActivePoint( Actor->Position() );
}

bool CUIZoneMap::ZoomIn()
{
	return true;
}

bool CUIZoneMap::ZoomOut()
{
	return true;
}
