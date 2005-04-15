////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_sound_data_visitor.cpp
//	Created 	: 02.02.2005
//  Modified 	: 02.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker sound data visitor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stalker_sound_data_visitor.h"
#include "ai/stalker/ai_stalker.h"
#include "stalker_sound_data.h"
#include "agent_manager.h"
#include "agent_member_manager.h"
#include "memory_manager.h"
#include "hit_memory_manager.h"
#include "enemy_manager.h"

CStalkerSoundDataVisitor::~CStalkerSoundDataVisitor	()
{
}

void CStalkerSoundDataVisitor::visit				(CStalkerSoundData *data)
{
	if (object().is_relation_enemy(&data->object()))
		return;

	if (!data->object().memory().enemy().selected() || data->object().memory().enemy().selected()->getDestroy())
		return;

	if (!object().is_relation_enemy(data->object().memory().enemy().selected()))
		return;

	if (!data->object().g_Alive())
		return;

	if (!object().g_Alive())
		return;

//	Msg			("%s : Adding fiction hit by sound info from stalker %s",*object().cName(),*data->object().cName());

	const MemorySpace::CHitObject	*m = data->object().memory().hit().hit(data->object().memory().enemy().selected());
	
	if (!m)
		return;

	object().memory().hit().add		(*m);
	
//	object().agent_manager().member().register_in_combat(m_object);
}
