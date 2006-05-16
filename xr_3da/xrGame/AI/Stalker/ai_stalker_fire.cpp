////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker_fire.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : Fire and enemy parameters for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "ai_stalker_impl.h"
#include "../../script_entity_action.h"
#include "../../inventory.h"
#include "../../ef_storage.h"
#include "../../stalker_decision_space.h"
#include "../../script_game_object.h"
#include "../../customzone.h"
#include "../../../skeletonanimated.h"
#include "../../agent_manager.h"
#include "../../stalker_animation_manager.h"
#include "../../stalker_planner.h"
#include "../../ef_pattern.h"
#include "../../memory_manager.h"
#include "../../hit_memory_manager.h"
#include "../../enemy_manager.h"
#include "../../item_manager.h"
#include "../../stalker_movement_manager.h"
#include "../../entitycondition.h"
#include "../../sound_player.h"
#include "../../cover_point.h"
#include "../../agent_member_manager.h"
#include "../../agent_location_manager.h"
#include "../../danger_cover_location.h"
#include "../../object_handler_planner.h"
#include "../../object_handler_space.h"
#include "../../visual_memory_manager.h"
#include "../../weapon.h"
#include "ai_stalker_space.h"
#include "../../effectorshot.h"
#include "../../BoneProtections.h"
#include "../../RadioactiveZone.h"

using namespace StalkerSpace;

const float DANGER_DISTANCE				= 3.f;
const u32	DANGER_INTERVAL				= 120000;

const float PRECISE_DISTANCE			= 2.5f;
const float FLOOR_DISTANCE				= 2.f;
const float NEAR_DISTANCE				= 2.5f;
const u32	FIRE_MAKE_SENSE_INTERVAL	= 20000;

float CAI_Stalker::GetWeaponAccuracy	() const
{
	float				base = PI/180.f;
	
	//������� ����� �� ��������
	base				*= m_fRankDisperison;

	if (!movement().path_completed()) {
		if (movement().movement_type() == eMovementTypeWalk)
			if (movement().body_state() == eBodyStateStand)
				return		(base*m_disp_walk_stand);
			else
				return		(base*m_disp_walk_crouch);
		else
			if (movement().movement_type() == eMovementTypeRun)
				if (movement().body_state() == eBodyStateStand)
					return	(base*m_disp_run_stand);
				else
					return	(base*m_disp_run_crouch);
	}
	
	if (movement().body_state() == eBodyStateStand)
		if (zoom_state())
			return			(base*m_disp_stand_stand);
		else
			return			(base*m_disp_stand_stand_zoom);
	else
		if (zoom_state())
			return			(base*m_disp_stand_crouch);
		else
			return			(base*m_disp_stand_crouch_zoom);
}

void CAI_Stalker::g_fireParams(const CHudItem* pHudItem, Fvector& P, Fvector& D)
{
	VERIFY				(inventory().ActiveItem());
//	if (!inventory().ActiveItem()) {
//		P				= Position();
//		D				= Fvector().set(0.f,0.f,1.f);
//		return;
//	}

	CWeapon				*weapon = smart_cast<CWeapon*>(inventory().ActiveItem());
	if (!weapon) {
		P				= eye_matrix.c;
		D				= eye_matrix.k;
		if (weapon_shot_effector().IsActive())
			D			= weapon_shot_effector_direction(D);
		return;
	}

	if (!g_Alive()) {
		P				= weapon->get_LastFP();
		D				= weapon->get_LastFD();
		return;
	}

	switch (movement().body_state()) {
		case eBodyStateStand : {
			if (movement().movement_type() == eMovementTypeStand) {
				P		= eye_matrix.c;
				D		= eye_matrix.k;
				if (weapon_shot_effector().IsActive())
					D	= weapon_shot_effector_direction(D);
			}
			else {
				D.setHP	(-movement().m_head.current.yaw,-movement().m_head.current.pitch);
				if (weapon_shot_effector().IsActive())
					D			= weapon_shot_effector_direction(D);
				Center	(P);
				P.mad	(D,.5f);
				P.y		+= .50f;
//				P		= weapon->get_LastFP();
//				D		= weapon->get_LastFD();
			}
			return;
		}
		case eBodyStateCrouch : {
			P			= eye_matrix.c;
			D			= eye_matrix.k;
			if (weapon_shot_effector().IsActive())
				D		= weapon_shot_effector_direction(D);
			return;
		}
		default			: NODEFAULT;
	}

#ifdef DEBUG
	P					= weapon->get_LastFP();
	D					= weapon->get_LastFD();
#endif
}

