////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.cpp
//	Created 	: 23.04.2002
//  Modified 	: 07.11.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_rat.h"
#include "../ai_monsters_misc.h"
#include "../../PhysicsShell.h"
#include "../../game_graph.h"
#include "../../game_level_cross_table.h"
#include "../../xrserver_objects_alife_monsters.h"
#include "ai_rat_space.h"

using namespace RatSpace;

CAI_Rat::CAI_Rat()
{
	init					();
}

CAI_Rat::~CAI_Rat()
{
}

void CAI_Rat::init()
{
	m_tAction				= eRatActionNone;
	m_hit_direction.set		(0,0,1);
	m_hit_time				= 0;
	m_tpCurrentGlobalAnimation = 0;
	m_tpCurrentGlobalBlend	= 0;
	m_bActionStarted		= false;
	m_bFiring				= false;
	m_previous_query_time		= 0;
	m_tGoalDir.set			(10.0f*(Random.randF()-Random.randF()),10.0f*(Random.randF()-Random.randF()),10.0f*(Random.randF()-Random.randF()));
	m_tCurrentDir			= m_tGoalDir;
	m_tHPB.set				(0,0,0);
	m_fDHeading				= 0;
	m_fGoalChangeTime		= 0.f;
	m_tLastSound.tpEntity	= 0;
	m_tLastSound.dwTime		= 0;
	m_tLastSound.eSoundType	= SOUND_TYPE_NO_SOUND;
	m_bNoWay				= false;
	m_dwMoraleLastUpdateTime = 0;
	m_bStanding				= false;
	m_bActive				= false;
	m_dwStartAttackTime		= 0;
	m_saved_impulse			= 0.f;
	m_bMoving				= false;
	m_bCanAdjustSpeed		= false;
	m_bStraightForward		= false;
	m_thinking				= false;
	m_turning				= false;
}

void CAI_Rat::reinit					()
{
	inherited::reinit		();
	CEatableItem::reinit	();
	init					();
}

void CAI_Rat::reload					(LPCSTR	section)
{
	inherited::reload		(section);
	CEatableItem::reload	(section);
	LPCSTR					head_bone_name = pSettings->r_string(section,"bone_head");
	CSoundPlayer::add		(pSettings->r_string(section,"sound_death"),	100, SOUND_TYPE_MONSTER_DYING,		0, u32(eRatSoundMaskDie),		eRatSoundDie,		head_bone_name);
	CSoundPlayer::add		(pSettings->r_string(section,"sound_hit"),		100, SOUND_TYPE_MONSTER_INJURING,	1, u32(eRatSoundMaskInjuring),	eRatSoundInjuring,	head_bone_name);
	CSoundPlayer::add		(pSettings->r_string(section,"sound_attack"),	100, SOUND_TYPE_MONSTER_ATTACKING,	2, u32(eRatSoundMaskAttack),	eRatSoundAttack,	head_bone_name);
	CSoundPlayer::add		(pSettings->r_string(section,"sound_voice"),	100, SOUND_TYPE_MONSTER_TALKING,	4, u32(eRatSoundMaskVoice),		eRatSoundVoice,		head_bone_name);
	CSoundPlayer::add		(pSettings->r_string(section,"sound_eat"),		100, SOUND_TYPE_MONSTER_EATING	,	3, u32(eRatSoundMaskEat),		eRatSoundEat,		head_bone_name);
}

void CAI_Rat::Die()
{
	inherited::Die( );
	m_eCurrentState = aiRatDie;

	SelectAnimation(XFORM().k,direction(),speed());

	CSoundPlayer::play		(eRatSoundDie);

	vfUpdateMoraleBroadcast(m_fMoraleDeathQuant,m_fMoraleDeathDistance);

	CGroupHierarchyHolder &Group = Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group());
	vfRemoveActiveMember();
	vfRemoveStandingMember();
	--(Group.m_dwAliveCount);
	m_eCurrentState = aiRatDie;
}

