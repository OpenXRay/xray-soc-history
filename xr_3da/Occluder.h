// Occluder.h: interface for the COccluder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OCCLUDER_H__DE651E37_23E6_4DBD_9B57_45586359DA4A__INCLUDED_)
#define AFX_OCCLUDER_H__DE651E37_23E6_4DBD_9B57_45586359DA4A__INCLUDED_
#pragma once

#include "frustum.h"

class ENGINE_API FBasicVisual;
class ENGINE_API COccluderSystem 
{
private:
	CFrustum*					baseF;
public:
	EFC_Visible	visibleVisual	(DWORD planes, FBasicVisual* pVisual);
	EFC_Visible	visibleSphere	(DWORD planes, Fvector &C, float r);
	EFC_Visible	visibleSphereNC	(Fvector &C, float r);
	BOOL		visiblePoint	(Fvector &P);
};

#endif // !defined(AFX_OCCLUDER_H__DE651E37_23E6_4DBD_9B57_45586359DA4A__INCLUDED_)
