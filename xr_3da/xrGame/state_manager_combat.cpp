////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_combat.cpp
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager combat
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "state_manager_combat.h"
#include "state_combat_attack_weak.h"

CStateManagerCombat::CStateManagerCombat	()
{
	Init					();
}

CStateManagerCombat::~CStateManagerCombat	()
{
}

void CStateManagerCombat::Init			()
{
}

void CStateManagerCombat::Load			(LPCSTR section)
{
	inherited::Load			(section);
}

void CStateManagerCombat::reinit		(CAI_Stalker *object)
{
	inherited::reinit		(object);
	add_state				(xr_new<CStateAttackWeak>(),	eCombatStateAttackWeak,		0);
	set_current_state		(eCombatStateAttackWeak);
	set_dest_state			(eCombatStateAttackWeak);
}

void CStateManagerCombat::reload		(LPCSTR section)
{
	inherited::reload		(section);
}

void CStateManagerCombat::initialize	()
{
	inherited::initialize	();
}

void CStateManagerCombat::execute		()
{
	set_dest_state			(eCombatStateAttackWeak);
	inherited::execute		();
}

void CStateManagerCombat::finalize		()
{
	inherited::finalize		();
}
