//=============================================================================
//  Filename:   UIColorAnimatorWrapper.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  ��������� ��� LightAnimLibrary, � ������� ����������� ����� �������
//	������ � ���������, � ��������� ������ ������� ����������� ��� UI: ���
//	��� UI ���������� �� �� ����� �����, �� ������� ������� ������������
//	Device.fTimeDelta ������ - �������� ����������� ����������. ���������� 
//	��������� ������� ����� ����� ���������.
//=============================================================================

#include "stdafx.h"
#include "UIColorAnimatorWrapper.h"
#include "../../LightAnimLibrary.h"

//////////////////////////////////////////////////////////////////////////

CUIColorAnimatorWrapper::CUIColorAnimatorWrapper()
	:	colorAnimation		(NULL),
		animationTime		(0.0f),
		color				(NULL),
		isDone				(false)
{
	prevGlobalTime	= Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

CUIColorAnimatorWrapper::CUIColorAnimatorWrapper(const shared_str &animationName, u32 *colorToModify)
	:	colorAnimation		(LALib.FindItem(*animationName)),
		animationTime		(0.0f),
		color				(colorToModify),
		isDone				(false)
{
	VERIFY(colorAnimation);
	VERIFY(color);
	prevGlobalTime	= Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

CUIColorAnimatorWrapper::CUIColorAnimatorWrapper(const shared_str &animationName)
	:	colorAnimation		(LALib.FindItem(*animationName)),
		animationTime		(0.0f),
		color				(NULL),
		isDone				(false)
{
	VERIFY(colorAnimation);
	prevGlobalTime	= Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::SetColorAnimation(const shared_str &animationName)
{
	if (animationName.size() != 0)
	{
		colorAnimation	= LALib.FindItem(*animationName);
		R_ASSERT2(colorAnimation, *animationName);
	}
	else
	{
		colorAnimation	= NULL;
	}
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::SetColorToModify(u32 *colorToModify)
{
	color = colorToModify;
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::Update()
{
	if (colorAnimation && !isDone)
	{
		if (!isCyclic)
		{
			if (animationTime < (colorAnimation->iFrameCount / colorAnimation->fFPS))
			{
				currColor		= colorAnimation->CalculateBGR(animationTime, currFrame);
//				Msg("frame: %i", dummy);
				currColor		= color_rgba(color_get_B(currColor), color_get_G(currColor), color_get_R(currColor), color_get_A(currColor));
				// ������� �����
				animationTime	+= Device.fTimeGlobal - prevGlobalTime;
			}
			else
			{
				// � ����� ������ (��� ����� ���) ��������� ������ ������ ���� ��������� ���� ��������
				if (currFrame != colorAnimation->iFrameCount - 1)
				{
					currColor	= colorAnimation->CalculateBGR(colorAnimation->iFrameCount / colorAnimation->fFPS, currFrame);
					currColor	= color_rgba(color_get_B(currColor), color_get_G(currColor), color_get_R(currColor), color_get_A(currColor));
				}
				// ���������� ����� ��������
				isDone = true;
			}
		}
		else
		{
			currColor	= colorAnimation->CalculateBGR(Device.fTimeGlobal, currFrame);
			currColor	= color_rgba(color_get_B(currColor), color_get_G(currColor), color_get_R(currColor), color_get_A(currColor));
		}

		if (color)
		{
			*color		= currColor;
		}
	}

	prevGlobalTime = Device.fTimeGlobal;
}

//////////////////////////////////////////////////////////////////////////

void CUIColorAnimatorWrapper::Reset()
{
	prevGlobalTime	= Device.fTimeGlobal;
	animationTime	= 0.0f;
	isDone			= false;
}

//////////////////////////////////////////////////////////////////////////

int CUIColorAnimatorWrapper::TotalFrames() const
{
	if (colorAnimation)
	{
		return colorAnimation->iFrameCount;
	}
	return 0;
}