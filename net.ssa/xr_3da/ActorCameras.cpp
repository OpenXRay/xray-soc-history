// Actor.cpp: implementation of the CActor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Actor.h"

void CActor::GetCamera(Fvector& P, Fvector& D, Fvector& N)
{
	// get calc eye point 90% from player height
	Fvector			R;
	Movement.Box().getsize	(R);
	cam_point.set	(0,R.y*0.9f,0.0f);
	mTransform.transform_tiny(cam_point);

	// soft crouch
	if (((eOldState==psCrouch)&&(eState==psStand))||
		((eState==psCrouch)&&(eOldState==psStand))||bEyeCrouch){
		float v_calc, v_res=5.f;
		v_calc			= (cam_point.y-fPrevCamPos)/Device.fTimeDelta;
		if (fabsf(v_calc)>EPS_L){
			if (fabsf(v_calc)<v_res) v_res=fabsf(v_calc);
			v_res		*= fabsf(v_calc)/v_calc;
			cam_point.y	= fPrevCamPos + v_res*Device.fTimeDelta;
			bEyeCrouch	= true;
		}else
			bEyeCrouch	= false;
	}
	// save previous position of camera
	fPrevCamPos=cam_point.y;
	// apply inertion
	cam_point.y = 0.5f*fPrevCamPos+0.5f*cam_point.y;

	// apply footstep bobbing effect
	Fvector offset, cam_dangle;
	offset.set(0,0,0);
	cam_dangle.set(0,0,0);
	fBobCycle+=Device.fTimeDelta;

	switch(eMoveState){
	case msWalk: 
		offset.y		= 0.1f*fabsf(sinf(7*fBobCycle)); 
		cam_dangle.x	= 0.01f*fabsf(cosf(fBobCycle*7));
		cam_dangle.z	= 0.01f*cosf(fBobCycle*7);
		if (eState==psCrouch)
			cam_dangle.y= 0.02f*sinf(fBobCycle*7);
		break;
	case msRun: 
		offset.y		= 0.01f*fabsf(sinf(11*fBobCycle)); 
		cam_dangle.x	= 0.01f*fabsf(cosf(fBobCycle*11));
		cam_dangle.z	= 0.01f*cosf(fBobCycle*11);
		if (eState==psCrouch)
			cam_dangle.y= 0.02f*sinf(fBobCycle*11);
		break;
	default: fBobCycle=0;
	}
	P.add(offset);

}

