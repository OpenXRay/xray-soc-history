// CustomFlyer.cpp: implementation of the CCustomFlyer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomFlyer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlyer::CFlyer() : inherited()
{

}

CFlyer::~CFlyer()
{

}

void CFlyer::Load(CInifile* ini, const char * section)
{
	Msg("Loading flyer: %s",section);
	inherited::Load(ini,section);
/*
	R_ASSERT	(pVisual->Type==MT_SKELETON);
	PKinematics	(pVisual)->LL_PlayCycle(
		-1,
		PKinematics(pVisual)->LL_MotionID("idle"),
		1,1,1
		);
*/
}

void CFlyer::OnMove()
{
	inherited::OnMove	();
}

void CFlyer::OnMoveVisible()
{
	inherited::OnMoveVisible();
}

void CFlyer::OnRender()
{
}

// input
void CFlyer::OnKeyboardPress(int cmd){
}

void CFlyer::OnKeyboardRelease(int cmd){
}

void CFlyer::OnKeyboardHold(int cmd){
}

void CFlyer::OnMouseMove(int dx, int dy)
{
}
