#include "stdafx.h"
#include "ai_bloodsucker.h"
#include "ai_bloodsucker_effector.h"
#include "../ai_monsters_misc.h"

#include "../ai_monster_utils.h"
#include "../ai_monster_effector.h"


CAI_Bloodsucker::CAI_Bloodsucker()
{
	stateRest			= xr_new<CBitingRest>			(this);
	stateEat			= xr_new<CBitingEat>			(this);
	stateAttack			= xr_new<CBitingAttack>			(this);
	statePanic			= xr_new<CBitingPanic>			(this);
	stateExploreDNE		= xr_new<CBitingExploreDNE>		(this);
	stateExploreNDE		= xr_new<CBitingExploreNDE>		(this);
	stateSquadTask		= xr_new<CBitingSquadTask>		(this);
	
	Init();
}

CAI_Bloodsucker::~CAI_Bloodsucker()
{
	xr_delete(stateRest);
	xr_delete(stateEat);
	xr_delete(stateAttack);
	xr_delete(statePanic);
	xr_delete(stateExploreDNE);
	xr_delete(stateExploreNDE);
	xr_delete(stateSquadTask);
}


void CAI_Bloodsucker::Init()
{
	inherited::Init();

	m_fEffectDist					= 0.f;

	CurrentState					= stateRest;
	CurrentState->Reset				();

	Bones.Reset();

	invisible_vel.set				(0.1f, 0.1f);
}

void CAI_Bloodsucker::Load(LPCSTR section) 
{
	inherited::Load(section);

	CMonsterInvisibility::Load(section);

	m_fInvisibilityDist = pSettings->r_float(section,"InvisibilityDist");
	m_ftrPowerDown		= pSettings->r_float(section,"PowerDownFactor");	
	m_fPowerThreshold	= pSettings->r_float(section,"PowerThreshold");	
	m_fEffectDist		= pSettings->r_float(section,"EffectDistance");	

	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	MotionMan.AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);
	
	BEGIN_LOAD_SHARED_MOTION_DATA();

	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimSleep,			"lie_sleep_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_LIE,	  	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &inherited::_sd->m_fsVelocityWalkFwdNormal,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &inherited::_sd->m_fsVelocityWalkFwdDamaged,	PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	&inherited::_sd->m_fsVelocityRunFwdNormal,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&inherited::_sd->m_fsVelocityRunFwdDamaged,		PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&inherited::_sd->m_fsVelocityNone,				PS_STAND, 	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimEat,				"sit_eat_",				-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimDie,				"stand_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		-1, &inherited::_sd->m_fsVelocityStandTurn,			PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimLookAround,		"stand_look_around_",	-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimSitIdle,			"sit_idle_",			-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimSitToSleep,		"sit_sleep_down_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_SIT,		"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimSteal,			"stand_steal_",			-1, &inherited::_sd->m_fsVelocitySteal,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimThreaten,		"stand_threaten_",		-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");
	MotionMan.AddAnim(eAnimMiscAction_00,	"stand_to_aggressive_",	-1, &inherited::_sd->m_fsVelocityNone,				PS_STAND,	"fx_run_f", "fx_stand_b", "fx_stand_l", "fx_stand_r");	
	
	// define transitions
//	MotionMan.AddTransition(PS_STAND,			eAnimThreaten,	eAnimMiscAction_00,	false);
	MotionMan.AddTransition(eAnimStandSitDown,	eAnimSleep,		eAnimSitToSleep,	false);
	MotionMan.AddTransition(PS_STAND,			eAnimSleep,		eAnimStandSitDown,	true);
	MotionMan.AddTransition(PS_STAND,			PS_SIT,			eAnimStandSitDown,	false);
	MotionMan.AddTransition(PS_STAND,			PS_LIE,			eAnimStandSitDown,	false);
	MotionMan.AddTransition(PS_SIT,				PS_STAND,		eAnimSitStandUp,	false);
	MotionMan.AddTransition(PS_LIE,				PS_STAND,		eAnimSitStandUp,	false);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, PI_DIV_6/5);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimSitIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimSitIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimWalkBkwd);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack,	eAnimRun,			eAnimRun,			PI_DIV_6);
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimLookAround); 
	MotionMan.LinkAction(ACT_TURN,			eAnimStandIdle,	eAnimStandTurnLeft, eAnimStandTurnRight, EPS_S); 

	MotionMan.AA_Load(pSettings->r_string(section, "attack_params"));
	MotionMan.STEPS_Load(pSettings->r_string(section, "step_params"), get_legs_number());

	END_LOAD_SHARED_MOTION_DATA();

	MotionMan.accel_load			(section);
	MotionMan.accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	MotionMan.accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	LoadEffector(pSettings->r_string(section,"postprocess_new"));

	invisible_vel.set(pSettings->r_float(section,"Velocity_Invisible_Linear"),pSettings->r_float(section,"Velocity_Invisible_Angular"));
	m_movement_params.insert(std::make_pair(eVelocityParameterInvisible,STravelParams(invisible_vel.linear, invisible_vel.angular)));
	
	invisible_particle_name = pSettings->r_string(section,"Particle_Invisible");

}


