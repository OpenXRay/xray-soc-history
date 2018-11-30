// AI.cpp: implementation of the CAI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "fstaticrender.h"
#include "AI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_Kernel::CAI_Kernel()
{

}

CAI_Kernel::~CAI_Kernel()
{

}

BOOL CAI_Kernel::CanISee(CAI_Frustum &F, Fvector &C, float r)
{
	if (!F.visible_point(C)) return FALSE;

	// Test occlusion
	pRender->Occluders.Select_AI(F.from,F);
	return pRender->Occluders.visibleSphere_AI(C,r);
}

VOID CAI_Kernel::Load()
{
	DWORD size;

	FILE_NAME LDesc;
	strcpy(LDesc,LEVEL_PATH);
	strcat(LDesc,"navigation.");

	CFileStream		fs(LDesc);

	// Header
	AI_FF_Header	H;
	fs.ReadChunk	(AIFF_Header,&H);
	R_ASSERT		(H.version==1);

	// Points
	vector<AI_NaviPoint>	NPoints;
	size		= fs.FindChunk(AIFF_NaviPoints);	R_ASSERT(size);
	R_ASSERT(size%sizeof(AI_NaviPoint) == 0);
	NPoints.resize(size/sizeof(AI_NaviPoint));
	fs.Read(NPoints.begin(),size);
	R_ASSERT(NPoints.size()<65535);

	// Network
	CAI_Network				NNetwork;
	size		= fs.FindChunk(AIFF_NaviNetwork);	R_ASSERT(size);
	NNetwork.INIT_by_count(NPoints.size());
	fs.Read(NNetwork.mem_pointer(),size);

	// Construct graph
	for (int i=0; i<NPoints.size(); i++)
	{
		CAI_NaviPoint P;
		NaviNetwork.push_back(P);
		NaviNetwork.back().type		= NPoints[i].type;
		NaviNetwork.back().position = NPoints[i].position;

		// determine count of connected points
		int	c_count = 0;
		for (int j=0; j<NPoints.size(); j++)
		{
			if (i==j) continue;
			if (NNetwork.get(i,j) || NNetwork.get(j,i)) c_count++;
		}
		R_ASSERT(c_count);

		// allocate memory
		NaviNetwork.back().Alloc(c_count);

		// transfer linked nodes
		for (j=0; j<NPoints.size(); j++)
		{
			if (i==j) continue;
			if (NNetwork.get(i,j) || NNetwork.get(j,i)) 
				NaviNetwork.back().Add(j);
		}

		// verify
		R_ASSERT(NaviNetwork.back().linked_count == c_count);
	}
	R_ASSERT(NaviNetwork.size() == NPoints.size());

	// Spatialize
	SpatialBuild	();
}

void CAI_Kernel::SpatialBuild()
{

}

void CAI_Kernel::RecurseFindPath	(int from)
{
	if (from==PF_need) {
		// destination found
		if (PFC_current<PFC_best) {
			PF_best		= PF_seen;
			PFC_best	= PFC_current;
			return;
		}
	}

	// Check if it is already processed
	CAI_NaviPoint&	P = NaviNetwork[from];

	// Recursive part here
	P.bProcessed = TRUE;		//*
	PF_seen.push_back(from);	//*

	for (int i=0; i<P.linked_count; i++)
	{
		int				ID	= P.linked_nodes[i];
		CAI_NaviPoint&	T	= NaviNetwork[ID];

		// skip already processed points
		if (T.bProcessed) continue;

		// calculate cost
		float path_cost	= P.position.distance_to(T.position);

		// branches & borders --- recursion depth limiter
		if ((PFC_current+path_cost)>PFC_best) continue;

		PFC_current += path_cost;
		RecurseFindPath(ID);
		PFC_current -= path_cost;
	}

	PF_seen.pop_back();			//*
	P.bProcessed = FALSE;		//*
}

VOID CAI_Kernel::FindPath(int from, int to, vector<WORD> &result)
{
	PF_seen.clear();
	PF_best.clear();
	PF_need		= to;
	PFC_current = 0;
	PFC_best	= flt_max;

	RecurseFindPath(from);

	result = PF_best;
	result.push_back(to);
}

int	CAI_Kernel::AdvancePath		(int point, Fvector &dest)
{
	// Check if it is already processed
	CAI_NaviPoint&	P	= NaviNetwork[point];
	float	sel_cost	= flt_max;
	int		sel_id		= -1;
	
	for (int i=0; i<P.linked_count; i++)
	{
		int				ID	= P.linked_nodes[i];
		CAI_NaviPoint&	T	= NaviNetwork[ID];

		// calculate cost
		float path_cost	= T.position.distance_to(dest);

		if (path_cost<sel_cost) {
			sel_cost	= path_cost;
			sel_id		= ID;
		}
	}
	VERIFY(sel_id>=0);
	return sel_id;
}
int	CAI_Kernel::SelectNearest	(Fvector &pos)
{
	float	best_dist	= flt_max;
	int		best_id		= -1;

	for (int i=0; i<NaviNetwork.size(); i++)
	{
		float dist = pos.distance_to_sqr(NaviNetwork[i].position);
		if (dist<best_dist) {
			best_dist	= dist;
			best_id		= i;
		}
	}
	return best_id;
}
