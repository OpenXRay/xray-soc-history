////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_biting_fsm.cpp
//	Created 	: 26.05.2003
//  Modified 	: 26.05.2003
//	Author		: Serge Zhem
//	Description : FSM for biting class
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_biting.h"

void CAI_Biting::Think()
{
	if (!g_Alive()) return;


	m_dwLastUpdateTime		= m_current_update;
	m_current_update		= Level().timeServer();

	vfUpdateParameters		();

	// pre-update path parameters
	enable_movement(true);
	CLevelLocationSelector::set_evaluator(0);

	// fix off-line displacement
	if ((flagsEnemy & FLAG_ENEMY_GO_OFFLINE) == FLAG_ENEMY_GO_OFFLINE) {
		CurrentState->Reset();
		SetState(stateRest);
	}

	StateSelector			();
	CurrentState->Execute	(m_current_update);

	// update path
	CDetailPathManager::set_path_type(eDetailPathTypeSmooth);
	update_path				();

	MotionMan.ProcessAction();

	if (IsMovingOnPath()) {
		// Get current linear and angular velocities
		u32 velocity_index = CDetailPathManager::path()[curr_travel_point_index()].velocity;
		xr_map<u32,STravelParams>::const_iterator it = m_movement_params.find(velocity_index);
		R_ASSERT(it != m_movement_params.end());

		// now it contains velocities
		m_fCurSpeed						= (*it).second.linear_velocity;
		m_body.speed					= 2*(*it).second.angular_velocity;
		m_fCurSpeed						= 0;	
	} else m_fCurSpeed						= 0;	


#pragma todo("Dima to Jim : This method will be automatically removed after 22.12.2003 00:00")
	set_desirable_speed		(m_fCurSpeed);
	
	// process sound
	ControlSound(m_current_update);
	
	m_head = m_body;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAI_Biting state-specific functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////

void CAI_Biting::SetState(IState *pS, bool bSkipInertiaCheck)
{
	if (CurrentState != pS) {
		// �������� �������
		if (!bSkipInertiaCheck)
			if (CurrentState->IsInertia()) {
				if (CurrentState->GetPriority() >= pS->GetPriority()) return;
			}

			CurrentState->Done();
			CurrentState->Reset();
			CurrentState = pS;
			CurrentState->Activate();
	}
}