void CAI_Bloodsucker::LoadEffector(LPCSTR section)
{
	pp_effector.duality.h			= pSettings->r_float(section,"duality_h");
	pp_effector.duality.v			= pSettings->r_float(section,"duality_v");
	pp_effector.gray				= pSettings->r_float(section,"gray");
	pp_effector.blur				= pSettings->r_float(section,"blur");
	pp_effector.noise.intensity		= pSettings->r_float(section,"noise_intensity");
	pp_effector.noise.grain			= pSettings->r_float(section,"noise_grain");
	pp_effector.noise.fps			= pSettings->r_float(section,"noise_fps");

	sscanf(pSettings->r_string(section,"color_base"),	"%f,%f,%f", &pp_effector.color_base.r, &pp_effector.color_base.g, &pp_effector.color_base.b);
	sscanf(pSettings->r_string(section,"color_gray"),	"%f,%f,%f", &pp_effector.color_gray.r, &pp_effector.color_gray.g, &pp_effector.color_gray.b);
	sscanf(pSettings->r_string(section,"color_add"),	"%f,%f,%f", &pp_effector.color_add.r,  &pp_effector.color_add.g,  &pp_effector.color_add.b);
}




void __stdcall CAI_Bloodsucker::BoneCallback(CBoneInstance *B)
{
	CAI_Bloodsucker*	this_class = dynamic_cast<CAI_Bloodsucker*> (static_cast<CObject*>(B->Callback_Param));

	this_class->Bones.Update(B, Level().timeServer());
}


void CAI_Bloodsucker::vfAssignBones()
{
	// ��������� callback �� �����
	bone_spine =	&PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_spine"));
	bone_head =		&PKinematics(Visual())->LL_GetBoneInstance(PKinematics(Visual())->LL_BoneID("bip01_head"));
	bone_spine->set_callback(BoneCallback,this);
	bone_head->set_callback(BoneCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(bone_spine, AXIS_X);	Bones.AddBone(bone_spine, AXIS_Y);
	Bones.AddBone(bone_head, AXIS_X);	Bones.AddBone(bone_head, AXIS_Y);
}


#define MAX_BONE_ANGLE PI_DIV_4

void CAI_Bloodsucker::LookDirection(Fvector to_dir, float bone_turn_speed)
{
	// �������� ������ ����������� � ��������� ����� � ��� ������� ����
	float		yaw,pitch;
	to_dir.getHP(yaw,pitch);

	// ���������� ��������� �������� �� yaw
	float cur_yaw = -m_body.current.yaw;						// ������� ������� ���� �������
	float bone_angle;											// ���� ��� ����	

	float dy = _abs(angle_normalize_signed(yaw - cur_yaw));		// ������, �� ������� ����� ��������������

	if (angle_difference(cur_yaw,yaw) <= MAX_BONE_ANGLE) {		// bone turn only
		bone_angle = dy;
	} else {													// torso & bone turn 
		if (IsMoveAlongPathFinished() || !CMovementManager::enabled()) m_body.target.yaw = angle_normalize(-yaw);
		if (dy / 2 < MAX_BONE_ANGLE) bone_angle = dy / 2;
		else bone_angle = MAX_BONE_ANGLE;
	}

	bone_angle /= 2;
	if (from_right(yaw,cur_yaw)) bone_angle *= -1.f;

	Bones.SetMotion(bone_spine, AXIS_X, bone_angle, bone_turn_speed, 100);
	Bones.SetMotion(bone_head,	AXIS_X, bone_angle, bone_turn_speed, 100);

	// ���������� ��������� �������� �� pitch
	clamp(pitch, -MAX_BONE_ANGLE, MAX_BONE_ANGLE);
	pitch /= 2; 

	Bones.SetMotion(bone_spine, AXIS_Y, pitch, bone_turn_speed, 100);
	Bones.SetMotion(bone_head,	AXIS_Y, pitch, bone_turn_speed, 100);	
}

void CAI_Bloodsucker::LookPosition(Fvector to_point, float angular_speed)
{
	Fvector	dir;
	dir.set(to_point);
	dir.sub(Position());
	LookDirection(dir,angular_speed);
}

void CAI_Bloodsucker::ActivateEffector(float life_time)
{
	//Level().Cameras.AddEffector(xr_new<CMonsterEffector>(pp_effector, life_time, 0.3f, 0.9f));
}


void CAI_Bloodsucker::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}

	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		MotionMan.SetCurAnim(eAnimThreaten);
		return;
	}

	if ((spec_params & ASP_STAND_SCARED) == ASP_STAND_SCARED) {
		MotionMan.SetCurAnim(eAnimLookAround);
		return;
	}

}

