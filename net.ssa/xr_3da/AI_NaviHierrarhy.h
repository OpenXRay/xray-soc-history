#ifndef _AI_NaviNode_
#define _AI_NaviNode_
#pragma once

#include "AI_NaviObstacle.h"

class ENGINE_API CAI_NaviNode {
private:
	typedef vector<CAI_NaviNode*>	vecNodes;
	typedef vecNodes::iterator		itNodes;
private:
    Fvector2				C;
    float					R;
    CAI_NaviObstacle*       O;
    vecNodes				N;
public:
	bool					bDirty;
    bool					bConnected;

    CAI_NaviNode()			{ R=flt_min; O=0; bConnected=false; }

    IC float		Radius() const	{ return R; }
    IC Fvector2&	Center() const	{ return C; }

    IC void MakeLeaf(CAI_NaviObstacle* _O)
    {
        O = _O;
        N.clear();
        CalcBounds();
    }
    IC void GetGeometry  (vec2DG &geom)
    {
        if (O) O->GetGeometry(geom);
        for (itNodes it=N.begin(); it!=N.end(); it++)
            (*it)->GetGeometry(geom);
    }
    void CalcBounds   () {
        vec2DG			geom;
        GetGeometry     (geom);

        Fvector2 min,max;
        min.set(flt_max,flt_max);
        max.set(flt_min,flt_min);
        for (it2DG it=geom.begin(); it!=geom.end(); it++)
        {
            min.min(*it);
            max.max(*it);
        }
        C.add(min,max);
        C.mul(0.5f);
        R = flt_min;
        for (it=geom.begin(); it!=geom.end(); it++)
        {
            float d = C.dist(*it);
            if (d>R) R=d;
        }
    }
    IC bool TryExpand   (CAI_NaviNode* node, float rLimit)
    {
        N.push_back(node);
        CalcBounds();
        if (R>rLimit) {
            N.pop_back();
            CalcBounds();
            return false;
        }
        node->bConnected = true;
        return true;
    }
    IC void Register    (CAI_NaviNode* node)
    {
        N.push_back(node);
    }

    IC bool isLeaf()        { return O!=0; };
    IC int  chieldCount()   { return int(N.size()); };
    
    IC void RayIntersect(CAI_NaviRayQuery& RQ)
    {
        if (fabsf(RQ.classify(C))>(R+RQ.radius))  return;
        if (RQ.classify_LE(C) > R )               return;
        if (RQ.classify_LS(C) < -R)               return;

        if (O) {
            O->CircleIntersect(RQ);
        } else {
            for (itNodes it=N.begin(); it!=N.end(); it++)
                (*it)->RayIntersect(RQ);
        }
    }
	IC void	RayIntersectMover(CAI_NaviRayQuery& RQ)
	{
        if (fabsf(RQ.classify(C))>(R+RQ.radius))  return;
        if (RQ.classify_LE(C) > R )               return;
        if (RQ.classify_LS(C) < -R)               return;

		VERIFY(O);
		if (bDirty)	{
			O->GenerateCircle(C,R); 
			bDirty = false;
		}
		O->CircleIntersect(RQ);
	}
    IC bool isInside(Fvector2& P)
    {
        if (P.dist(C)>R) return false;

        for (itNodes it=N.begin(); it!=N.end(); it++)
            if ((*it)->isInside(P)) return true;

        return false;
    }

};

#endif