void CAI_Stalker::g_WeaponBones	(int &L, int &R1, int &R2)
{
	int				r_hand, r_finger2, l_finger1;
	CObjectHandler::weapon_bones(r_hand, r_finger2, l_finger1);
	R1				= r_hand;
	R2				= r_finger2;
	if	(
			(GetCurrentAction() && !GetCurrentAction()->m_tAnimationAction.m_bHandUsage) ||
			(!animation().script_animations().empty() && animation().script_animations().front().hand_usage())
		)
	{
			L		= R2;
	}
	else {
		L			= l_finger1;
	}
}

void			CAI_Stalker::Hit					(SHit* pHDS)
{
//	pHDS->power *= .1f;
	//��� ����� �������� � ����������� �� ����� (������� �������� ������ ����, ��� ��������)
	SHit HDS = *pHDS;
	HDS.power *= m_fRankImmunity;
	if(m_boneHitProtection && HDS.hit_type == ALife::eHitTypeFireWound){
		float BoneArmour = m_boneHitProtection->getBoneArmour(HDS.bone());	
		float NewHitPower = HDS.damage() - BoneArmour;
		if (NewHitPower < HDS.power*m_boneHitProtection->m_fHitFrac) HDS.power = HDS.power*m_boneHitProtection->m_fHitFrac;
		else
			HDS.power = NewHitPower;
	}

	if (g_Alive()) {
		const CCoverPoint	*cover = agent_manager().member().member(this).cover();
		if (cover && pHDS->initiator() && (pHDS->initiator()->ID() != ID()) && !fis_zero(pHDS->damage()) && brain().affect_cover())
			agent_manager().location().add	(xr_new<CDangerCoverLocation>(cover,Device.dwTimeGlobal,DANGER_INTERVAL,DANGER_DISTANCE));

		// Play hit-ref_sound
		const CEntityAlive	*entity_alive = smart_cast<const CEntityAlive*>(pHDS->initiator());
		if (!entity_alive || (tfGetRelationType(entity_alive) != ALife::eRelationTypeFriend))
			sound().play	(eStalkerSoundInjuring);
		else
			sound().play	(eStalkerSoundInjuringByFriend);
		
		if (animation().script_animations().empty() && (pHDS->bone() != BI_NONE)) {
			Fvector					D;
			float					yaw, pitch;
			D.getHP					(yaw,pitch);

#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
			float					power_factor = m_power_fx_factor*pHDS->damage()/100.f;
			clamp					(power_factor,0.f,1.f);
			CKinematicsAnimated		*tpKinematics = smart_cast<CKinematicsAnimated*>(Visual());
#ifdef DEBUG
			tpKinematics->LL_GetBoneInstance(pHDS->bone());
			if(pHDS->bone()>=tpKinematics->LL_BoneCount()){
				Msg("tpKinematics has no bone_id %d",pHDS->bone());
				pHDS->_dump();
			}
#endif
			int						fx_index = iFloor(tpKinematics->LL_GetBoneInstance(pHDS->bone()).get_param(1) + (angle_difference(movement().m_body.current.yaw,-yaw) <= PI_DIV_2 ? 0 : 1));
			if (fx_index != -1)
				animation().play_fx	(power_factor,fx_index);
		}
	}

	inherited::Hit(&HDS);
}

void CAI_Stalker::HitSignal				(float amount, Fvector& vLocalDir, CObject* who, s16 element)
{
	if (getDestroy())
		return;

	if (g_Alive())
		memory().hit().add	(amount,vLocalDir,who,element);
}

void CAI_Stalker::OnItemTake			(CInventoryItem *inventory_item)
{
	CObjectHandler::OnItemTake	(inventory_item);
	m_item_actuality			= false;
	m_sell_info_actuality		= false;
}

void CAI_Stalker::OnItemDrop			(CInventoryItem *inventory_item)
{
	CObjectHandler::OnItemDrop	(inventory_item);
	m_item_actuality			= false;
	m_sell_info_actuality		= false;
}