void CAI_Rat::Load(LPCSTR section)
{ 
	init();

	inherited::Load(section);
	CEatableItem::Load(section);

	// initialize start position
	Fvector	P						= Position();
	P.x								+= ::Random.randF();
	P.z								+= ::Random.randF();

	// active\passive
	m_fChangeActiveStateProbability = pSettings->r_float (section,"ChangeActiveStateProbability");
	m_dwPassiveScheduleMin			= pSettings->r_s32   (section,"PassiveScheduleMin");
	m_dwPassiveScheduleMax			= pSettings->r_s32   (section,"PassiveScheduleMax");
	m_dwActiveCountPercent			= pSettings->r_s32   (section,"ActiveCountPercent");
	m_dwStandingCountPercent		= pSettings->r_s32   (section,"StandingCountPercent");

	// eye shift
	m_tEyeShift.y					= pSettings->r_float (section,"EyeYShift");

	// former constants
	m_dwLostMemoryTime				= pSettings->r_s32   (section,"LostMemoryTime");
	m_dwLostRecoilTime				= pSettings->r_s32   (section,"LostRecoilTime");
	m_fUnderFireDistance			= pSettings->r_float (section,"UnderFireDistance");
	m_dwRetreatTime					= pSettings->r_s32   (section,"RetreatTime");
	m_fRetreatDistance				= pSettings->r_float (section,"RetreatDistance");
	m_fAttackStraightDistance		= pSettings->r_float (section,"AttackStraightDistance");
	m_fStableDistance				= pSettings->r_float (section,"StableDistance");
	m_fWallMinTurnValue				= pSettings->r_float (section,"WallMinTurnValue")/180.f*PI;
	m_fWallMaxTurnValue				= pSettings->r_float (section,"WallMaxTurnValue")/180.f*PI;

	m_fAngleSpeed					= pSettings->r_float (section,"AngleSpeed");
	m_fSafeGoalChangeDelta	 		= pSettings->r_float (section,"GoalChangeDelta");
	m_tGoalVariation	  			= pSettings->r_fvector3(section,"GoalVariation");

	m_fMoraleDeathDistance	 		= pSettings->r_float (section,"MoraleDeathDistance");
	m_dwActionRefreshRate	 		= pSettings->r_s32   (section,"ActionRefreshRate");

	m_fMaxHealthValue	 			= pSettings->r_float (section,"MaxHealthValue");
	m_fSoundThreshold				= pSettings->r_float (section,"SoundThreshold");

	m_bEatMemberCorpses				= pSettings->r_bool  (section,"EatMemberCorpses");
	m_bCannibalism					= pSettings->r_bool  (section,"Cannibalism");
	m_dwEatCorpseInterval			= pSettings->r_s32   (section,"EatCorpseInterval");

	m_fNullASpeed					= pSettings->r_float (section,"AngularStandSpeed")/180.f*PI;//PI_MUL_2
	m_fMinASpeed					= pSettings->r_float (section,"AngularMinSpeed")/180.f*PI;//PI_MUL_2
	m_fMaxASpeed					= pSettings->r_float (section,"AngularMaxSpeed")/180.f*PI;//.2f
	m_fAttackASpeed					= pSettings->r_float (section,"AngularAttackSpeed")/180.f*PI;//.15f;

	m_phMass						= pSettings->r_float (section,"corp_mass");
	m_dwActiveScheduleMin			= shedule.t_min;
	m_dwActiveScheduleMax			= shedule.t_max;

	m_fGoingSpeed					= pSettings->r_float	(section, "going_speed");
}