BOOL CAI_Bloodsucker::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	vfAssignBones	();

	return(TRUE);
}

void CAI_Bloodsucker::UpdateCL()
{
	inherited::UpdateCL();

	// Blink processing
	bool PrevVis	=	IsCurrentVisible();
	bool NewVis		=	CMonsterInvisibility::Update();
	if (NewVis != PrevVis) setVisible(NewVis);

	if (!CMonsterInvisibility::IsActiveBlinking()) state_invisible = !PrevVis;
	else {
		if (state_invisible) {
			CMovementManager::enable_movement(false);
			CMovementManager::enable_movement(true);
			m_velocity_linear.current = m_velocity_linear.target = 0.f;
		}
	}

}

void CAI_Bloodsucker::StateSelector()
{
	IState *pState = CurrentState;

	if (EnemyMan.get_enemy()) {
		switch (EnemyMan.get_danger_type()) {
			case eVeryStrong:				pState = statePanic; break;
			case eStrong:		
			case eNormal:
			case eWeak:						pState = stateAttack; break;
		}
	} else if (hear_dangerous_sound || hear_interesting_sound) {
		if (hear_dangerous_sound)			pState = stateExploreNDE;		
		if (hear_interesting_sound)			pState = stateExploreNDE;	
	} else	if (CorpseMan.get_corpse() && ((GetSatiety() < _sd->m_fMinSatiety) || flagEatNow))					
											pState= stateEat;	
	else									pState = stateRest;
	
	SetState(pState);
}


////////////////////////////////////////////////////////////////////////////////////////////////

void CAI_Bloodsucker::set_visible(bool val) 
{
	CMonsterInvisibility::Switch(val);
	CParticlesPlayer::StartParticles(invisible_particle_name,Fvector().set(0.0f,0.1f,0.0f),ID());
}

void CAI_Bloodsucker::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);

	if (state_invisible) {
		SGameMtlPair* mtl_pair		= CMaterialManager::get_current_pair();
		if (!mtl_pair) return;

		R_ASSERT2(mtl_pair->CollideParticles.size()>0, "Collide Particles have not been set for current material pair!");

		LPCSTR ps_name = *mtl_pair->CollideParticles[::Random.randI(0,mtl_pair->CollideParticles.size())];

		//�������� �������� ������������ ����������
		CParticlesObject* ps = xr_new<CParticlesObject>(ps_name);

		// ��������� ������� � �������������� ��������
		Fmatrix pos; 

		// ���������� �����������
		pos.k.set(Fvector().set(0.0f,1.0f,0.0f));
		Fvector::generate_orthonormal_basis(pos.k, pos.i, pos.j);
		// ���������� �������
		pos.c.set(get_foot_position(eFrontLeft));

		ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
		Level().ps_needtoplay.push_back(ps);
	}
}