void CAI_Stalker::update_best_item_info	()
{
	ai().ef_storage().alife_evaluation(false);

	if	(
			m_item_actuality &&
			m_best_item_to_kill &&
			m_best_item_to_kill->can_kill()
		) {
		
		if (!memory().enemy().selected()) 
			return;

		ai().ef_storage().non_alife().member()	= this;
		ai().ef_storage().non_alife().enemy()	= memory().enemy().selected() ? memory().enemy().selected() : this;
		ai().ef_storage().non_alife().member_item()	= &m_best_item_to_kill->object();
		float									value;
		value									= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
		if (fsimilar(value,m_best_item_value))
			return;
	}

	// initialize parameters
	m_item_actuality							= true;
	ai().ef_storage().non_alife().member()		= this;
	ai().ef_storage().non_alife().enemy()		= memory().enemy().selected() ? memory().enemy().selected() : this;
	m_best_item_to_kill			= 0;
	m_best_ammo					= 0;
	m_best_found_item_to_kill	= 0;
	m_best_found_ammo			= 0;
	m_best_item_value			= 0.f;

	// try to find the best item which can kill
	{
		TIItemContainer::iterator					I = inventory().m_all.begin();
		TIItemContainer::iterator					E = inventory().m_all.end();
		for ( ; I != E; ++I) {
			if ((*I)->can_kill()) {
				ai().ef_storage().non_alife().member_item()	= &(*I)->object();
				float								value;
				if (memory().enemy().selected())
					value							= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
				else
					value							= (float)(*I)->object().ef_weapon_type();

				if (!fsimilar(value,m_best_item_value) && (value < m_best_item_value))
					continue;

				if (!fsimilar(value,m_best_item_value) && (value > m_best_item_value)) {
					m_best_item_value	= value;
					m_best_item_to_kill = *I;
					continue;
				}

				VERIFY					(fsimilar(value,m_best_item_value));
				if ((*I)->object().ef_weapon_type() <= m_best_item_to_kill->object().ef_weapon_type())
					continue;

				m_best_item_value		= value;
				m_best_item_to_kill		= *I;
			}
		}
	}

	// check if we found
	if (m_best_item_to_kill) {
		m_best_ammo				= m_best_item_to_kill;
		return;
	}

	// so we do not have such an item
	// check if we remember we saw item which can kill
	// or items which can make my item killing
	{
		xr_vector<const CGameObject*>::const_iterator	I = memory().item().objects().begin();
		xr_vector<const CGameObject*>::const_iterator	E = memory().item().objects().end();
		for ( ; I != E; ++I) {
			const CInventoryItem	*inventory_item = smart_cast<const CInventoryItem*>(*I);
			if (!inventory_item || !memory().item().useful(&inventory_item->object()))
				continue;
			CInventoryItem			*item			= inventory_item->can_kill(&inventory());
			if (item) {
				ai().ef_storage().non_alife().member_item()	= &inventory_item->object();
				float value							= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
				if (value > m_best_item_value) {
					m_best_item_value				= value;
					m_best_found_item_to_kill		= inventory_item;
					m_best_found_ammo				= 0;
					m_best_ammo						= item;
				}
			}
			else {
				item								= inventory_item->can_make_killing(&inventory());
				if (!item)
					continue;

				ai().ef_storage().non_alife().member_item()	= &item->object();
				float value							= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
				if (value > m_best_item_value) {
					m_best_item_value				= value;
					m_best_item_to_kill				= item;
					m_best_found_item_to_kill		= 0;
					m_best_found_ammo				= inventory_item;
				}
			}
		}
	}

	// check if we found such an item
	if (m_best_found_item_to_kill || m_best_found_ammo)
		return;

	// check if we remember we saw item to kill
	// and item which can make this item killing
	xr_vector<const CGameObject*>::const_iterator	I = memory().item().objects().begin();
	xr_vector<const CGameObject*>::const_iterator	E = memory().item().objects().end();
	for ( ; I != E; ++I) {
		const CInventoryItem	*inventory_item = smart_cast<const CInventoryItem*>(*I);
		if (!inventory_item || !memory().item().useful(&inventory_item->object()))
			continue;
		const CInventoryItem	*item = inventory_item->can_kill(memory().item().objects());
		if (item) {
			ai().ef_storage().non_alife().member_item()	= &inventory_item->object();
			float value							= ai().ef_storage().m_pfWeaponEffectiveness->ffGetValue();
			if (value > m_best_item_value) {
				m_best_item_value			= value;
				m_best_found_item_to_kill	= inventory_item;
				m_best_found_ammo			= item;
			}
		}
	}
}

bool CAI_Stalker::item_to_kill			()
{
	update_best_item_info	();
	return					(!!m_best_item_to_kill);
}

bool CAI_Stalker::item_can_kill			()
{
	update_best_item_info	();
	return					(!!m_best_ammo);
}

bool CAI_Stalker::remember_item_to_kill	()
{
	update_best_item_info	();
	return					(!!m_best_found_item_to_kill);
}

