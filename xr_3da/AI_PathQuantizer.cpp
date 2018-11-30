// AI_PathQuantizer.cpp: implementation of the CAI_PathQuantizer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xr_creator.h"
#include "AI_PathQuantizer.h"

void t_spline(Fvector &dest, float t, Fvector &p0, Fvector &p1, Fvector &p2, Fvector &p3 ) {
	float		t2  = t * t;
    float		t3  = t2 * t;
	float		m[4];

	m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
    m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
    m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
    m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );

	dest.x = p0.x*m[0] + p1.x*m[1] + p2.x*m[2] + p3.x*m[3];
	dest.y = p0.y*m[0] + p1.y*m[1] + p2.y*m[2] + p3.y*m[3];
	dest.z = p0.z*m[0] + p1.z*m[1] + p2.z*m[2] + p3.z*m[3];
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_PathQuantizer::CAI_PathQuantizer()
{
}

CAI_PathQuantizer::~CAI_PathQuantizer()
{

}

void CAI_PathQuantizer::QuantizeLine	(Fvector& P1, Fvector& P2)
{
	float	length = P1.distance_to(P2);
	int		subdiv = int((length-sLastPathError)/AIPQ_QUANT);

	if		(subdiv<=0) {
		sPath.push_back(P2);
		return;
	}

	Fvector		D;
	D.sub		(P2,P1);
	D.normalize	();

	subdiv++;
	for (int I=0; I<subdiv; I++)
	{
		float dist = (I*AIPQ_QUANT)+sLastPathError;
//		float part = dist/length;

		Fvector P;
		P.direct(P1,D,dist);
		sPath.push_back(P);
	}

	// compute last quant error (in meters)
	float dist = length-(((subdiv-1)*AIPQ_QUANT)+sLastPathError);
	VERIFY(dist>0);
	VERIFY(dist<AIPQ_QUANT);
	sLastPathError = dist;
}

void CAI_PathQuantizer::OnMove()
{
	// Update time
	float time_scale = fSpeed/AIPQ_QUANT;
	sTime += Device.fTimeDelta*time_scale;
	if (sTime>1.f) {
		sTime-=1.f;

		sPath.pop_front();
	}
	if (sPath.size()<4) ContinuePath();
	if (sPath.size()<4) return;

	// Interpolate spline
	t_spline(sPosition,sTime,sPath[0],sPath[1],sPath[2],sPath[3]);
//	Log("* ",sPosition);
}

void CAI_PathQuantizer::ContinuePath()
{
	if (vTarget) {
		while (sPath.size()<4) {
			sLastNaviPoint		= pCreator->AI.AdvancePath(sLastNaviPoint,*vTarget);
			CAI_NaviPoint&	P	= pCreator->AI.PointByID(sLastNaviPoint);
			QuantizeLine	(sPath.back(),P.position);
		}
	}
}

void CAI_PathQuantizer::Reset(Fvector &pos, Fvector* target, float fSpeed )
{
	sLastPathError	= 0;
	sTime			= 0;
	sPath.clear		();
	sPosition.set	(pos);

	SetTarget		(target);
	SetSpeed		(fSpeed);

	sPath.push_back	(sPosition);
	sLastNaviPoint	= pCreator->AI.SelectNearest(sPosition);

	// Move to nearest navigation point
	CAI_NaviPoint&	P	= pCreator->AI.PointByID(sLastNaviPoint);
	QuantizeLine	(sPosition,P.position);

	// Extend path by moving to target
	ContinuePath	();
}
