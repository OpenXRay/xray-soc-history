// AI_Mover.h: interface for the CAI_Mover class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI_MOVER_H__65694ACD_0191_41DD_9E8B_57BD361AB98E__INCLUDED_)
#define AFX_AI_MOVER_H__65694ACD_0191_41DD_9E8B_57BD361AB98E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ENGINE_API CAI_Mover  
{
public:
	bool						MoveUnit(CAI_NaviNode* unit, Fvector& P)
	{
		Fvector pos;
		pos.transform_tiny(P,W2L);
		if (((pos.x-unit->R) > dimX) || (((pos.y-unit->R) > dimY))
			return false;

		unit->bDirty = true;
		unit->C.set(pos.x,pos.y);
	}

	CAI_Mover();
	virtual ~CAI_Mover();

};

#endif // !defined(AFX_AI_MOVER_H__65694ACD_0191_41DD_9E8B_57BD361AB98E__INCLUDED_)
