#pragma once
#include "../BaseMonster/base_monster.h"
#include "../telekinesis.h"
#include "../anim_triple.h"
#include "../scanning_ability.h"
#include "../../../script_export_space.h"

class CCharacterPhysicsSupport;

class CBurer :	public CBaseMonster,
				public CTelekinesis,
				public CScanningAbility<CBurer> {

	typedef		CBaseMonster				inherited;

public:
	typedef		CScanningAbility<CBurer>	TScanner;


	u32			last_hit_frame;

	u32			time_last_scan;
	

	typedef		CTelekinesis				TTelekinesis;

	struct	GraviObject {
		bool		active;
		Fvector		cur_pos;
		Fvector		target_pos;
		Fvector		from_pos;

		u32			time_last_update;

		const CEntityAlive *enemy;
		
		GraviObject() {
			active = false;
			enemy = 0;
		}
		
		
		void		activate(const CEntityAlive *e, const Fvector &cp, const Fvector &tp) {
			active				= true;
			from_pos			= cp;
			cur_pos				= cp;
			target_pos			= tp;
			time_last_update	= Device.dwTimeGlobal;
			enemy				= e;
		}

		void		deactivate() {
			active = false;
		}

	} m_gravi_object;

	LPCSTR	 particle_gravi_wave;
	LPCSTR   particle_gravi_prepare;
	LPCSTR	 particle_tele_object;

	ref_sound	sound_gravi_wave;
	ref_sound	sound_scan;

	u32		m_gravi_speed;
	u32		m_gravi_step;
	u32		m_gravi_time_to_hold;
	float	m_gravi_radius;
	float	m_gravi_impulse_to_objects;
	float	m_gravi_impulse_to_enemy;
	float	m_gravi_hit_power;
	

	u32		m_tele_max_handled_objects;
	u32		m_tele_time_to_hold;
	float	m_tele_object_min_mass;
	float	m_tele_object_max_mass;
	float	m_tele_find_radius;


	bool	m_shield_active;
	LPCSTR	particle_fire_shield;
public:
					CBurer				();
	virtual			~CBurer				();	


	virtual void	reinit				();
	virtual void	reload				(LPCSTR section);

	virtual void	Load				(LPCSTR section);

	virtual	void	shedule_Update		(u32 dt);
	virtual void	UpdateCL			();
	virtual void	Hit					(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type);
	virtual void	Die					(CObject* who);
			void	ProcessTurn			();
	virtual void	CheckSpecParams		(u32 spec_params);

			void	UpdateGraviObject	();

			void	StartGraviPrepare	();
			void	StopGraviPrepare	();

			void	StartTeleObjectParticle(CGameObject *pO);
			void	StopTeleObjectParticle(CGameObject *pO);

			void	ActivateShield		() {m_shield_active = true;}
			void	DeactivateShield	() {m_shield_active = false;}

	virtual bool	ability_distant_feel() {return true;}

	virtual void	on_scanning			();
	virtual void	on_scan_success		();

public:
	CAnimTriple		anim_triple_gravi;
	CAnimTriple		anim_triple_tele;


#ifdef DEBUG
	virtual CBaseMonster::SDebugInfo show_debug_info();
#endif


	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CBurer)
#undef script_type_list
#define script_type_list save_type_list(CBurer)
