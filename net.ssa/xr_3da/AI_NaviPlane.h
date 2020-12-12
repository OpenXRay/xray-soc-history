// AI_NaviPlane.h: interface for the CAI_NaviPlane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI_NAVIPLANE_H__71584EC5_DC4C_4033_A517_DDBC3472DD28__INCLUDED_)
#define AFX_AI_NAVIPLANE_H__71584EC5_DC4C_4033_A517_DDBC3472DD28__INCLUDED_
#pragma once

class ENGINE_API CAI_NaviPlane  
{
	friend class CAI_Navigation;
private:
	typedef vector<CAI_NaviNode*>	vecNodes;
	typedef vecNodes::iterator		itNodes;

	typedef set<CAI_NaviNode*>		setNodes;
	typedef setNodes::iterator		sitNodes;

private:
	int							self_id;
	Fmatrix						L2W,W2L;	// local to world, world to local
	float						dimX,dimY;	// dimensions (0,0)-(dimX,dimY)

    vector<CAI_NaviObstacle*>   obstacles;
    vecNodes					nodes;
    CAI_NaviNode*				root;

	setNodes					dynamics;	// all movers which are currently 
											// on this 'plane'
	vector<WORD>				naviLinks;
private:
    void						MakeHierrarhy();
public:
	// Loading
	void						Load(CStream* fs);

	// Path-finding
    void						Query	(CAI_NaviRayQuery &RAY)
    {
        if (root) root->RayIntersect(RAY);
		sitNodes I = dynamics.begin();
		for (;I!=dynamics.end();I++)
			(*I)->RayIntersectMover(RAY);
    }
    bool isInsideObstacle		(Fvector2& P)
    {
        if (root && root->isInside(P)) return true;;
		sitNodes I = dynamics.begin();
		for (;I!=dynamics.end();I++)
			if ((*I)->isInside(P)) return true;
        return false;
    }

	// Units collision / interaction
	IC void						unitAttach(CAI_NaviNode* unit)
	{	dynamics.insert	(unit);	}
	IC void						unitDetach(CAI_NaviNode* unit)
	{	dynamics.erase	(unit);	}
	IC float					unitPlacementHeuristic(Fvector& P, float R)
	{
		Fvector local;
		local.transform_tiny(P,W2L);
		if (local.x-R>dimX || local.y-R>dimY) return flt_max;
		else return local.z;
	}

	// Constructor / destructor
	CAI_NaviPlane				();
	~CAI_NaviPlane				();
};

#endif // !defined(AFX_AI_NAVIPLANE_H__71584EC5_DC4C_4033_A517_DDBC3472DD28__INCLUDED_)
