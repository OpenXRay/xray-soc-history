#include "stdafx.h"
#include "custommonster.h"

namespace IQ 
{

#define NS_WALK		20.0
#define NS_RUN		40.0
	
#define TM_StayAnimation	0
#define TM_TargetLost		1
	
	// Purpose: re-select target
	void OnInitialize	(IQ_Brain* brain)
	{
		//	msg("sizeof(Brain)=%d", sizeof(IQ_Brain));
		brain->EyeMode			= EYE_LookAround;
		brain->EyeHeight		= 1;
		brain->EyeFOV			= 80;
		brain->EyeRange			= 100;
		brain->EyeLookSpeed		= 4;
		iqVisible_set_clsid		(&(brain->Visible[TCLASS_Primary]),"O_ACTOR");
		brain->Target			= NULL;
		brain->NaviMode			= NAVI_Stay;
		brain->NaviPrediction		= 0.1;
		brain->NaviSpeed		= NS_RUN;
		brain->AuralPerception		= 0.5;
	};
	
	void timersCancel(IQ_Brain* brain)
	{
		brain->Timers[TM_StayAnimation].cProcedure	= 0;
		brain->Timers[TM_TargetLost].cProcedure		= 0;
	}
	
	// Purpose: re-select target
	// Note:	'Target' may be automatically cleared if it becomes invisible
	//			TargetPoint specifies last 'seen' position
	void naviPrimary(IQ_Brain* brain)
	{
		//	msg("naviPrimary");
		brain->Target	= brain->Visible[TCLASS_Primary].nearestReachable;
		brain->NaviMode	= NAVI_TargetObject;
		brain->EyeMode	= EYE_LookTarget;
		brain->NaviSpeed= NS_RUN;
		timersCancel(brain);
	}
	void naviOptional(IQ_Brain* brain)
	{
		//	msg("naviOptional");
		brain->Target	= brain->Visible[TCLASS_Optional].nearestReachable;
		brain->NaviMode	= NAVI_TargetObject;
		brain->EyeMode	= EYE_LookAround;
		brain->NaviSpeed= NS_WALK;
		timersCancel(brain);
	}
	void naviStay	(IQ_Brain* brain)
	{
		//	msg("naviStay");
		brain->Target	= NULL;
		brain->NaviMode	= NAVI_Stay;
		brain->EyeMode	= EYE_LookAround;
		brain->NaviSpeed= 0;
		timersCancel(brain);
	}
	
	void OnVisGain		(IQ_Brain* brain, int group)
	{
		//	msg("OnVisGain");
		if (brain->Target) {
			switch (group) {
			case TCLASS_Primary:		// enemy
				// re-select target
				naviPrimary(brain);
				break;
			case TCLASS_Secondary:		// dangerous thing
				// do nothing
				break;
			case TCLASS_Optional:		// item
				// re-select target
				if (brain->TargetGroup != TCLASS_Primary) {
					// we was moving to item or something else
					// reselect target if better exists
					naviOptional(brain);
				}
				break;
			}
		} else {
			// no target at all select anything
			switch (group) {
			case TCLASS_Primary:		// enemy
				// run to it
				naviPrimary(brain);
				break;
			case TCLASS_Secondary:		// dangerous thing
				// do nothing
				break;
			case TCLASS_Optional:		// item
				// select target
				if (brain->NaviMode!=NAVI_TargetPoint) 
					naviOptional(brain);
				break;
			}
		}
	}
	
	// Target lost
	void OnVisTargetLost(IQ_Brain* brain)
	{
		//	msg("OnVisTargetLost");
		if (brain->Visible[TCLASS_Primary].nearestReachable)
		{
			naviPrimary(brain);
			return;
		}
		
		// Move to last seen position
		brain->NaviMode	= NAVI_TargetPoint;
		brain->EyeMode	= EYE_LookAround;
		brain->NaviSpeed= NS_RUN;
	}
	
	void tm_TargetLost(IQ_Brain* brain)
	{
		//	msg("tm_TargetLost");
		// Cancel all timers
		timersCancel(brain);
		
		// Make decision what to do
		if (brain->Visible[TCLASS_Primary].nearestReachable)
		{
			naviPrimary(brain);
		} else {
			if (brain->Visible[TCLASS_Optional].nearestReachable)
			{
				naviOptional(brain);
			} else {
				// I don't know what to do
				// naviStay	(brain);
				//			msg("* I don't know what to do :(");
			}
		}
	}
	
	void OnTargetReached(IQ_Brain* brain)
	{
		//	msg("OnTargetReached");
		// What in hell we was doing?
		if (brain->NaviMode==NAVI_TargetPoint)
		{
			// For some reason we was moving to specified point
			//	1. Pre-defined actions/scenery
			//	2. We lost our target and was going to last 'seen' point
			
			//  Stay here for some time - than make decision
			naviStay(brain);
//			iqTimer_init_rand(&(brain->Timers[TM_TargetLost]),"tm_TargetLost",1,3);
		} else {
			// Kill him!
			//msg("* I don't know what to do :(");
		}
	}
	
	// Purpose: re-select target / turn / move to destination
	void OnHear			(IQ_Brain* brain, Vector* where)
	{
		//msg("OnHear");
		if (brain->Target) return;
		
		brain->TargetPoint	= *where;
		brain->NaviMode		= NAVI_TargetPoint;
		brain->EyeMode		= EYE_LookAround;
		brain->NaviSpeed	= NS_WALK;
		timersCancel		(brain);
	};
};