BOOL CAI_Rat::net_Spawn	(LPVOID DC)
{
	//////////////////////////////////////////////////////////////////////////
	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeMonsterRat				*tpSE_Rat = smart_cast<CSE_ALifeMonsterRat*>(e);
	// model
	if (!inherited::net_Spawn(DC))
		return(FALSE);
	// model
	if (!CEatableItem::net_Spawn(DC))
		return(FALSE);
	// personal characteristics
	m_body.current.yaw				= m_body.target.yaw	= -tpSE_Rat->o_Angle.y;
	m_body.current.pitch			= m_body.target.pitch	= 0;
	m_body.speed					= PI_MUL_2;

	eye_fov							= tpSE_Rat->fEyeFov;
	eye_range						= tpSE_Rat->fEyeRange;
	fEntityHealth					= tpSE_Rat->fHealth;
	m_fMinSpeed						= tpSE_Rat->fMinSpeed;
	m_fMaxSpeed						= tpSE_Rat->fMaxSpeed;
	m_fAttackSpeed					= tpSE_Rat->fAttackSpeed;
	m_fMaxPursuitRadius				= tpSE_Rat->fMaxPursuitRadius;
	m_fMaxHomeRadius				= tpSE_Rat->fMaxHomeRadius;
	// morale
	m_fMoraleSuccessAttackQuant		= tpSE_Rat->fMoraleSuccessAttackQuant;
	m_fMoraleDeathQuant				= tpSE_Rat->fMoraleDeathQuant;
	m_fMoraleFearQuant				= tpSE_Rat->fMoraleFearQuant;
	m_fMoraleRestoreQuant			= tpSE_Rat->fMoraleRestoreQuant;
	m_dwMoraleRestoreTimeInterval	= tpSE_Rat->u16MoraleRestoreTimeInterval;
	m_fMoraleMinValue				= tpSE_Rat->fMoraleMinValue;
	m_fMoraleMaxValue				= tpSE_Rat->fMoraleMaxValue;
	m_fMoraleNormalValue			= tpSE_Rat->fMoraleNormalValue;
	// attack
	m_fHitPower						= tpSE_Rat->fHitPower;
	m_dwHitInterval					= tpSE_Rat->u16HitInterval;
	m_fAttackDistance				= tpSE_Rat->fAttackDistance;
	m_fAttackAngle					= tpSE_Rat->fAttackAngle/180.f*PI;
	m_fAttackSuccessProbability		= tpSE_Rat->fAttackSuccessProbability;

//	m_tCurGP						= tpSE_Rat->m_tGraphID;
//	m_tNextGP						= tpSE_Rat->m_tNextGraphID;
	m_tCurGP						= m_tNextGP = game_vertex_id();

	int								iPointCount	= (int)vertex_types().size();
	for (int j=0; j<iPointCount; ++j)
		if (ai().game_graph().mask(vertex_types()[j].tMask,ai().game_graph().vertex(game_vertex_id())->vertex_type())) {
			m_dwTimeToChange	= Level().timeServer() + ::Random.randI(vertex_types()[j].dwMinTime,vertex_types()[j].dwMaxTime);
			break;
		}

	//////////////////////////////////////////////////////////////////////////

	m_fSpeed						= m_fCurSpeed = m_fMaxSpeed;

	m_tOldPosition.set				(Position());
	if (g_Alive())
		m_tSpawnPosition.set		(Level().seniority_holder().team(g_Team()).squad(g_Squad()).leader()->Position());
	else
		m_tSpawnPosition.set		(Position());
	m_tSafeSpawnPosition.set		(m_tSpawnPosition);
	m_tStateStack.push				(m_eCurrentState = aiRatFreeHuntingActive);
	if (g_Alive())
		vfAddActiveMember			(true);

	m_bStateChanged					= true;
	set_game_vertex					(ai().cross_table().vertex(level_vertex_id()).game_vertex_id());

	m_tHPB.x						= -m_body.current.yaw;
	m_tHPB.y						= -m_body.current.pitch;
	m_tHPB.z						= 0;

	enable_movement					(false);

	vfLoadAnimations				();

	if (g_Alive())
		Level().seniority_holder().team(g_Team()).squad(g_Squad()).group(g_Group()).m_dwLastActionTime = 0;

	return							(TRUE);
}

void CAI_Rat::net_Destroy()
{
	inherited::net_Destroy();
	CEatableItem::net_Destroy();
}

