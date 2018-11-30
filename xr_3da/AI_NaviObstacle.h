#ifndef _AI_NaviObstacle_
#define _AI_NaviObstacle_
#pragma once

typedef vector<Fvector2>	vec2DG;
typedef vec2DG::iterator	it2DG;

extern float g_AINavi_RayRadius;

class ENGINE_API CAI_NaviObstacle;

class ENGINE_API CAI_NaviRayQuery {
public:
	// input
	Fvector2	Start;
	Fvector2	Dest;
	Fvector2	Dir;
	
	// cache
	Fvector2	N;      // line equation
	float		c;      //
	
	float		_cE;    // end effector equation   (Dir,_cE)
	float		_cS;    // start effector equation (Dir,_cS)
	
	// output - only if intersection occurs
	float       distance;
	struct      CriticalPoint
	{
		Fvector2	V;
		int			ID;
	};
	CriticalPoint		CP[2];
	CAI_NaviObstacle*	pObstacle;
	
	IC void Init_RAY(Fvector2& S, Fvector2& D)
	{
		// General
		Start.set   (S);
		Dest.set    (D);
		Dir.sub     (D,S);
		Dir.norm    ();
		
		// Line EQ
		N.cross(Dir);
		c = -N.dot(Start);
	}
	
	IC void Init(Fvector2& S, Fvector2& D)
	{
		Init_RAY(S,D);
		
		// Create End-effector
		_cE = -Dir.dot(Dest);
		Fvector2 T;
		T.direct(Start,Dir,.1f);
		_cS = -Dir.dot(T);
		
		// Distance
		distance    = flt_max;
		pObstacle   = 0;
	}
	IC bool WasCollision()
	{
		return pObstacle!=0;
	}
	IC float classify(Fvector2& V)
	{  return N.dot(V)+c; }
	IC float classify_LE(Fvector2& V)
	{  return Dir.dot(V)+_cE; }
	IC float classify_LS(Fvector2& V)
	{  return Dir.dot(V)+_cS; }
};

class ENGINE_API CAI_NaviObstacle {
    vec2DG				vertices;
    vector<bool>        visited;
public:
	void	GenerateCircle(Fvector2& P, float r)
	{
		// we sample 5 points on it to gain minimal 'circle'
		vertices.clear();
		visited.assign(5,false);

		Fvector2 P;
		for (float a=0; a<deg2rad(360); a+=deg2rad(360.f/5.f))
		{
			P.set(cosf(a),sinf(a));
			P.mul(r);
			P.add(Pos);
			vertices.push_back(P);
		}
	}
    void GetGeometry(vec2DG &V)
    {
        V.insert(V.end(),vertices.begin(),vertices.end());
    }
	void SetGeometry(Fvector2* begin, Fvector2* end)
	{
		vertices.assign(begin,end);
		visited.assign(end-begin,false);
	}

    // corrects vertex 'end'
    void Reproject(CAI_NaviRayQuery& RQ_Base)
    {
        CAI_NaviRayQuery    RQmin,RQmax;
        it2DG				IDmin,IDmax,it;
        float				min = flt_max, max = flt_min;

        RQmin.Init_RAY (RQ_Base.Start,RQ_Base.CP[0].V);
        RQmax.Init_RAY (RQ_Base.Start,RQ_Base.CP[1].V);
        for (it=vertices.begin(); it!=vertices.end(); it++)
        {
            float d;

			d = RQmin.classify(*it);
            if (d<min)   { min=d; IDmin = it; };

            d = RQmax.classify(*it);
            if (d>max)   { max=d; IDmax = it; };
        }

        float				c_correction = g_AINavi_RayRadius*1.2f+.5f;

        RQ_Base.CP[0].V.direct(*IDmin,RQ.N,-c_correction);
        RQ_Base.CP[0].ID = IDmin-vertices.begin();

        RQ_Base.CP[1].V.direct(*IDmax,RQ.N,c_correction);
        RQ_Base.CP[1].ID = IDmax-vertices.begin();
    }
    IC bool vis_query(int id) { return visited[id]; }
    IC void vis_set  (int id) { visited[id]=true;   }
    IC void vis_clear(int id) { visited[id]=false;  }
    
    bool CircleIntersect(CAI_NaviRayQuery& RQ)
    {
        float min = flt_max;  Fvector2*   Vmin=0;
        float max = flt_min;  Fvector2*   Vmax=0;

        bool   was_intersection = false;
        for (it2DG it=vertices.begin(); it!=vertices.end(); it++)
        {
            Fvector2 &Q = *it;
            if (RQ.classify_LE(Q)<0 && RQ.classify_LS(Q)>0) was_intersection = true; else continue;
            float cls = RQ.classify(Q);
            if (cls<min) { min=cls; Vmin=it; }
            if (cls>max) { max=cls; Vmax=it; }
        }

        // bounds defined
        if (was_intersection) {
            if (min>=g_AINavi_RayRadius || max<=-g_AINavi_RayRadius) return false;
        } else return false;

        // calc distance for sorting out obstacles
        float DC = RQ.Start.dist(*Vmin);
        if (DC<RQ.distance) {
            float c_correction = g_AINavi_RayRadius*1.1;

            RQ.pObstacle = this;

            RQ.distance  = DC;

            RQ.CP[0].V.direct(*Vmin,RQ.N,-c_correction);

            RQ.CP[1].V.direct(*Vmax,RQ.N, c_correction);
        }

        return true;        
    }
};

#endif
