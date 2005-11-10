#pragma once
#include "../BaseMonster/base_monster.h"
#include "../anim_triple.h"
#include "controller_psy_aura.h"
#include "../../../script_export_space.h"

class CControllerAnimation;
class CControllerDirection;

class SndShockEffector;

class CControllerPsyHit;

class CController : public CBaseMonster, 
					public CPsyAuraController {

	typedef		CBaseMonster	inherited;

	u8					m_max_controlled_number;
	ref_sound			control_start_sound;		// ����, ������� �������� � ������ � ������
	ref_sound			control_hit_sound;			// ����, ������� �������� � ������ � ������

	ref_sound			m_sound_hit_fx;
	SndShockEffector*	m_sndShockEffector;					

	ref_sound			m_sound_aura_left_channel;
	ref_sound			m_sound_aura_right_channel;
	ref_sound			m_sound_aura_hit_left_channel;
	ref_sound			m_sound_aura_hit_right_channel;


	SAttackEffector		m_control_effector;

	u32					time_control_hit_started;
	bool				active_control_fx;
	
	bool				int_need_deactivate;		// internal var

	LPCSTR				particles_fire;

	CControllerAnimation	*m_custom_anim_base;
	CControllerDirection	*m_custom_dir_base;

	u32					m_psy_fire_start_time;
	u32					m_psy_fire_delay;

	CControllerPsyHit	*m_psy_hit;

public:	
	SVelocityParam		m_velocity_move_fwd;
	SVelocityParam		m_velocity_move_bkwd;

public:	
	CControllerAnimation	&custom_anim()	{return (*m_custom_anim_base);}
	CControllerDirection	&custom_dir()	{return (*m_custom_dir_base);}

public:
	xr_vector<CEntity*> m_controlled_objects;

public:
					CController			();
	virtual			~CController		();	

	virtual void	Load				(LPCSTR section);
	virtual void	reload				(LPCSTR section);
	virtual void	reinit				();
	virtual void	UpdateCL			();
	virtual void	shedule_Update		(u32 dt);
	virtual void	Die					(CObject* who);

	virtual void	net_Destroy			();
	virtual BOOL	net_Spawn			(CSE_Abstract *DC);
	virtual void	net_Relcase			(CObject *O);

	virtual	void	CheckSpecParams		(u32 spec_params);

	virtual	void	PitchCorrection		() {}

	virtual void	InitThink			();

	virtual void	create_base_controls();	
	
	virtual const MonsterSpace::SBoneRotation &head_orientation	() const;

	virtual void	TranslateActionToPathParams	();

	//-------------------------------------------------------------------

	virtual	bool	is_relation_enemy	(const CEntityAlive *tpEntityAlive) const;
	xr_vector<shared_str>				m_friend_community_overrides;
	void								load_friend_community_overrides	(LPCSTR section);
	bool								is_community_friend_overrides	(const CEntityAlive *tpEntityAlive) const;
	//-------------------------------------------------------------------
	// Controller ability
			bool	HasUnderControl		() {return (!m_controlled_objects.empty());}
			void	TakeUnderControl	(CEntity *);
			void	UpdateControlled	();
			void	FreeFromControl		();
			void	OnFreedFromControl	(const CEntity *);  // ���� ������ ��� ���� ��������� (destroyed || die)

			void	set_controlled_task (u32 task);



			void	play_control_sound_start	();
			void	play_control_sound_hit		();

			void	control_hit					();

			void	psy_fire					();
			bool	can_psy_fire				();
			
			void	set_psy_fire_delay_zero		();
			void	set_psy_fire_delay_default	();


	//-------------------------------------------------------------------

public: 

	void						draw_fire_particles();
	
	void						test_covers();


public:
	enum EMentalState {
		eStateIdle,
		eStateDanger
	} m_mental_state;

	void			set_mental_state			(EMentalState state){m_mental_state = state;}

public:
	virtual bool	use_center_to_aim			() const {return true;}

	SAnimationTripleData anim_triple_control;

#ifdef DEBUG
	virtual CBaseMonster::SDebugInfo show_debug_info();

#endif

private:
#ifdef _DEBUG	
		virtual void	debug_on_key		(int key);
#endif


	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CController)
#undef script_type_list
#define script_type_list save_type_list(CController)

