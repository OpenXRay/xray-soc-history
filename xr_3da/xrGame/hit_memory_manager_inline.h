////////////////////////////////////////////////////////////////////////////
//	Module 		: hit_memory_manager_inline.h
//	Created 	: 25.12.2003
//  Modified 	: 25.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Hit memory manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CHitMemoryManager::CHitMemoryManager						(CCustomMonster *object)
{
	VERIFY					(object);
	m_object				= object;
}

IC	const CHitMemoryManager::HITS &CHitMemoryManager::objects	() const
{
	VERIFY				(m_hits);
	return				(*m_hits);
}

IC	const CHitMemoryManager::CHitObject *CHitMemoryManager::hit	() const
{
	return				(m_selected_hit);
}

IC	void CHitMemoryManager::set_squad_objects					(xr_vector<CHitObject> *squad_objects)
{
	m_hits				= squad_objects;
}
