// AI.h: interface for the CAI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI_H__985902E4_DDB4_4F2B_A647_20C173CDB5D9__INCLUDED_)
#define AFX_AI_H__985902E4_DDB4_4F2B_A647_20C173CDB5D9__INCLUDED_
#pragma once

#include "occluder.h"
#include "ai_frustum.h"
#include "ai_navigationpoint.h"
#include "ai_network.h"
#include "ai_fileformat.h"

class ENGINE_API CAI_Kernel
{
private:
//	COccluderSystem		Occluders;
	vector<CAI_NaviPoint>	NaviNetwork;

	void			SpatialBuild	();

	// Navigation
	vector<WORD>	PF_seen;		// which points we have already processed
	vector<WORD>	PF_best;		// best path
	int				PF_need;		// index of point we need to reach
	float			PFC_current;	// cost of current path
	float			PFC_best;		// cost of best path - smaller - better
	void			RecurseFindPath	(int from);
public:
	BOOL			CanISee			(CAI_Frustum &F, Fvector &C, float r);

	int				SelectNearest	(Fvector &pos);

	int				AdvancePath		(int point, Fvector &dest); // returns id
	VOID			FindPath		(int from, int to, vector<WORD> &result);

	CAI_NaviPoint&	PointByID		(int ID) { return NaviNetwork[ID]; }

	VOID			Load			();

	CAI_Kernel();
	~CAI_Kernel();
};

#endif // !defined(AFX_AI_H__985902E4_DDB4_4F2B_A647_20C173CDB5D9__INCLUDED_)