bool CAI_Stalker::remember_ammo			()
{
	update_best_item_info	();
	return					(!!m_best_found_ammo);
}

bool CAI_Stalker::ready_to_kill			()
{
	return					(
		m_best_item_to_kill && 
		inventory().ActiveItem() && 
		(inventory().ActiveItem()->object().ID() == m_best_item_to_kill->object().ID()) &&
		m_best_item_to_kill->ready_to_kill()
	);
}

bool CAI_Stalker::ready_to_detour		()
{
	if (!ready_to_kill())
		return			(false);

	CWeapon				*weapon = smart_cast<CWeapon*>(m_best_item_to_kill);
	if (!weapon)
		return			(true);

	return				(weapon->GetAmmoElapsed() > weapon->GetAmmoMagSize()/2);
}

class ray_query_param	{
public:
	CAI_Stalker				*m_holder;
	float					m_power;
	float					m_power_threshold;
	bool					m_can_kill_enemy;
	bool					m_can_kill_member;
	float					m_pick_distance;

public:
	IC				ray_query_param		(const CAI_Stalker *holder, float power_threshold, float distance)
	{
		m_holder			= const_cast<CAI_Stalker*>(holder);
		m_power_threshold	= power_threshold;
		m_power				= 1.f;
		m_can_kill_enemy	= false;
		m_can_kill_member	= false;
		m_pick_distance		= distance;
	}
};

IC BOOL ray_query_callback	(collide::rq_result& result, LPVOID params)
{
	ray_query_param						*param = (ray_query_param*)params;
	float								power = param->m_holder->feel_vision_mtl_transp(result.O,result.element);
	param->m_power						*= power;

	if (power >= .05f) {
		param->m_pick_distance			= result.range;
		return							(true);
	}

	if (!result.O) {
		if (param->m_power > param->m_power_threshold)
			return						(true);

		param->m_pick_distance			= result.range;
		return							(false);
	}

	CEntityAlive						*entity_alive = smart_cast<CEntityAlive*>(result.O);
	if (!entity_alive) {
		if (param->m_power > param->m_power_threshold)
			return						(true);

		param->m_pick_distance			= result.range;
		return							(false);
	}

	if (param->m_holder->is_relation_enemy(entity_alive))
		param->m_can_kill_enemy			= true;
	else
		param->m_can_kill_member		= true;

	param->m_pick_distance				= result.range;
	return								(false);
}

void CAI_Stalker::can_kill_entity		(const Fvector &position, const Fvector &direction, float distance, collide::rq_results& rq_storage)
{
	VERIFY							(!fis_zero(direction.square_magnitude()));

	collide::ray_defs				ray_defs(position,direction,distance,CDB::OPT_CULL,collide::rqtBoth);
	VERIFY							(!fis_zero(ray_defs.dir.square_magnitude()));
	
	ray_query_param					params(this,memory().visual().transparency_threshold(),distance);

	Level().ObjectSpace.RayQuery	(rq_storage,ray_defs,ray_query_callback,&params,NULL,this);
	m_can_kill_enemy				= m_can_kill_enemy  || params.m_can_kill_enemy;
	m_can_kill_member				= m_can_kill_member || params.m_can_kill_member;
	m_pick_distance					= _max(m_pick_distance,params.m_pick_distance);
}

void CAI_Stalker::can_kill_entity_from	(const Fvector &position, Fvector direction, float distance)
{
	m_pick_distance			= 0.f;
	collide::rq_results		rq_storage;
	can_kill_entity			(position,direction,distance,rq_storage);
	if (m_can_kill_member && m_can_kill_enemy)
		return;

	float					yaw, pitch, safety_fire_angle = 1.f*PI_DIV_8*.5f;
	direction.getHP			(yaw,pitch);

	direction.setHP			(yaw - safety_fire_angle,pitch);
	can_kill_entity			(position,direction,distance,rq_storage);
	if (m_can_kill_member && m_can_kill_enemy)
		return;

	direction.setHP			(yaw + safety_fire_angle,pitch);
	can_kill_entity			(position,direction,distance,rq_storage);
	if (m_can_kill_member && m_can_kill_enemy)
		return;

	direction.setHP			(yaw,pitch - safety_fire_angle);
	can_kill_entity			(position,direction,distance,rq_storage);
	if (m_can_kill_member && m_can_kill_enemy)
		return;

	direction.setHP			(yaw,pitch + safety_fire_angle);
	can_kill_entity			(position,direction,distance,rq_storage);
}

