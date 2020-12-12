// AI_NaviPlane.cpp: implementation of the CAI_NaviPlane class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AI_NaviPlane.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_NaviPlane::CAI_NaviPlane()
{
	
}

CAI_NaviPlane::~CAI_NaviPlane()
{
	
}

void CAI_NaviPlane::MakeHierrarhy()
{
	// make sure everything is cleaned up
	VERIFY(nodes.empty());
	
	//*********** first: create nodes for all our obstacles
	CAI_NaviNode    Scene;
	float   _min	= flt_max;
	for (int i=0; i<int(obstacles.size()); i++)
	{
		CAI_NaviNode* N = new CAI_NaviNode;
		N->MakeLeaf(obstacles[i]);
		if (N->Radius()<_min) _min = N->Radius();
		nodes.push_back(N);
		Scene.Register(N);
	}
	Scene.CalcBounds();
	
	//*********** second: build hierrarhy itself
	float		delimiter = Scene.Radius()*2;
	float	    SizeLimit = delimiter;
	while (SizeLimit>_min) SizeLimit/=2;
	
	int		    iLevel      = 1;
	for         (SizeLimit-=0.01f; SizeLimit<=delimiter; SizeLimit*=2.f)
	{
		int iSize			= nodes.size();
		
		vector<CAI_NaviNode*>   nodes_sorted = nodes;
		for (int I=0; I<iSize; I++)
		{
			if (nodes[I]->bConnected) continue;
			
			// Connect first node
			CAI_NaviNode*    N = new CAI_NaviNode;
			if (!N->TryExpand(nodes[I],SizeLimit)) {
				delete N;
				continue;
			}
			
			// Find best object to connect with
			sort_base.set(N->Center());
			std::sort(nodes_sorted.begin(),nodes_sorted.end(),sort_pred);
			
			for (int J=0; J<iSize; J++)
			{
				if ( nodes_sorted[J]->bConnected) continue;
				N->TryExpand(nodes_sorted[J],SizeLimit);
			}
			
			if (N->chieldCount()>1) {
				nodes.push_back(N);
			} else {
				delete N;
				nodes[I]->bConnected = false;
			}
		}
		
//		_OUT(tmp.sprintf("#%2d [%3.1f]: %d nodes",iLevel,SizeLimit,nodes.size()-iSize));
		if (iSize != int(nodes.size())) iLevel++;
	}
	root = nodes.back();
//	_OUT(tmp.sprintf("* TREE levels %d, TREE size %d",iLevel,nodes.size()));
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

void	CAI_NaviPlane::FindWayToPlane(CAI_NaviPlane::PlaneFind &query)
{
	if (query.dest_id == self_id)
	{
		// if we get here - cost already computed for us
		// and - it is the best one
		query.c_best	= query.c_current;
		query.p_best	= query.p_current;
		return;
	}

	// Check if it is already processed
	vector<int>::iterator F = find(query.p_current.begin(),query.p_current.end(),self_id);
	if (F!=query.p_current.end()) return;

	query.p_current.push_back(self_id);
	vector<PlaneLink>::iterator I = links.begin();
	for (;I!=links.end();I++)
	{
		vector<int>
	}
	query.p_current.pop_back();
}
