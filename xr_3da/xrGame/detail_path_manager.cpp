////////////////////////////////////////////////////////////////////////////
//	Module 		: detailed_path_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 12.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Detail path manager
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "detail_path_manager.h"
#include "ai_space.h"

CDetailPathManager::CDetailPathManager	()
{
	Init					();
}

CDetailPathManager::~CDetailPathManager	()
{
}

void CDetailPathManager::Init			()
{
	CAI_ObjectLocation::Init();
	m_actuality				= false;
	m_failed				= false;
	m_start_position		= Fvector().set(0,0,0);
	m_dest_position			= Fvector().set(0,0,0);
	m_current_travel_point	= u32(-1);
	m_path_type				= eDetailPathTypeSmooth;
	m_path.clear			();
	m_movement_params.clear	();
	m_desirable_mask		= u32(-1);
	m_velocity_mask			= 0;
	m_try_min_time			= false;
	m_use_dest_orientation	= false;
}

bool CDetailPathManager::valid			() const
{
	bool					b = true;
	b						= b && !m_path.empty();
	b						= b && fis_zero(m_path.front().position.distance_to_xz_sqr(m_start_position));
	b						= b && fis_zero(m_path.back().position.distance_to_xz_sqr(m_dest_position));
	return					(!m_path.empty() && fis_zero(m_path.front().position.distance_to_xz_sqr(m_start_position)) && fis_zero(m_path.back().position.distance_to_xz_sqr(m_dest_position)));
}

bool CDetailPathManager::valid			(const Fvector &position) const
{
	return					(!!_valid(position));
}

const Fvector CDetailPathManager::direction()
{
	if ((m_path.size() < 2) || (m_path.size() <= m_current_travel_point + 1))
		return				(Fvector().set(0,0,1));
	
	Fvector				direction;
	direction.sub			(m_path[m_current_travel_point + 1].position, m_path[m_current_travel_point].position);

	if (direction.square_magnitude() < EPS_L)
		direction.set		(0.f,0.f,1.f);
	else
		direction.normalize	();

	return					(direction);
}

void CDetailPathManager::build_path(const xr_vector<u32> &level_path, u32 intermediate_index)
{
	if (valid(m_start_position) && valid(m_dest_position)) {
		switch (m_path_type) {
			case eDetailPathTypeSmooth : {
				build_smooth_path(level_path,intermediate_index);
				break;
			}
			case eDetailPathTypeSmoothDodge : {
				build_dodge_path(level_path,intermediate_index);
				break;
			}
			case eDetailPathTypeSmoothCriteria : {
				build_criteria_path(level_path,intermediate_index);
				break;
			}
		}
		if (valid()) {
			m_actuality				= true;
			m_current_travel_point	= 0;
		}
	}
}

bool CDetailPathManager::actual() const
{
	return					(m_actuality);
}
