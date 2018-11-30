#include "stdafx.h"

EFC_Visible CFrustumClipper::visibleQuad(Fvector &p1, Fvector &p2, Fvector &p3, Fvector &p4)
{
	float c1,c2,c3,c4;

	// left plane
	c1 = Planes[fcpLeft].classify(p1);
	c2 = Planes[fcpLeft].classify(p2);
	c3 = Planes[fcpLeft].classify(p3);
	c4 = Planes[fcpLeft].classify(p4);
	if ((c1>0) && (c2>0) && (c3>0) && (c4>0)) return fcvNone;
	// right plane
	c1 = Planes[fcpRight].classify(p1);
	c2 = Planes[fcpRight].classify(p2);
	c3 = Planes[fcpRight].classify(p3);
	c4 = Planes[fcpRight].classify(p4);
	if ((c1>0) && (c2>0) && (c3>0) && (c4>0)) return fcvNone;
	// bottom plane
	c1 = Planes[fcpBottom].classify(p1);
	c2 = Planes[fcpBottom].classify(p2);
	c3 = Planes[fcpBottom].classify(p3);
	c4 = Planes[fcpBottom].classify(p4);
	if ((c1>0) && (c2>0) && (c3>0) && (c4>0)) return fcvNone;
	// top plane
	c1 = Planes[fcpTop].classify(p1);
	c2 = Planes[fcpTop].classify(p2);
	c3 = Planes[fcpTop].classify(p3);
	c4 = Planes[fcpTop].classify(p4);
	if ((c1>0) && (c2>0) && (c3>0) && (c4>0)) return fcvNone;

	return fcvFully;
}