IC	float CAI_Stalker::start_pick_distance	() const
{
	float					result = 50.f;
	if (!memory().enemy().selected())
		return				(result);

	return					(
		_max(
			result,
			memory().enemy().selected()->Position().distance_to(Position()) + 1.f
		)
	);
}

float CAI_Stalker::pick_distance		()
{
	if (!inventory().ActiveItem())
		return				(start_pick_distance());
		
	update_can_kill_info	();
	return					(m_pick_distance);
}

void CAI_Stalker::update_can_kill_info	()
{
	if (m_pick_frame_id == Device.dwFrame)
		return;

	m_pick_frame_id			= Device.dwFrame;
	m_can_kill_member		= false;
	m_can_kill_enemy		= false;

	Fvector					position, direction;
	g_fireParams			(0,position,direction);
	can_kill_entity_from	(position,direction,start_pick_distance());
}

bool CAI_Stalker::undetected_anomaly	()
{
	return					(inside_anomaly() || brain().CStalkerPlanner::m_storage.property(StalkerDecisionSpace::eWorldPropertyAnomaly));
}

bool CAI_Stalker::inside_anomaly		()
{
	xr_vector<CObject*>::const_iterator	I = feel_touch.begin();
	xr_vector<CObject*>::const_iterator	E = feel_touch.end();
	for ( ; I != E; ++I) {
		CCustomZone			*zone = smart_cast<CCustomZone*>(*I);
		if (zone) {
			if (smart_cast<CRadioactiveZone*>(zone))
				continue;

			return			(true);
		}
	}
	return					(false);
}

bool CAI_Stalker::zoom_state			() const
{
	if (!inventory().ActiveItem())
		return				(false);

	if ((movement().movement_type() != eMovementTypeStand) && (movement().body_state() != eBodyStateCrouch) && !movement().path_completed())
		return				(false);

	switch (CObjectHandler::planner().current_action_state_id()) {
		case ObjectHandlerSpace::eWorldOperatorAim1 :
		case ObjectHandlerSpace::eWorldOperatorAim2 :
		case ObjectHandlerSpace::eWorldOperatorAimingReady1 :
		case ObjectHandlerSpace::eWorldOperatorAimingReady2 :
		case ObjectHandlerSpace::eWorldOperatorQueueWait1 :
		case ObjectHandlerSpace::eWorldOperatorQueueWait2 :
		case ObjectHandlerSpace::eWorldOperatorFire1 :
		// we need this 2 operators to prevent fov/range twitching
		case ObjectHandlerSpace::eWorldOperatorReload1 :
		case ObjectHandlerSpace::eWorldOperatorReload2 :
		case ObjectHandlerSpace::eWorldOperatorForceReload1 :
		case ObjectHandlerSpace::eWorldOperatorForceReload2 :
			return			(true);
	}

	return					(false);
}

void CAI_Stalker::update_range_fov		(float &new_range, float &new_fov, float start_range, float start_fov)
{
	float					range = start_range, fov = start_fov;

	if (zoom_state())
		inventory().ActiveItem()->modify_holder_params(range,fov);

	return					(inherited::update_range_fov(new_range,new_fov,range,fov));
}

bool CAI_Stalker::fire_make_sense		()
{
	const CEntityAlive		*enemy = memory().enemy().selected();
	if (!enemy)
		return				(false);

	if ((pick_distance() + PRECISE_DISTANCE) < Position().distance_to(enemy->Position()))
		return				(false);

	if (_abs(Position().y - enemy->Position().y) > FLOOR_DISTANCE)
		return				(false);

	if (pick_distance() < NEAR_DISTANCE)
		return				(false);

	if (memory().visual().visible_right_now(enemy))
		return				(true);

	u32						last_time_seen = memory().visual().visible_object_time_last_seen(enemy);
	if (last_time_seen == u32(-1))
		return				(false);

	if (Device.dwTimeGlobal > last_time_seen + FIRE_MAKE_SENSE_INTERVAL)
		return				(false);

	return					(true);
}

// shot effector stuff
void CAI_Stalker::on_weapon_shot_start		(CWeapon *weapon)
{
	weapon_shot_effector().SetRndSeed	(m_weapon_shot_random_seed);
	weapon_shot_effector().Shot			(weapon->camDispersion + weapon->camDispersionInc*float(weapon->ShotsFired()));
}

void CAI_Stalker::on_weapon_shot_stop		(CWeapon *weapon)
{
}

void CAI_Stalker::on_weapon_hide			(CWeapon *weapon)
{
}