void CAI_Rat::net_Export(NET_Packet& P)
{
	R_ASSERT				(Local());

	// export last known packet
	R_ASSERT				(!NET.empty());
	net_update& N			= NET.back();
	P.w_float_q16			(fEntityHealth,-500,1000);
	P.w_u32					(N.dwTimeStamp);
	P.w_u8					(0);
	P.w_vec3				(N.p_pos);
	P.w_angle8				(N.o_model);
	P.w_angle8				(N.o_torso.yaw);
	P.w_angle8				(N.o_torso.pitch);
	P.w_u8					(u8(g_Team()));
	P.w_u8					(u8(g_Squad()));
	P.w_u8					(u8(g_Group()));

	ALife::_GRAPH_ID		l_game_vertex_id = game_vertex_id();
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	P.w						(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	float					f1 = 0;
	if (ai().game_graph().valid_vertex_id(l_game_vertex_id)) {
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.w					(&f1,						sizeof(f1));
		f1					= Position().distance_to	(ai().game_graph().vertex(l_game_vertex_id)->level_point());
		P.w					(&f1,						sizeof(f1));
	}
	else {
		P.w					(&f1,						sizeof(f1));
		P.w					(&f1,						sizeof(f1));
	}

	CEatableItem::net_Export(P);
}

void CAI_Rat::net_Import(NET_Packet& P)
{
	R_ASSERT				(Remote());
	net_update				N;

	u8 flags;

	float health;
	P.r_float_q16		(health,-500,1000);
	fEntityHealth = health;	
	
	P.r_u32					(N.dwTimeStamp);
	P.r_u8					(flags);
	P.r_vec3				(N.p_pos);
	P.r_angle8				(N.o_model);
	P.r_angle8				(N.o_torso.yaw);
	P.r_angle8				(N.o_torso.pitch);
	id_Team					= P.r_u8();
	id_Squad				= P.r_u8();
	id_Group				= P.r_u8();

	ALife::_GRAPH_ID		t;
	P.r						(&t,				sizeof(t));
	P.r						(&t,				sizeof(t));
	set_game_vertex			(t);

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back			(N);
		NET_WasInterpolating	= TRUE;
	}

	setVisible				(TRUE);
	setEnabled				(TRUE);

	CEatableItem::net_Import(P);
}

void CAI_Rat::CreateSkeleton(){

	if (!Visual())
		return;
	CPhysicsElement* element=P_create_Element();
	Fobb box;
	box.m_rotate.identity();
	box.m_translate.set(0,0.1f,-0.15f);
	box.m_halfsize.set(0.10f,0.085f,0.25f);
	element->add_Box(box);
	//Fsphere sphere;
	//sphere.P.set(0,0,0);
	//sphere.R=0.25;
	//element->add_Sphere(sphere);
	element->setDensity(m_phMass);
	element->SetMaterial(smart_cast<CKinematics*>(Visual())->LL_GetData(smart_cast<CKinematics*>(Visual())->LL_GetBoneRoot()).game_mtl_idx);
	m_pPhysicsShell=P_create_Shell();
	m_pPhysicsShell->add_Element(element);
	m_pPhysicsShell->Activate(XFORM(),0,XFORM());
	if(!fsimilar(0.f,m_saved_impulse)){

		m_pPhysicsShell->applyHit(m_saved_hit_position,m_saved_hit_dir,m_saved_impulse,0,m_saved_hit_type);
	}
	/*
	CKinematics* M		= smart_cast<CKinematics*>(Visual());			VERIFY(M);
	m_pPhysicsShell		= P_create_Shell();

	//get bone instance
	int id=M->LL_BoneID("bip01_pelvis");
	CBoneInstance& instance=M->LL_GetBoneInstance				(id);

	//create root element
	CPhysicsElement* element=P_create_Element				();
	element->mXFORM.identity();
	instance.set_callback(m_pPhysicsShell->GetBonesCallback(),element);
	Fobb box;
	box.m_rotate.identity();
	box.m_translate.set(0,0,0);
	box.m_halfsize.set(0.10f,0.085f,0.25f);
	element->add_Box(box);

	element->setDensity(200.f);
	m_pPhysicsShell->add_Element(element);
	element->SetMaterial("materials/skel1");

	//set shell start position
	Fmatrix m;
	m.set(mRotate);
	m.c.set(Position());
	m_pPhysicsShell->mXFORM.set(m);
	*/
}

void CAI_Rat::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);
}

void CAI_Rat::UpdateCL			()
{
	if (!Useful()) {
		inherited::UpdateCL		();
		Exec_Look				(Device.fTimeDelta);
	}
	else
		CEatableItem::UpdateCL	();

	if(!m_pPhysicsShell && !g_Alive())
		CreateSkeleton			();
}

