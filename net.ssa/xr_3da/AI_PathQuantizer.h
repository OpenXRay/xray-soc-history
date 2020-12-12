// AI_PathQuantizer.h: interface for the CAI_PathQuantizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI_PATHQUANTIZER_H__DC97E1D8_5F84_4262_8056_DA8EEA041DF0__INCLUDED_)
#define AFX_AI_PATHQUANTIZER_H__DC97E1D8_5F84_4262_8056_DA8EEA041DF0__INCLUDED_

#pragma once

#define AIPQ_QUANT	3.f		// in meters

class ENGINE_API CAI_PathQuantizer  
{
	float			fSpeed;
	Fvector*		vTarget;
	
	float			sLastPathError;
	float			sTime;	// 0..1
	deque<Fvector>	sPath;
	Fvector			sPosition;
	int				sLastNaviPoint;

	void	QuantizeLine	(Fvector& P1, Fvector& P2);
	void	ContinuePath	();
public:
	void	SetSpeed		(float V)			{ fSpeed = V; }
	void	SetTarget		(Fvector* target)	{ vTarget = target;	}

	void	GetOrientation	(Fvector& pos, Fvector& dir) {
		pos.set(sPosition);
		dir.set(0,1,0);
	}
	
	void	Reset			(Fvector &pos, Fvector* target, float fSpeed );
	void	OnMove			();

	CAI_PathQuantizer		();
	~CAI_PathQuantizer		();
};

#endif // !defined(AFX_AI_PATHQUANTIZER_H__DC97E1D8_5F84_4262_8056_DA8EEA041DF0__INCLUDED_)
