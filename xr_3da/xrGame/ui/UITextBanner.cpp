//=============================================================================
//  �������� ��������� ������ � ���������� ������
//=============================================================================

#include "stdafx.h"
#include "UITextBanner.h"
#include "../UI.h"

CUITextBanner::CUITextBanner()
	:	m_bAnimate			(true),
		m_Cl				(0xffffffff),
		m_pFont				(NULL),
		m_bNewRenderMethod	(false),
		fontSize			(-1.0f),
		aligment			(CGameFont::alLeft)
{
}

////////////////////////////////////////////////////////////////////////////////

CUITextBanner::~CUITextBanner()
{
}

////////////////////////////////////////////////////////////////////////////////

EffectParams * CUITextBanner::SetStyleParams(const TextBannerStyles styleName)
{
	if (tbsNone == styleName)
	{
		if (!m_StyleParams.empty())
			m_StyleParams.clear();
		return NULL;
	}

	return &m_StyleParams[styleName];
}

////////////////////////////////////////////////////////////////////////////////

void CUITextBanner::Update()
{
	StyleParams_it it = m_StyleParams.begin();

	// ���� �������� ��������
	if (m_bAnimate)
	{
		for (; it != m_StyleParams.end(); ++it)
			if (it->second.bOn)
				it->second.fTimePassed += Device.fTimeDelta;
	}
}

////////////////////////////////////////////////////////////////////////////////

void CUITextBanner::Out(float x, float y, const char *fmt, ...)
{
	if (!fmt) return;

	StyleParams_it it = m_StyleParams.begin();

	// ��������� �������
	for (; it != m_StyleParams.end(); ++it)
	{
		// Fade effect
		if (it->first & tbsFade)
		{
			EffectFade();
		}
		// Flicker effect
		if (it->first & tbsFlicker)
		{
			EffectFlicker();
		}
	}

	va_list		Print;                                                                  
	string256	msg;
	xr_string buf;

	va_start(Print, fmt);
		vsprintf(msg, fmt, Print);                   
		buf += msg;
	va_end(Print);

	R_ASSERT(m_pFont);
	m_pFont->SetColor(m_Cl);
	m_pFont->SetAligment(aligment);
	if(fontSize>0.0f)
		m_pFont->SetSize(fontSize);

	// ���� ���������� ������ ��������, �� ������������ ��������� ������ ������
	if (x >= 1.0f && y >= 1.0f && Device.dwHeight < UI_BASE_HEIGHT)
	{
		x *= static_cast<float>(Device.dwWidth) / UI_BASE_WIDTH;
		y *= static_cast<float>(Device.dwHeight) / UI_BASE_HEIGHT;
	}
	m_pFont->Out(x, y, buf.c_str());
	if (m_bNewRenderMethod)
		m_pFont->OnRender();
}

////////////////////////////////////////////////////////////////////////////////

void CUITextBanner::EffectFade()
{
	EffectParams	&fade = m_StyleParams[tbsFade];

	// �������� �������� �� ��������
	if (!fade.bOn) return;

	// ���� ������ ����� ������� ����������� �����
	if (fade.fTimePassed > fade.fPeriod)
	{
		if (!fade.bCyclic)
		{
			fade.bOn = false;
			return;
		}

		if (0 == fade.iEffectStage)
			fade.iEffectStage = 1;
		else
			fade.iEffectStage = 0;
		fade.fTimePassed = 0;
	}

	if (fade.iEffectStage)
	{
		m_Cl = subst_alpha(GetTextColor(), u8(iFloor(255.f*(fade.fTimePassed / fade.fPeriod))));
	}
	else
	{
		m_Cl = subst_alpha(GetTextColor(), u8(iFloor(255.f*(1 - (fade.fTimePassed / fade.fPeriod)))));
	}
}

//////////////////////////////////////////////////////////////////////////

void CUITextBanner::EffectFlicker()
{
	EffectParams	&flicker = m_StyleParams[tbsFlicker];

	// �������� �������� �� ��������
	if (!flicker.bOn) return;

	// ���� ������ �����, ��������/�������� �������
	if (flicker.fTimePassed > flicker.fPeriod)
	{
		if (!flicker.bCyclic)
		{ 
			flicker.bOn = false;
			return;
		}
		
		if (0 == flicker.iEffectStage)
			flicker.iEffectStage = 1;
		else
			flicker.iEffectStage = 0;
		flicker.fTimePassed = 0;
	}

	if (flicker.iEffectStage)
	{
		m_Cl = subst_alpha(GetTextColor(), 0);
	}
	else
	{
		m_Cl = subst_alpha(GetTextColor(), 255);
	}
}

//////////////////////////////////////////////////////////////////////////

void CUITextBanner::SetTextColor(u32 cl)
{

	m_Cl = cl;
}

//////////////////////////////////////////////////////////////////////////

u32  CUITextBanner::GetTextColor()
{
	void SetTextColor			(u32 cl);
	u32  GetTextColor			();

	return m_Cl;
}

//////////////////////////////////////////////////////////////////////////

void CUITextBanner::ResetAnimation(const TextBannerStyles styleName)
{
	m_StyleParams[styleName].fTimePassed = 0;
}