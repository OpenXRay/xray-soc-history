#include "stdafx.h"
#include "EffectorBobbing.h"


#include "actor.h"
#include "actor_defs.h"


#define BOBBING_SECT "bobbing_effector"

#define CROUCH_FACTOR	0.75f
#define SPEED_REMINDER	5.f 



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectorBobbing::CEffectorBobbing() : CCameraEffector(eCEBobbing,10000.f,FALSE)
{
	fTime			= 0;
	fReminderFactor	= 0;
	is_limping		= false;

	m_fAmplitudeRun		= pSettings->r_float(BOBBING_SECT, "run_amplitude");
	m_fAmplitudeWalk	= pSettings->r_float(BOBBING_SECT, "walk_amplitude");
	m_fAmplitudeLimp	= pSettings->r_float(BOBBING_SECT, "limp_amplitude");

	m_fSpeedRun			= pSettings->r_float(BOBBING_SECT, "run_speed");
	m_fSpeedWalk		= pSettings->r_float(BOBBING_SECT, "walk_speed");
	m_fSpeedLimp		= pSettings->r_float(BOBBING_SECT, "limp_speed");
}

CEffectorBobbing::~CEffectorBobbing	()
{
}

void CEffectorBobbing::SetState(u32 mstate, bool limping){
	dwMState		= mstate;
	is_limping		= limping;
}


BOOL CEffectorBobbing::Process		(Fvector &p, Fvector &d, Fvector &n, float& /**fFov/**/, float& /**fFar/**/, float& /**fAspect/**/)
{
	fTime			+= Device.fTimeDelta;
	if (dwMState&ACTOR_DEFS::mcAnyMove){
		if (fReminderFactor<1.f)	fReminderFactor += SPEED_REMINDER*Device.fTimeDelta;
		else						fReminderFactor = 1.f;
	}else{
		if (fReminderFactor>0.f)	fReminderFactor -= SPEED_REMINDER*Device.fTimeDelta;
		else						fReminderFactor = 0.f;
	}
	if (!fsimilar(fReminderFactor,0)){
		Fmatrix		M;
		M.identity	();
		M.j.set		(n);
		M.k.set		(d);
		M.i.crossproduct(n,d);
		M.c.set		(p);
		
		// apply footstep bobbing effect
		Fvector dangle;
		float k		= ((dwMState& ACTOR_DEFS::mcCrouch)?CROUCH_FACTOR:1.f);

		float A, ST;

		if(CActor::isAccelerated(dwMState))
		{
			A	= m_fAmplitudeRun*k;
			ST	= m_fSpeedRun*fTime*k;
		}
		else if(is_limping)
		{
			A	= m_fAmplitudeLimp*k;
			ST	= m_fSpeedLimp*fTime*k;
		}
		else
		{
			A	= m_fAmplitudeWalk*k;
			ST	= m_fSpeedWalk*fTime*k;
		}
	
		float _sinA	= _abs(_sin(ST)*A)*fReminderFactor;
		float _cosA	= _cos(ST)*A*fReminderFactor;

		p.y			+=	_sinA;
		dangle.x	=	_cosA;
		dangle.z	=	_cosA;
		dangle.y	=	_sinA;

		Fmatrix		R;
		R.setHPB	(dangle.x,dangle.y,dangle.z);

		Fmatrix		mR;
		mR.mul		(M,R);
		
		d.set		(mR.k);
		n.set		(mR.j);
	}
//	else{
//		fTime		= 0;
//	}
	return TRUE;
}
