// UIMainIngameWnd.h:  ������-���������� � ����
// 
//////////////////////////////////////////////////////////////////////

#pragma once

#include "UIStatic.h"
#include "UIButton.h"
#include "UIFrameWindow.h"
#include "UIDragDropItem.h"
#include "UIDragDropList.h"
#include "UIProgressBar.h"
#include "UIWpnDragDropItem.h"
#include "UIListWnd.h"

#include "../UIZoneMap.h"

#include "../actor.h"
#include "../weapon.h"

//��� ������ ��������� HUD
extern int				g_bHudAdjustMode;
extern float			g_fHudAdjustValue;

class CUIMainIngameWnd: public CUIWindow  
{
public:
	CUIMainIngameWnd();
	virtual ~CUIMainIngameWnd();

	virtual void Init();
	//virtual void SendMessage(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void Draw();
	virtual void Update();

	bool OnKeyboardPress(int dik);

	//��� ����������� ��������� ���������� �� PDA
	void ReceivePdaMessage(CInventoryOwner* pSender, EPdaMsg msg, int info_index);
	
	void AddGameMessage	(CInventoryOwner* pSender, LPCSTR TextMessage);

protected:
	void HideAll();
	void ShowAll();
	bool m_bShowHudInfo;
	bool m_bShowHudCrosshair;

	CUIStatic	UIStaticHealth;
	CUIStatic	UIStaticMapBack;
	CUIStatic	UIStaticRadiationLow;
	CUIStatic	UIStaticRadiationMedium;
	CUIStatic	UIStaticRadiationHigh;
	CUIStatic	UIStaticWound;
	// ������ ������� ��� ����������� ��������� �������� ��� ��������� ������� �� ������
	// ������ ������, ��� � ������� ��� ��� ������� ������������ ������
	CUIButton	UIStaticQuickHelp;
	
	CUIStatic	UITextWound;

	CUIProgressBar UIHealthBar;

	CUIZoneMap UIZoneMap;

	//������, ������������ ���������� �������� PDA
	CUIStatic			UIPdaOnline;
	//������ �������� ��������� PDA
	CUIListWnd			UIPdaMsgListWnd;
	
	//�������� ����������� ���������
	int					m_dwMaxShowTime;
	//����� ������� ������������ ����� ��������� PDA
	int					m_dwMsgShowingTime;

	//����������� ������
	CUIStatic			UIWeaponBack;
	CUIStatic			UIWeaponSignAmmo;
	CUIStatic			UIWeaponSignName;
	CUIStatic			UIWeaponIcon;
	//CUIDragDropItem	UIWeaponIcon;

	

	//��� �������� ��������� ������ � ������
	CActor*				m_pActor;	
	CWeapon*			m_pWeapon;
	int					m_iWeaponIconX;
	int					m_iWeaponIconY;
	int					m_iWeaponIconWidth;
	int					m_iWeaponIconHeight;

	// ��������� ��� ��������� fadein/fadeout ������������ ���������
	float				fuzzyShowInfo;
	// ����������� ��������� ��� ��������� ������� �� ������
	// ��� ��������� ����� ������ ����� ����� ��������� CHUDCursor::Render().
	// ��������, ���������� ������� ���� 1 ��������� ���������.
	void RenderQuickInfos();

	// ������������� ������ � �������. ����� ��� �� map, ��� ��� ���������� ���������� 1 ���,
	// � ����� ����� ������������� �����.
	std::map<int, ref_str>	m_strTips;

	// ��� ���� ��������� �� ������ ������������� ����� ������������ ������������ �������
	int					m_iFade_mSec;
};