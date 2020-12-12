#ifndef _VIS_TEST_
#define _VIS_TEST_
#pragma once

#include "device.h"

// planes are directed outside
class ENGINE_API CFrustumClipper {
	Fplane			Planes	[fcp_last];	// planes of the frustum
	Fvector			ProjDirs[fcp_last]; // directions of projectors along corners of frustum
	Fvector			COP;				// center-of-projection for the frustum (world coords)
	float			wR,wL,wT,wB;		// viewplane windows extents on z=1 camera coord plane
private:
	int				frustumEdgeIsect(Fvector& Start, Fvector& End);
	int				obbOverlapPlane	(Fplane &P, Fvector *obbV, Fvector &X, Fvector &Y, Fvector &Z);
	int				obbRayIsect		(Fvector &Start, Fvector &Dir,
									Fvector* obbV, Fvector &X, Fvector &Y, Fvector &Z,
									float &InT, float &OutT);
	int				obbOverlapVF	(Fvector* V, Fvector &X, Fvector &Y, Fvector &Z);
public:
	float			fFarPlane;

	void			Projection		(float FOV, float N, float F);

	void			BuildFrustum	(Fvector &P, Fvector &D, Fvector &N);
	void			DrawFrustum		();

	// Sphere visibility
	IC EFC_Visible	visibleSphere	(Fvector &c, float r)
	{
		float		cls1,cls2,cls3,cls4,cls5;
		// left
		cls1 = Planes[fcpLeft].classify(c);
		if (cls1>r) return fcvNone;
		
		// right
		cls2 = Planes[fcpRight].classify(c);
		if (cls2>r) return fcvNone;
		
		// bottom
		cls3 = Planes[fcpBottom].classify(c);
		if (cls3>r) return fcvNone;
		
		// top
		cls4 = Planes[fcpTop].classify(c);
		if (cls4>r) return fcvNone;
		
		// far
		cls5 = Planes[fcpFar].classify(c);
		if (cls5>r) return fcvNone;
		
		// now we have detected that sphere is visible
		// test for 'partial' visibility...
		if (fabsf(cls1)<r || fabsf(cls2)<r || fabsf(cls3)<r || fabsf(cls4)<r || fabsf(cls5)<r)
			return fcvPartial;
		else
			return fcvFully;
	}

	EFC_Visible		visibleQuad		(Fvector &p1, Fvector &p2, Fvector &p3, Fvector &p4);

	// Oriented bounding box visibility
	EFC_Visible		visibleOBB		();

	// Axis-aligned bounding box visibility
	EFC_Visible		visibleAABB		();
};


#endif