void CAI_Rat::UpdatePositionAnimation()
{
	Fmatrix						l_tSavedTransform = XFORM();
	m_fTimeUpdateDelta			= Device.fTimeDelta;
	vfComputeNewPosition		(m_bCanAdjustSpeed,m_bStraightForward);

	float						y,p,b;
	XFORM().getHPB				(y,p,b);
	NET_Last.p_pos				= Position();
	NET_Last.o_model			= -y;
	NET_Last.o_torso.yaw		= -y;
	NET_Last.o_torso.pitch		= -p;
	XFORM()						= l_tSavedTransform;

	if (!bfScriptAnimation())
		SelectAnimation		(XFORM().k,Fvector().set(1,0,0),m_fSpeed);
}


void CAI_Rat::Hit(float P,Fvector &dir,CObject*who,s16 element,Fvector p_in_object_space,float impulse, ALife::EHitType hit_type /*= ALife::eHitTypeWound*/){
	inherited::Hit				(P,dir,who,element,p_in_object_space,impulse, hit_type);
	if (!m_pPhysicsShell) {
		m_saved_impulse			= impulse;
		m_saved_hit_dir.set		(dir);
		m_saved_hit_type		= hit_type;
		m_saved_hit_position.set(p_in_object_space);
	}
}

void CAI_Rat::feel_touch_new(CObject* O)
{
}

/////////////////////////////////////
// Rat as eatable item
/////////////////////////////////////
void CAI_Rat::OnH_A_Chield		()
{
	inherited::OnH_A_Chield		();
	CEatableItem::OnH_A_Chield	();
}

void CAI_Rat::OnH_B_Chield		()
{
	inherited::OnH_B_Chield		();
	CEatableItem::OnH_B_Chield	();
}

void CAI_Rat::OnH_B_Independent	()
{
	inherited::OnH_B_Independent	();
	CEatableItem::OnH_B_Independent	();
}

void CAI_Rat::OnH_A_Independent	()
{
	inherited::OnH_A_Independent	();
	CEatableItem::OnH_A_Independent	();
}

bool CAI_Rat::Useful() const
{
	if (!g_Alive())
	{
		return CEatableItem::Useful();
	}

	return false;
}

#ifdef DEBUG
void CAI_Rat::OnRender()
{
	inherited::OnRender();
	CEatableItem::OnRender();
}
#endif

BOOL CAI_Rat::UsedAI_Locations()
{
	return					(TRUE);
}

void CAI_Rat::make_Interpolation ()
{
	inherited::make_Interpolation();
	CEatableItem::make_Interpolation();
}

void CAI_Rat::PH_B_CrPr			()
{
	inherited::PH_B_CrPr		();
	CEatableItem::PH_B_CrPr		();
}

void CAI_Rat::PH_I_CrPr			()
{
	inherited::PH_I_CrPr		();
	CEatableItem::PH_I_CrPr		();
}

#ifdef DEBUG
void CAI_Rat::PH_Ch_CrPr		()
{
	inherited::PH_Ch_CrPr		();
	CEatableItem::PH_Ch_CrPr	();
}
#endif


void CAI_Rat::PH_A_CrPr			()
{
	inherited::PH_A_CrPr		();
	CEatableItem::PH_A_CrPr		();
}

void CAI_Rat::create_physic_shell()
{
	// do not delete!!!
}

void CAI_Rat::setup_physic_shell()
{
	// do not delete!!!
}

void CAI_Rat::activate_physic_shell	()
{
	CEatableItem::activate_physic_shell();
}

float CAI_Rat::get_custom_pitch_speed	(float def_speed)
{
	if (fsimilar(m_fSpeed,0.f))
		return				(PI_DIV_6);
	else
		if (fsimilar(m_fSpeed,m_fMinSpeed))
			return			(PI_DIV_4);
		else
			if (fsimilar(m_fSpeed,m_fMaxSpeed))
				return		(PI_DIV_3);
			else
				if (fsimilar(m_fSpeed,m_fAttackSpeed))
					return	(PI_DIV_2);

	Debug.fatal				("Impossible RAT speed!");
	return					(PI_DIV_2);
}

BOOL CAI_Rat::renderable_ShadowReceive	()
{
	return TRUE;
}

BOOL CAI_Rat::renderable_ShadowGenerate	()
{
	return FALSE;
}
