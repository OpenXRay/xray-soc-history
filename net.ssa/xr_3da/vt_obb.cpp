#include "stdafx.h"

//--------------------------------------------------------------------------
// CHECKS IF AN EDGE AB INTERSECTS THE VIEW-FRUSTUM (JUST A SET OF 4 PLANES)
//--------------------------------------------------------------------------
int CFrustumClipper::frustumEdgeIsect(Fvector& Start, Fvector& End)
{
	Fvector Dir;
	Dir.sub(End,Start);				// CALC DIRECTION VECTOR OF EDGE
	float InT=-99999, OutT=99999;   // INIT INTERVAL T-VAL ENDPTS TO -/+ INFINITY
	float NdotDir, NdotStart;       // STORAGE FOR REPEATED CALCS NEEDED FOR NewT CALC
	float NewT;

	for (int i=0; i<4; i++)         // CHECK INTERSECTION AGAINST EACH VF PLANE
	{
		NdotDir		= Planes[i].n.dotproduct(Dir);
		NdotStart	= Planes[i].n.dotproduct(Start);
		if (fabsf(NdotDir) < EPS)   // CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
		{
			if (NdotStart > Planes[i].d) return(0); // IF STARTS "OUTSIDE", NO INTERSECTION
		}
		else
		{
			NewT = (Planes[i].d - NdotStart) / NdotDir;      // FIND HIT "TIME" (DISTANCE)
			if (NdotDir < 0) { if (NewT > InT) InT=NewT; }   // IF "FRONTFACING", MUST BE NEW IN "TIME"
			else { if (NewT < OutT) OutT=NewT; } // IF "BACKFACING", MUST BE NEW OUT "TIME"
		}
		if (InT > OutT) return(0);   // CHECK FOR EARLY EXITS (INTERSECTION INTERVAL "OUTSIDE")
	}

	// IF AT LEAST ONE THE Tvals ARE IN THE INTERVAL [0,1] WE HAVE INTERSECTION
	if (InT>=0 && InT<=1) return(1);
	else if (OutT>=0 && OutT<=1) return(1);
	else return(0);
}

//---------------------------------------------------------------------------
// Oriented Bounding Box / Plane overlap test (returns type of overlap)
//  Chooses a corresponding vert (in obbV) of the OBB in the same octant as the
//  given plane's normal. Requires plane, obb vertices, obb axes.
//  Calcs # of the octant: 0=RTN,1=LTN,2=LBN,3=RBN,4=RTF,5=LTF,6=LBF,7=RBF
//  also calcs the opposite corner (Neg of Normal) index.
//---------------------------------------------------------------------------
int CFrustumClipper::obbOverlapPlane(Fplane &P, Fvector *obbV, Fvector &X, Fvector &Y, Fvector &Z)
{
	// CALC INDICES (iPos,iNeg) FOR EXTREME PTS ALONG NORMAL AXIS (iPos in dir of norm, etc.)

	// XFORM PLANE NORM TO BOX COORD SYS
	Fvector N;
	N.x = P.n.dotproduct(X);
	N.y = P.n.dotproduct(Y);
	N.z = P.n.dotproduct(Z);

	int iPos, iNeg;
	if(N.x>0) {
		if(N.y>0) {
			if(N.z>0) { iPos=0; iNeg=6; } else { iPos=4; iNeg=2; }
		} else {
			if(N.z>0) { iPos=3; iNeg=5; } else { iPos=7; iNeg=1; }
		}
	} else {
		if(N.y>0) {
			if(N.z>0) { iPos=1; iNeg=7; } else { iPos=5; iNeg=3; }
		} else {
			if(N.z>0) { iPos=2; iNeg=4; } else { iPos=6; iNeg=0; }
		}
	}

	// CHECK DISTANCE TO PLANE FROM EXTREMAL POINTS TO DETERMINE OVERLAP
	if (P.classify(obbV[iNeg]) > 0) return(fcvNone);
	else
	{
		if (P.classify(obbV[iPos]) <= 0) return(fcvFully);
		else return(fcvPartial);
	}
}

