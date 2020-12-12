// CustomFlyer.h: interface for the CCustomFlyer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLYER_H__06360CF2_1B3F_4F82_814D_F7BF5E4F601C__INCLUDED_)
#define AFX_FLYER_H__06360CF2_1B3F_4F82_814D_F7BF5E4F601C__INCLUDED_
#pragma once

#include "entity.h"

class CFlyer : public CEntity, public pureRender  
{
	enum EMoveState{
		msIdle,
		msStill,
		msWalk,
		msRun,
		msJump
	};
	enum EFlyerCameras {
		efcFrontView,
		efcLookAt
	};

private:
	typedef CEntity		inherited;
protected:
	
	EFlyerCameras		cam_style;

public:
						CFlyer			();
	virtual				~CFlyer			();

	virtual void		Load			(CInifile* ini, const char* section);

	virtual void		GetCamera		(Fvector& P, Fvector& D, Fvector& N){};
	virtual void		GetFireParams	(Fvector &fire_pos, Fvector &fire_dir){};

	virtual void		OnMove			( );
	virtual void		OnMoveVisible	( );
//	virtual void		OnMoveSheduled	( int group, bool bFull );

	virtual void		OnMouseMove			(int x, int y);
	virtual void		OnKeyboardPress		(int dik);
	virtual void		OnKeyboardRelease	(int dik);
	virtual void		OnKeyboardHold		(int dik);

	virtual void		OnRender		();
};

#endif // !defined(AFX_FLYER_H__06360CF2_1B3F_4F82_814D_F7BF5E4F601C__INCLUDED_)