//--------------------------------------------------------------------------
// Ray-OBB intersection test. returns In-Out HitTimes
// Requires ray start and direction, obb vertices and axes
//--------------------------------------------------------------------------
int CFrustumClipper::obbRayIsect(Fvector &Start, Fvector &Dir,
                Fvector* obbV, Fvector &X, Fvector &Y, Fvector &Z,
                float &InT, float &OutT)
{
	InT=-99999, OutT=99999;    // INIT INTERVAL T-VAL ENDPTS TO -/+ "INFINITY"
	float NewInT, NewOutT;     // STORAGE FOR NEW T VALUES
	float MinD, MaxD;          // SLAB PLANE D VALS (DIST ALONG NORMAL TO PLANE)
	float NdotDir, NdotStart;  // STORAGE FOR REPEATED CALCS NEEDED FOR NewT CALC

	// X-SLAB (PARALLEL PLANES PERPENDICULAR TO X-AXIS) INTERSECTION (Xaxis is Normal)
	NdotDir	= X.dotproduct(Dir);	// CALC DOT PROD OF PLANE NORMAL AND RAY DIR
	NdotStart = X.dotproduct(Start);	// CALC DOT PROD OF PLANE NORMAL AND RAY START PT
	MinD = X.dotproduct(obbV[6]);		// CALC D-VAL FOR FIRST PLANE OF SLAB (use LBF)
	MaxD = X.dotproduct(obbV[0]);		// CALC D-VAL FOR SECOND PLANE OF SLAB (use RTN)
	if (fabsf(NdotDir) < EPS)			// CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
	{
		if (MinD < MaxD) { if ((NdotStart < MinD) || (NdotStart > MaxD)) return(0); }
		else { if ((NdotStart < MaxD) || (NdotStart > MinD)) return(0); }
	}
	else
	{
		NewInT =	(MinD - NdotStart) / NdotDir;
		NewOutT =	(MaxD - NdotStart) / NdotDir;

		if (NewOutT > NewInT)
		{
			if (NewInT>InT) InT=NewInT;
			if (NewOutT<OutT) OutT=NewOutT;
		}
		else
		{
			if (NewOutT > InT) InT=NewOutT;
			if (NewInT < OutT) OutT=NewInT;
		}
		if (InT > OutT) return(0);
	}

	// Y-SLAB (PARALLEL PLANES PERPENDICULAR TO Y-AXIS) INTERSECTION (Yaxis is Normal)
	NdotDir = Y.dotproduct(Dir);     // CALC DOT PROD OF PLANE NORMAL AND RAY DIR
	NdotStart = Y.dotproduct(Start); // CALC DOT PROD OF PLANE NORMAL AND RAY START PT
	MinD = Y.dotproduct(obbV[6]);    // CALC D-VAL FOR FIRST PLANE OF SLAB (use LBF)
	MaxD = Y.dotproduct(obbV[0]);    // CALC D-VAL FOR SECOND PLANE OF SLAB (use RTN)
	if (fabsf(NdotDir) < EPS)        // CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
	{
		if (MinD < MaxD)
		{
			if ((NdotStart < MinD) || (NdotStart > MaxD)) return(0);
		}
		else
		{
			if ((NdotStart < MaxD) || (NdotStart > MinD)) return(0);
		}
	}
	else
	{
		NewInT  = (MinD - NdotStart) / NdotDir;
		NewOutT = (MaxD - NdotStart) / NdotDir;
		if (NewOutT > NewInT)
		{
			if (NewInT>InT) InT=NewInT;
			if (NewOutT<OutT) OutT=NewOutT;
		}
		else
		{
			if (NewOutT > InT) InT=NewOutT;
			if (NewInT < OutT) OutT=NewInT;
		}
		if (InT > OutT) return(0);
	}

	// Z-SLAB (PARALLEL PLANES PERPENDICULAR TO Z-AXIS) INTERSECTION (Zaxis is Normal)
	NdotDir   = Z.dotproduct(Dir);    // CALC DOT PROD OF PLANE NORMAL AND RAY DIR
	NdotStart = Z.dotproduct(Start);  // CALC DOT PROD OF PLANE NORMAL AND RAY START PT
	MinD = Z.dotproduct(obbV[6]);     // CALC D-VAL FOR FIRST PLANE OF SLAB (use LBF)
	MaxD = Z.dotproduct(obbV[0]);     // CALC D-VAL FOR SECOND PLANE OF SLAB (use RTN)
	if (fabsf(NdotDir) < EPS)			// CHECK IF RAY IS PARALLEL TO THE SLAB PLANES
	{
		if (MinD < MaxD) { if ((NdotStart < MinD) || (NdotStart > MaxD)) return(0); }
		else { if ((NdotStart < MaxD) || (NdotStart > MinD)) return(0); }
	}
	else
	{
		NewInT = (MinD - NdotStart) / NdotDir;
		NewOutT = (MaxD - NdotStart) / NdotDir;
		if (NewOutT > NewInT) { if (NewInT>InT) InT=NewInT;  if (NewOutT<OutT) OutT=NewOutT; }
		else { if (NewOutT > InT) InT=NewOutT;  if (NewInT < OutT) OutT=NewInT; }
		if (InT > OutT) return(0);
	}

	// CHECK IF INTERSECTIONS ARE "AT OR BEYOND" THE START OF THE RAY
	if (InT>=0 || OutT>=0) return(1); else return(0);
}


//--------------------------------------------------------------------------
// View-frustum/OBB overlap test: given the OBB verts and axes.
// returns the type of overlap determined (complete in, partial, complete out)
//--------------------------------------------------------------------------
int CFrustumClipper::obbOverlapVF(Fvector* V, Fvector &X, Fvector &Y, Fvector &Z)
{
  // GO FOR TRIVIAL REJECTION OR ACCEPTANCE USING "FASTER OVERLAP TEST"
  int CompletelyIn=1;    // ASSUME COMPLETELY IN UNTIL ONE COUNTEREXAMPLE
  int R;                 // TEST RETURN VALUE
  R=obbOverlapPlane(Planes[0],V,X,Y,Z);
  if(R==fcvNone) return(fcvNone); else if(R==fcvPartial) CompletelyIn=0;
  R=obbOverlapPlane(Planes[1],V,X,Y,Z);
  if(R==fcvNone) return(fcvNone); else if(R==fcvPartial) CompletelyIn=0;
  R=obbOverlapPlane(Planes[2],V,X,Y,Z);
  if(R==fcvNone) return(fcvNone); else if(R==fcvPartial) CompletelyIn=0;
  R=obbOverlapPlane(Planes[3],V,X,Y,Z);
  if(R==fcvNone) return(fcvNone); else if(R==fcvPartial) CompletelyIn=0;

  if (CompletelyIn) return(fcvFully);  // CHECK IF STILL COMPLETELY "IN"

  // TEST FOR VIEW-FRUSTUM EDGE PROTRUSION THROUGH AABB FACE (PROJECTORS ONLY)
  float InT, OutT;
  if (obbRayIsect(COP,ProjDirs[0],V,X,Y,Z,InT,OutT)) return(fcvPartial);
  if (obbRayIsect(COP,ProjDirs[1],V,X,Y,Z,InT,OutT)) return(fcvPartial);
  if (obbRayIsect(COP,ProjDirs[2],V,X,Y,Z,InT,OutT)) return(fcvPartial);
  if (obbRayIsect(COP,ProjDirs[3],V,X,Y,Z,InT,OutT)) return(fcvPartial);

  // TEST FOR PROTRUSION OF AABB EDGE THROUGH VIEW-FRUSTUM FACE
  if (frustumEdgeIsect(V[0],V[4])) return(fcvPartial);
  if (frustumEdgeIsect(V[1],V[5])) return(fcvPartial);
  if (frustumEdgeIsect(V[2],V[6])) return(fcvPartial);
  if (frustumEdgeIsect(V[3],V[7])) return(fcvPartial);
  if (frustumEdgeIsect(V[0],V[3])) return(fcvPartial);
  if (frustumEdgeIsect(V[3],V[2])) return(fcvPartial);
  if (frustumEdgeIsect(V[2],V[1])) return(fcvPartial);
  if (frustumEdgeIsect(V[1],V[0])) return(fcvPartial);
  if (frustumEdgeIsect(V[4],V[7])) return(fcvPartial);
  if (frustumEdgeIsect(V[7],V[6])) return(fcvPartial);
  if (frustumEdgeIsect(V[6],V[5])) return(fcvPartial);
  if (frustumEdgeIsect(V[5],V[4])) return(fcvPartial);

  // VF MUST BE COMPLETELY ENCLOSED SINCE PT IS NOT "OUT "OF ANY AABB PLANE.
  return(fcvNone);
};


void M(Fvector &D, Fmatrix &X, Fmatrix &Y, Fmatrix &Z)
{
	X.mul(Y,Z);
	X.transform_tiny(D);
}
