// Actor.cpp: implementation of the CActor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Actor_Flags.h"

#include "hudmanager.h"
#ifdef DEBUG
#include "ode_include.h"
#include "../StatGraph.h"
#include "PHDebug.h"
#endif
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "Car.h"
#include "xrserver_objects_alife_monsters.h"

#include "CameraLook.h"
#include "CameraFirstEye.h"


#include "effectorfall.h"
#include "EffectorBobbing.h"
#include "clsid_game.h"

#include "ActorEffector.h"
#include "EffectorZoomInertion.h"

#include "SleepEffector.h"

#include "character_info.h"
#include "CustomOutfit.h"

#include "actorcondition.h"
#include "UIGameCustom.h"


// breakpoints
#include "../xr_input.h"

//
#include "Actor.h"
#include "ActorAnimation.h"
#include "actor_anim_defs.h"

#include "HudItem.h"
#include "WeaponMagazined.h"

#include "ai_sounds.h"
#include "ai_space.h"

#include "trade.h"
#include "LevelFogOfWar.h"

#include "inventory.h"

#include "Physics.h"
#include "level.h"
#include "GamePersistent.h"
#include "game_cl_base.h"
#include "game_cl_single.h"
#include "xrmessages.h"

#include "string_table.h"
#include "usablescriptobject.h"
#include "../cl_intersect.h"
#include "ExtendedGeom.h"
#include "alife_registry_wrappers.h"
#include "../skeletonanimated.h"

#include "artifact.h"
#include "CharacterPhysicsSupport.h"

#include "material_manager.h"
#include "IColisiondamageInfo.h"
#include "ui/UIMainIngameWnd.h"
#include "ui/UIArtefactPanel.h"

#include "map_manager.h"
#include "GameTaskManager.h"
#include "EffectorShotX.h"

#include "actor_memory.h"

const u32		patch_frames	= 50;
const float		respawn_delay	= 1.f;
const float		respawn_auto	= 7.f;

static float IReceived = 0;
static float ICoincidenced = 0;


//skeleton



static Fbox		bbStandBox;
static Fbox		bbCrouchBox;
static Fvector	vFootCenter;
static Fvector	vFootExt;

Flags32			psActorFlags={0};



CActor::CActor() : CEntityAlive()
{

	encyclopedia_registry	= xr_new<CEncyclopediaRegistryWrapper	>();
	game_news_registry		= xr_new<CGameNewsRegistryWrapper		>();
	// Cameras
	cameras[eacFirstEye]	= xr_new<CCameraFirstEye>	(this, pSettings, "actor_firsteye_cam", 0);
	cameras[eacLookAt]		= xr_new<CCameraLook>		(this, pSettings, "actor_look_cam",		0);
	cameras[eacFreeLook]	= xr_new<CCameraLook>		(this, pSettings, "actor_free_cam",		0);
	cam_active				= eacFirstEye;
	fPrevCamPos				= 0.0f;
	vPrevCamDir.set			(0.f,0.f,1.f);
	fCurAVelocity			= 0.0f;
	// ���������
	pCamBobbing				= 0;
	m_pSleepEffector		= NULL;
	m_pSleepEffectorPP		= NULL;


	r_torso.yaw				= 0;
	r_torso.pitch			= 0;
	r_torso.roll			= 0;
	r_torso_tgt_roll		= 0;
	r_model_yaw				= 0;
	r_model_yaw_delta		= 0;
	r_model_yaw_dest		= 0;

	b_DropActivated			= 0;
	f_DropPower				= 0.f;

	m_fRunFactor			= 2.f;
	m_fCrouchFactor			= 0.2f;
	m_fClimbFactor			= 1.f;
	m_fCamHeightFactor		= 0.87f;

	m_fFallTime				=	s_fFallTime;
	m_bAnimTorsoPlayed		=	false;

	m_pPhysicsShell			=	NULL;



	m_holder				=	NULL;
	m_holderID				=	u16(-1);
	m_pPhysics_support		=	xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etActor,this);

#ifdef DEBUG
	Device.seqRender.Add	(this,REG_PRIORITY_LOW);
#endif

	//��������� ������������� ����� � inventory
	inventory().SetBeltUseful(true);

	m_pPersonWeLookingAt	= NULL;
	m_pVehicleWeLookingAt	= NULL;
	m_pObjectWeLookingAt	= NULL;
	m_bPickupMode			= false;

	pStatGraph				= NULL;

	m_pActorEffector		= NULL;

	m_bZoomAimingMode		= false;

	m_sDefaultObjAction		= NULL;

	m_fSprintFactor			= 4.f;

	hFriendlyIndicator.create(FVF::F_LIT,RCache.Vertex.Buffer(),RCache.QuadIB);

	m_pUsableObject			= NULL;


	m_anims					= xr_new<SActorMotions>();
	m_vehicle_anims			= xr_new<SActorVehicleAnims>();
	m_entity_condition		= NULL;
	m_iLastHitterID			= u16(-1);
	m_iLastHittingWeaponID	= u16(-1);
	m_game_task_manager		= NULL;
	m_statistic_manager		= NULL;
	//-----------------------------------------------------------------------------------
	m_memory				= xr_new<CActorMemory>(this);

	hit_probability			= 1.f;
}


CActor::~CActor()
{
	xr_delete				(m_memory);

	xr_delete				(encyclopedia_registry);
	xr_delete				(game_news_registry);
#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif
	//xr_delete(Weapons);
	for (int i=0; i<eacMaxCam; ++i) xr_delete(cameras[i]);

	m_HeavyBreathSnd.destroy();
	m_BloodSnd.destroy		();

	xr_delete				(m_pActorEffector);

	xr_delete				(m_pSleepEffector);

	hFriendlyIndicator.destroy();

	xr_delete				(m_pPhysics_support);

	xr_delete				(m_anims);
	xr_delete				(m_vehicle_anims);
}

void CActor::reinit	()
{
	m_PhysicMovementControl->CreateCharacter		();
	m_PhysicMovementControl->SetPhysicsRefObject	(this);
	
	CEntityAlive::reinit			();
	CInventoryOwner::reinit			();
	
	material().reinit				();
	m_pPhysics_support->in_Init		();
	m_pUsableObject					= NULL;
	memory().reinit					();
	
	set_input_external_handler		(0);
	m_time_lock_accel				= 0;
}

void CActor::reload	(LPCSTR section)
{
	CEntityAlive::reload	(section);
	CInventoryOwner::reload	(section);
	material().reload		(section);
	CStepManager::reload	(section);
	memory().reload			(section);
}

void CActor::Load	(LPCSTR section )
{
	// Msg					("Loading actor: %s",section);
	inherited::Load			(section);
	material().Load			(section);
	CInventoryOwner::Load	(section);
	memory().Load			(section);

	OnDifficultyChanged		();
	//////////////////////////////////////////////////////////////////////////
	ISpatial*		self			=	smart_cast<ISpatial*> (this);
	if (self)	{
		self->spatial.type	|=	STYPE_VISIBLEFORAI;
		self->spatial.type	&= ~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////////////////////////////////////////

	// m_PhysicMovementControl: General
	//m_PhysicMovementControl->SetParent		(this);
	Fbox	bb;Fvector	vBOX_center,vBOX_size;
	// m_PhysicMovementControl: BOX
	vBOX_center= pSettings->r_fvector3	(section,"ph_box2_center"	);
	vBOX_size	= pSettings->r_fvector3	(section,"ph_box2_size"		);
	bb.set	(vBOX_center,vBOX_center); bb.grow(vBOX_size);
	m_PhysicMovementControl->SetBox		(2,bb);

	// m_PhysicMovementControl: BOX
	vBOX_center= pSettings->r_fvector3	(section,"ph_box1_center"	);
	vBOX_size	= pSettings->r_fvector3	(section,"ph_box1_size"		);
	bb.set	(vBOX_center,vBOX_center); bb.grow(vBOX_size);
	m_PhysicMovementControl->SetBox		(1,bb);

	// m_PhysicMovementControl: BOX
	vBOX_center= pSettings->r_fvector3	(section,"ph_box0_center"	);
	vBOX_size	= pSettings->r_fvector3	(section,"ph_box0_size"		);
	bb.set	(vBOX_center,vBOX_center); bb.grow(vBOX_size);
	m_PhysicMovementControl->SetBox		(0,bb);

	//// m_PhysicMovementControl: Foots
	//Fvector	vFOOT_center= pSettings->r_fvector3	(section,"ph_foot_center"	);
	//Fvector	vFOOT_size	= pSettings->r_fvector3	(section,"ph_foot_size"		);
	//bb.set	(vFOOT_center,vFOOT_center); bb.grow(vFOOT_size);
	////m_PhysicMovementControl->SetFoots	(vFOOT_center,vFOOT_size);

	// m_PhysicMovementControl: Crash speed and mass
	float	cs_min		= pSettings->r_float	(section,"ph_crash_speed_min"	);
	float	cs_max		= pSettings->r_float	(section,"ph_crash_speed_max"	);
	float	mass		= pSettings->r_float	(section,"ph_mass"				);
	m_PhysicMovementControl->SetCrashSpeeds	(cs_min,cs_max);
	m_PhysicMovementControl->SetMass		(mass);
	if(pSettings->line_exist(section,"stalker_restrictor_radius"))
		m_PhysicMovementControl->SetActorRestrictorRadius(CPHCharacter::rtStalker,pSettings->r_float(section,"stalker_restrictor_radius"));
	if(pSettings->line_exist(section,"stalker_small_restrictor_radius"))
		m_PhysicMovementControl->SetActorRestrictorRadius(CPHCharacter::rtStalker,pSettings->r_float(section,"stalker_small_restrictor_radius"));
	if(pSettings->line_exist(section,"medium_monster_restrictor_radius"))
		m_PhysicMovementControl->SetActorRestrictorRadius(CPHCharacter::rtMonsterMedium,pSettings->r_float(section,"medium_monster_restrictor_radius"));
	m_PhysicMovementControl->Load(section);

	m_PhysicMovementControl->SetParent(this);

	m_fWalkAccel				= pSettings->r_float(section,"walk_accel");	
	m_fJumpSpeed				= pSettings->r_float(section,"jump_speed");
	m_fRunFactor				= pSettings->r_float(section,"run_coef");
	m_fRunBackFactor			= pSettings->r_float(section,"run_back_coef");
	m_fWalkBackFactor			= pSettings->r_float(section,"walk_back_coef");
	m_fCrouchFactor				= pSettings->r_float(section,"crouch_coef");
	m_fClimbFactor				= pSettings->r_float(section,"climb_coef");
	m_fSprintFactor				= pSettings->r_float(section,"sprint_koef");

	m_fWalk_StrafeFactor		= READ_IF_EXISTS(pSettings, r_float, section, "walk_strafe_coef", 1.0f);
	m_fRun_StrafeFactor			= READ_IF_EXISTS(pSettings, r_float, section, "run_strafe_coef", 1.0f);


	m_fCamHeightFactor			= pSettings->r_float(section,"camera_height_factor");
	m_PhysicMovementControl		->SetJumpUpVelocity(m_fJumpSpeed);
	float AirControlParam		= pSettings->r_float	(section,"air_control_param"	);
	m_PhysicMovementControl		->SetAirControlParam(AirControlParam);

	m_fPickupInfoRadius	= pSettings->r_float(section,"pickup_info_radius");
	m_fSleepTimeFactor	= pSettings->r_float(section,"sleep_time_factor");

	m_pPhysics_support->in_Load		(section);
	
	//��������� ��������� ���������
//	LoadShootingEffector	("shooting_effector");
	LoadSleepEffector		("sleep_effector");

	//��������� ��������� �������� firepoint
	m_vMissileOffset	= pSettings->r_fvector3(section,"missile_throw_offset");

	//Weapons				= xr_new<CWeaponList> (this);

	LPCSTR hit_snd_sect = pSettings->r_string(section,"hit_sounds");
	for(int hit_type=0;hit_type<(int)ALife::eHitTypeMax;++hit_type){
		LPCSTR hit_name = ALife::g_cafHitType2String((ALife::EHitType)hit_type);
		LPCSTR hit_snds = pSettings->r_string(hit_snd_sect, hit_name);
		int cnt = _GetItemCount(hit_snds);
		string128 tmp;
		for(int i=0; i<cnt;++i){
			sndHit[hit_type].push_back		(ref_sound());
			sndHit[hit_type].back().create	(TRUE,_GetItem(hit_snds,i,tmp));
		}
	}




	// sounds
	char buf[256];

/*	::Sound->create		(sndHit[0],			TRUE,	strconcat(buf,*cName(),"\\hurt1"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[1],			TRUE,	strconcat(buf,*cName(),"\\hurt2"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[2],			TRUE,	strconcat(buf,*cName(),"\\hurt3"),SOUND_TYPE_MONSTER_INJURING);
	::Sound->create		(sndHit[3],			TRUE,	strconcat(buf,*cName(),"\\hurt4"),SOUND_TYPE_MONSTER_INJURING);
*/	
	::Sound->create		(sndDie[0],			TRUE,	strconcat(buf,*cName(),"\\die0"), SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[1],			TRUE,	strconcat(buf,*cName(),"\\die1"), SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[2],			TRUE,	strconcat(buf,*cName(),"\\die2"), SOUND_TYPE_MONSTER_DYING);
	::Sound->create		(sndDie[3],			TRUE,	strconcat(buf,*cName(),"\\die3"), SOUND_TYPE_MONSTER_DYING);

	m_HeavyBreathSnd.create(TRUE, pSettings->r_string(section,"heavy_breath_snd"), SOUND_TYPE_MONSTER_INJURING);
	m_BloodSnd.create(TRUE, pSettings->r_string(section,"heavy_blood_snd"), SOUND_TYPE_MONSTER_INJURING);

	cam_Set					(eacFirstEye);

	// sheduler
	shedule.t_min				= shedule.t_max = 1;

	// ��������� ��������� ��������
	m_fDispBase					= pSettings->r_float		(section,"disp_base"		 );
	m_fDispBase					= deg2rad(m_fDispBase);

	m_fDispAim					= pSettings->r_float		(section,"disp_aim"		 );
	m_fDispAim					= deg2rad(m_fDispAim);

	m_fDispVelFactor			= pSettings->r_float		(section,"disp_vel_factor"	 );
	m_fDispAccelFactor			= pSettings->r_float		(section,"disp_accel_factor" );
	m_fDispCrouchFactor			= pSettings->r_float		(section,"disp_crouch_factor");
	m_fDispCrouchNoAccelFactor	= pSettings->r_float		(section,"disp_crouch_no_acc_factor");

	LPCSTR							default_outfit = READ_IF_EXISTS(pSettings,r_string,section,"default_outfit",0);
	SetDefaultVisualOutfit			(default_outfit);

	invincibility_fire_shield_1st	= READ_IF_EXISTS(pSettings,r_string,section,"Invincibility_Shield_1st",0);
	invincibility_fire_shield_3rd	= READ_IF_EXISTS(pSettings,r_string,section,"Invincibility_Shield_3rd",0);
//-----------------------------------------
	m_AutoPickUp_AABB				= READ_IF_EXISTS(pSettings,r_fvector3,section,"AutoPickUp_AABB",Fvector().set(0.02f, 0.02f, 0.02f));
	m_AutoPickUp_AABB_Offset		= READ_IF_EXISTS(pSettings,r_fvector3,section,"AutoPickUp_AABB_offs",Fvector().set(0, 0, 0));

	CStringTable string_table;
	m_sCharacterUseAction			= string_table.translate("character_use");
	m_sDeadCharacterUseAction		= string_table.translate("dead_character_use");
	m_sCarCharacterUseAction		= string_table.translate("car_character_use");
	m_sInventoryItemUseAction		= string_table.translate("inventory_item_use");
	//---------------------------------------------------------------------
	m_sHeadShotParticle	= READ_IF_EXISTS(pSettings,r_string,section,"HeadShotParticle",0);

}

void CActor::PHHit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type /* = ALife::eHitTypeWound */)
{
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse,hit_type,!g_Alive());
}

//void CActor::Hit		(float iLost, Fvector &dir, CObject* who, s16 element,Fvector position_in_bone_space, float impulse, ALife::EHitType hit_type, float AP)
void	CActor::Hit							(SHit* pHDS)
{
	SHit HDS = *pHDS;	

	bool bPlaySound = true;
	if (!g_Alive()) bPlaySound = false;

	if (GameID() != GAME_SINGLE)
	{
		game_PlayerState* ps = Game().GetPlayerByGameID(ID());
		if (ps && ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE))
		{
			bPlaySound = false;
			if (Device.dwFrame != last_hit_frame &&
				HDS.bone() != BI_NONE)
			{		
				// ��������� ������� � �������������� ��������
				Fmatrix pos; 

				CParticlesPlayer::MakeXFORM(this,HDS.bone(),HDS.dir,HDS.p_in_bone_space,pos);

				// ���������� particles
				CParticlesObject* ps = NULL;

				if (eacFirstEye == cam_active && this == Level().CurrentEntity())
					ps = CParticlesObject::Create(invincibility_fire_shield_1st,TRUE);
				else
					ps = CParticlesObject::Create(invincibility_fire_shield_3rd,TRUE);

				ps->UpdateParent(pos,Fvector().set(0.f,0.f,0.f));
				GamePersistent().ps_needtoplay.push_back(ps);
			};
		};
		 

		last_hit_frame = Device.dwFrame;
	};

	if( !sndHit[HDS.hit_type].empty() && (ALife::eHitTypeTelepatic != HDS.hit_type)){
		ref_sound& S = sndHit[HDS.hit_type][Random.randI(sndHit[HDS.hit_type].size())];

		if(ALife::eHitTypeExplosion == HDS.hit_type)
		{
			if (this == Level().CurrentControlEntity())
			{
				S.set_volume(10.0f);
				if(!m_sndShockEffector){
					m_sndShockEffector = xr_new<SndShockEffector>();
					m_sndShockEffector->Start(this, float(S._handle()->length_ms()), HDS.damage() );
				}
			}
			else
				bPlaySound = false;
		}
		if (bPlaySound && !S._feedback()) 
		{
			Fvector point		= Position();
			point.y				+= CameraHeight();
			S.play_at_pos		(this, point);
		};
	}

	
	//slow actor, only when he gets hit
	if(HDS.hit_type == ALife::eHitTypeWound || HDS.hit_type == ALife::eHitTypeStrike)
	{
		hit_slowmo				= HDS.damage();
		clamp					(hit_slowmo,0.0f,1.f);
	}
	else
		hit_slowmo = 0.f;
	//---------------------------------------------------------------
	if (Level().CurrentViewEntity() == this){
		CObject* pLastHitter = Level().Objects.net_Find(m_iLastHitterID);
		CObject* pLastHittingWeapon = Level().Objects.net_Find(m_iLastHittingWeaponID);
		HitSector(pLastHitter, pLastHittingWeapon);
	};

	if ((mstate_real&mcSprint) && Level().CurrentControlEntity() == this && 
		HDS.hit_type != ALife::eHitTypeTelepatic &&
		HDS.hit_type != ALife::eHitTypeRadiation 
		)
	{
//		mstate_real	&=~mcSprint;
		mstate_wishful	&=~mcSprint;
	};
	//---------------------------------------------------------------
	HitMark			(HDS.damage(), HDS.dir, HDS.who, HDS.bone(), HDS.p_in_bone_space, HDS.impulse, HDS.hit_type);
	//---------------------------------------------------------------
	switch (GameID())
	{
	case GAME_SINGLE:		
		{
			float hit_power	= HitArtefactsOnBelt(HDS.damage(), HDS.hit_type);

			if (GodMode())//psActorFlags.test(AF_GODMODE))
			{
				HDS.power = 0.0f;
//				inherited::Hit(0.f,dir,who,element,position_in_bone_space,impulse, hit_type);
				inherited::Hit(&HDS);
				return;
			}
			else 
			{
				//inherited::Hit		(hit_power,dir,who,element,position_in_bone_space, impulse, hit_type);
				HDS.power = hit_power;
				inherited::Hit(&HDS);
			};
		}
		break;
	default:
		{
			m_bWasBackStabbed = false;
			if (HDS.hit_type == ALife::eHitTypeWound_2 && Check_for_BackStab_Bone(HDS.bone()))
			{
				// convert impulse into local coordinate system
				Fmatrix					mInvXForm;
				mInvXForm.invert		(XFORM());
				Fvector					vLocalDir;
				mInvXForm.transform_dir	(vLocalDir,HDS.dir);
				vLocalDir.invert		();

				Fvector a	= {0,0,1};
				float res = a.dotproduct(vLocalDir);
				if (res < -0.707)
				{
					game_PlayerState* ps = Game().GetPlayerByGameID(ID());
					if (!ps || !ps->testFlag(GAME_PLAYER_FLAG_INVINCIBLE))						
						m_bWasBackStabbed = true;
				}
			};
			
			float hit_power = 0;

			if (m_bWasBackStabbed) hit_power = 100000;
			else hit_power	= HitArtefactsOnBelt(HDS.damage(), HDS.hit_type);

			HDS.power = hit_power;
			inherited::Hit (&HDS);
			//inherited::Hit	(hit_power,dir,who,element,position_in_bone_space, impulse, hit_type, 0.0f);
		}		
		break;
	}
}

void CActor::HitMark	(float P, 
						 Fvector dir,			
						 CObject* who, 
						 s16 element, 
						 Fvector position_in_bone_space, 
						 float impulse,  
						 ALife::EHitType hit_type)
{
	// hit marker
	if ( (hit_type==ALife::eHitTypeFireWound||hit_type==ALife::eHitTypeWound_2) && g_Alive() && Local() && /*(this!=who) && */(Level().CurrentEntity()==this) )	
	{
		HUD().Hit(0, P, dir);

	if(1||psHUD_Flags.test(HUD_CAM_ANIM_HIT)){
		CEffectorCam* ce = Cameras().GetCamEffector((ECamEffectorType)effFireHit);
		if(!ce)
			{
			int id						= -1;
			Fvector						cam_pos,cam_dir,cam_norm;
			cam_Active()->Get			(cam_pos,cam_dir,cam_norm);
			cam_dir.normalize_safe		();
			dir.normalize_safe			();

			float ang_diff				= angle_difference	(cam_dir.getH(), dir.getH());
			Fvector						cp;
			cp.crossproduct				(cam_dir,dir);
			bool bUp					=(cp.y>0.0f);

			Fvector cross;
			cross.crossproduct			(cam_dir, dir);
			VERIFY						(ang_diff>=0.0f && ang_diff<=PI);

			float _s1 = PI_DIV_8;
			float _s2 = _s1+PI_DIV_4;
			float _s3 = _s2+PI_DIV_4;
			float _s4 = _s3+PI_DIV_4;

			if(ang_diff<=_s1){
				id = 2;
			}else
			if(ang_diff>_s1 && ang_diff<=_s2){
				id = (bUp)?5:7;
			}else
			if(ang_diff>_s2 && ang_diff<=_s3){
				id = (bUp)?3:1;
			}else
			if(ang_diff>_s3 && ang_diff<=_s4){
				id = (bUp)?4:6;
			}else
			if(ang_diff>_s4){
				id = 0;
			}else{
				VERIFY(0);
			}

			string64 sect_name;
			sprintf(sect_name,"effector_fire_hit_%d",id);
			AddEffector(this, effFireHit, sect_name, P/100.0f);
			}
		}
	}

}

void CActor::HitSignal(float perc, Fvector& vLocalDir, CObject* who, s16 element)
{
	if (g_Alive()) 
	{

		// stop-motion
		if (m_PhysicMovementControl->Environment()==CPHMovementControl::peOnGround || m_PhysicMovementControl->Environment()==CPHMovementControl::peAtWall)
		{
			Fvector zeroV;
			zeroV.set			(0,0,0);
			m_PhysicMovementControl->SetVelocity(zeroV);
		}
		
		// check damage bone
		Fvector D;
		XFORM().transform_dir(D,vLocalDir);

		float	yaw, pitch;
		D.getHP(yaw,pitch);
		CKinematicsAnimated *tpKinematics = smart_cast<CKinematicsAnimated*>(Visual());
		VERIFY(tpKinematics);
#pragma todo("Dima to Dima : forward-back bone impulse direction has been determined incorrectly!")
		MotionID motion_ID = m_anims->m_normal.m_damage[iFloor(tpKinematics->LL_GetBoneInstance(element).get_param(1) + (angle_difference(r_model_yaw + r_model_yaw_delta,yaw) <= PI_DIV_2 ? 0 : 1))];
		float power_factor = perc/100.f; clamp(power_factor,0.f,1.f);
		VERIFY(motion_ID.valid());
		tpKinematics->PlayFX(motion_ID,power_factor);
	}
}

void CActor::Die	(CObject* who)
{
	inherited::Die(who);
	//-------------------------------------
	if (OnServer())
	{	
		xr_vector<CInventorySlot>::iterator I = inventory().m_slots.begin(), B = I;
		xr_vector<CInventorySlot>::iterator E = inventory().m_slots.end();
		for ( ; I != E; ++I)
		{
			if ((I - B) == (int)inventory().GetActiveSlot()) 
			{
				if((*I).m_pIItem)
				{
					if (OnServer())
					{
						if (GameID() == GAME_SINGLE)
							(*I).m_pIItem->Drop();
						else
						{
							NET_Packet P;
							(*I).m_pIItem->object().u_EventGen(P, GE_OWNERSHIP_REJECT, 
								(*I).m_pIItem->object().H_Parent()->ID());
							P.w_u16(u16((*I).m_pIItem->object().ID()));
							(*I).m_pIItem->object().u_EventSend(P);
						}
					}
				};
			}
			else
			{
				CCustomOutfit *pOutfit = smart_cast<CCustomOutfit *> ((*I).m_pIItem);
				if (pOutfit) continue;

				if((*I).m_pIItem) inventory().Ruck((*I).m_pIItem);
			};
		};


		///!!! ������ �����
		TIItemContainer &l_blist = inventory().m_belt;
		while (!l_blist.empty())	inventory().Ruck(l_blist.front());

		if (OnServer() && GameID() != GAME_SINGLE)
		{
			//		bool MedKitDropped = false;
			//if we are on server and actor has PDA - destroy PDA
			TIItemContainer &l_rlist = inventory().m_ruck;
			for(TIItemContainer::iterator l_it = l_rlist.begin(); l_rlist.end() != l_it; ++l_it)
			{
				if (GameID() == GAME_ARTEFACTHUNT)
				{
					CArtefact* pArtefact = smart_cast<CArtefact*> (*l_it);
					if (pArtefact)
					{
						(*l_it)->Drop();
						continue;
					};
				};

				if ((*l_it)->object().CLS_ID == CLSID_OBJECT_PLAYERS_BAG)
				{
					(*l_it)->Drop();
					continue;
				};
				/*
				if ((*l_it)->object().CLS_ID == CLSID_IITEM_MEDKIT && !MedKitDropped)
				{
				MedKitDropped = true;
				(*l_it)->Drop();
				continue;
				};

				if ((*l_it)->object().CLS_ID == CLSID_DEVICE_PDA)
				{
				(*l_it)->Drop();
				continue;
				};

				CCustomOutfit *pOutfit = smart_cast<CCustomOutfit *> (*l_it);
				if (pOutfit) continue;

				//���� � ��� ������ ���������� �����, ������� ��� ������� �� ���������
				(*l_it)->object().DestroyObject();
				*/
			};
		};
	};
	//-------------------------------------
	// Play ref_sound
	::Sound->play_at_pos	(sndDie[Random.randI(SND_DIE_COUNT)],this,Position());
	cam_Set					(eacFreeLook);

	mstate_wishful	&=		~mcAnyMove;
	mstate_real		&=		~mcAnyMove;


	//���������� ���� �������� �������
	m_HeavyBreathSnd.stop	();
	m_BloodSnd.stop			();		

	xr_delete				(m_sndShockEffector);
	if(IsGameTypeSingle()){
		HUD().GetUI()->UIGame()->AddCustomStatic("game_over", true);
	}
}

void CActor::g_Physics			(Fvector& _accel, float jump, float dt)
{	
	// Correct accel
	Fvector		accel;
	accel.set					(_accel);
	hit_slowmo					-=	dt;
	if (hit_slowmo<0)			hit_slowmo = 0.f;

	accel.mul					(1.f-hit_slowmo);
	
	if(g_Alive())
	{
	if(mstate_real&mcClimb&&!cameras[eacFirstEye]->bClampYaw)accel.set(0.f,0.f,0.f);
	m_PhysicMovementControl->Calculate			(accel,cameras[cam_active]->vDirection,0,jump,dt,false);
	m_PhysicMovementControl->GetPosition		(Position());
	m_PhysicMovementControl->bSleep				=false;
	}

	if (Local() && g_Alive()) {
		if (m_PhysicMovementControl->gcontact_Was)
			Cameras().AddCamEffector		(xr_new<CEffectorFall> (m_PhysicMovementControl->gcontact_Power));
		if (!fis_zero(m_PhysicMovementControl->gcontact_HealthLost))	{
			const ICollisionDamageInfo* di=m_PhysicMovementControl->CollisionDamageInfo();
			Fvector hdir;di->HitDir(hdir);
			SetHitInfo(this, NULL, 0, Fvector().set(0, 0, 0), hdir);
			//				Hit	(m_PhysicMovementControl->gcontact_HealthLost,hdir,di->DamageInitiator(),m_PhysicMovementControl->ContactBone(),di->HitPos(),0.f,ALife::eHitTypeStrike);//s16(6 + 2*::Random.randI(0,2))
			if (Level().CurrentControlEntity() == this)
			{
				SHit HDS = SHit(m_PhysicMovementControl->gcontact_HealthLost,hdir,di->DamageInitiator(),m_PhysicMovementControl->ContactBone(),di->HitPos(),0.f,di->HitType());
//				Hit(&HDS);

				NET_Packet	l_P;
				HDS.GenHeader(GE_HIT, ID());
				HDS.whoID = di->DamageInitiator()->ID();
				HDS.weaponID = di->DamageInitiator()->ID();
				HDS.Write_Packet(l_P);

				u_EventSend	(l_P);
			}
		}
	}
}

float CActor::currentFOV()
{
	CWeapon* pWeapon = smart_cast<CWeapon*>(inventory().ActiveItem());	

	if (eacFirstEye == cam_active && pWeapon &&
		pWeapon->IsZoomed() && (!pWeapon->ZoomTexture() ||
		(!pWeapon->IsRotatingToZoom() && pWeapon->ZoomTexture())))
		return pWeapon->GetZoomFactor();
	else
		return DEFAULT_FOV;
}

BOOL	g_bEnableMPL	= FALSE;	//.
void CActor::UpdateCL	()
{
	if(m_holder)
		m_holder->UpdateEx( currentFOV() );

	m_snd_noise -= 0.3f*Device.fTimeDelta;
	//clamp(m_snd_noise,0.0f,4.f);

	VERIFY2								(_valid(renderable.xform),*cName());
	inherited::UpdateCL();
	VERIFY2								(_valid(renderable.xform),*cName());
	m_pPhysics_support->in_UpdateCL	();
	VERIFY2								(_valid(renderable.xform),*cName());

	if (g_Alive()) 
		PickupModeUpdate	();	

	PickupModeUpdate_COD();
	//-------------------------------------------------------------------
//*
	m_bZoomAimingMode = false;
	CWeapon* pWeapon = smart_cast<CWeapon*>(inventory().ActiveItem());	
/*
	//�������� ��������� ������ � FOV 
	float dt = float(Device.dwTimeDelta)/1000.0f;

	if (eacFirstEye == cam_active && pWeapon &&
		pWeapon->IsZoomed() && (!pWeapon->ZoomTexture() ||
		(!pWeapon->IsRotatingToZoom() && pWeapon->ZoomTexture())))
		cam_Update(dt, pWeapon->GetZoomFactor());
	
	else 
		cam_Update(dt, DEFAULT_FOV);
*/
	Device.Statistic->TEST1.Begin		();
	cam_Update(float(Device.dwTimeDelta)/1000.0f, currentFOV());
	Device.Statistic->TEST1.End		();

	if(pWeapon)
	{
		if(pWeapon->IsZoomed()/* && !pWeapon->IsRotatingToZoom()*/)
		{
			float full_fire_disp = pWeapon->GetFireDispersion(true);
//			Msg("full_fire_disp  - %f", full_fire_disp );

			CEffectorZoomInertion* S = smart_cast<CEffectorZoomInertion*>	(Cameras().GetCamEffector(eCEZoom));
			if(S) S->SetParams(full_fire_disp);

			//�������, ��� ���� m_bZoomAimingMode = true
			//pWeapon->GetFireDispersion() ������ �������� ��������� ���
			//����� ��������� �������, ��� ��� �� ���������� � ��� GetWeaponAccuracy
			m_bZoomAimingMode = true;
		}

		//if(eacFirstEye == cam_active)
//		if(this == smart_cast<CActor*>(Level().CurrentEntity()))
		if(Level().CurrentEntity() && this->ID()==Level().CurrentEntity()->ID() )
		{
			float fire_disp = pWeapon->GetFireDispersion(true);
//			Msg("fire_disp - %f", fire_disp );
			HUD().SetCrosshairDisp(fire_disp);
			HUD().ShowCrosshair(true);
		}
	}
	else
	{
//		if(this == smart_cast<CActor*>(Level().CurrentEntity()))
		if(Level().CurrentEntity() && this->ID()==Level().CurrentEntity()->ID() )
		{
			HUD().SetCrosshairDisp(0.f);
			HUD().ShowCrosshair(false);
		}
	}

	UpdateDefferedMessages();
//*/
	//-------------------------------------------------------------------

	if (g_Alive()) 
		CStepManager::update();

	spatial.type |=STYPE_REACTTOSOUND;

	if(m_sndShockEffector)
	{
		if (this == Level().CurrentViewEntity())
		{
			m_sndShockEffector->Update();

			if(!m_sndShockEffector->InWork())
				xr_delete(m_sndShockEffector);
		}
		else
			xr_delete(m_sndShockEffector);
	}
	//-------------------------------------------
}

void CActor::shedule_Update	(u32 DT)
{
	setSVU(OnServer());

	//���������� ����� ������ HUD ��� �������� ��������� �����
	CHudItem* pHudItem = smart_cast<CHudItem*>(inventory().ActiveItem());	
	if(pHudItem && !pHudItem->object().getDestroy()) 
		pHudItem->SetHUDmode(HUDview());

	//���������� ���������
	UpdateInventoryOwner			(DT);
	if (GameID() == GAME_SINGLE)
		GameTaskManager().UpdateTasks	();

	if(m_holder || !getEnabled() || !Ready())
	{
		m_sDefaultObjAction				= NULL;
		inherited::shedule_Update		(DT);
		if (OnServer())
		{
			Check_Weapon_ShowHideState();
		};	
		return;
	}

	// 
	clamp					(DT,0u,100u);
	float	dt	 			=  float(DT)/1000.f;

	// Check controls, create accel, prelimitary setup "mstate_real"
	float	Jump	= 0;
	//----------- for E3 -----------------------------
//	if (Local() && (OnClient() || Level().CurrentEntity()==this))
	if (Level().CurrentControlEntity() == this)
	//------------------------------------------------
	{
		g_cl_CheckControls		(mstate_wishful,NET_SavedAccel,Jump,dt);
		g_cl_Orientate			(mstate_real,dt);
		g_Orientate				(mstate_real,dt);

		g_Physics				(NET_SavedAccel,Jump,dt);
		
		g_cl_ValidateMState		(dt,mstate_wishful);
		g_SetAnimation			(mstate_real);
		UpdateMotionIcon		(mstate_real);

		// Check for game-contacts
		Fvector C; float R;		
		//m_PhysicMovementControl->GetBoundingSphere	(C,R);
		
		Center(C);
		R=Radius();
		feel_touch_update		(C,R);

		// Dropping
		if (b_DropActivated)	{
			f_DropPower			+= dt*0.1f;
			clamp				(f_DropPower,0.f,1.f);
		} else {
			f_DropPower			= 0.f;
		}
	}
	else 
	{
		if (NET.size())
		{
			NET_SavedAccel = NET_Last.p_accel;
			mstate_real = mstate_wishful = NET_Last.mstate;

			g_sv_Orientate				(NET_Last.mstate,dt			);
			g_Orientate					(NET_Last.mstate,dt			);
			g_Physics					(NET_Last.p_accel,Jump,dt	);
			g_cl_ValidateMState			(dt,mstate_wishful);
			g_SetAnimation				(mstate_real);

			if (NET_Last.mstate & mcCrouch)
			{
				if (isActorAccelerated(mstate_real, IsZoomAimingMode()))
					m_PhysicMovementControl->ActivateBox(1, true);
				else
					m_PhysicMovementControl->ActivateBox(2, true);
			}
			else 
				m_PhysicMovementControl->ActivateBox(0, true);
		};
	}
	make_Interpolation();

	//------------------------------------------------	


	// generic stuff
	inherited::shedule_Update	(DT);
	//------------------------------------------------
	if (OnServer())
	{
		Check_Weapon_ShowHideState();
	};	
	//------------------------------------------------
	//�������� ���������� ��� ������
	if (!pCamBobbing)
	{
		pCamBobbing = xr_new<CEffectorBobbing>	();
		Cameras().AddCamEffector			(pCamBobbing);
	}
	pCamBobbing->SetState						(mstate_real, conditions().IsLimping(), IsZoomAimingMode());

	//���� �������� ������� ��� �������� � ��������
	if(this==Level().CurrentControlEntity())
	{
		if(conditions().IsLimping() && g_Alive()){
			if(!m_HeavyBreathSnd._feedback()){
				m_HeavyBreathSnd.play_at_pos(this, Fvector().set(0,ACTOR_HEIGHT,0), sm_Looped | sm_2D);
			}else{
				m_HeavyBreathSnd.set_position(Fvector().set(0,ACTOR_HEIGHT,0));
			}
		}else if(m_HeavyBreathSnd._feedback()){
			m_HeavyBreathSnd.stop		();
		}

		float bs = conditions().BleedingSpeed();
		if(bs>0.6f)
		{
			Fvector snd_pos;
			snd_pos.set(0,ACTOR_HEIGHT,0);
			if(!m_BloodSnd._feedback())
				m_BloodSnd.play_at_pos(this, snd_pos, sm_Looped | sm_2D);
			else
				m_BloodSnd.set_position(snd_pos);

			float v = bs+0.25f;

			m_BloodSnd.set_volume	(v);
		}else{
			if(m_BloodSnd._feedback())
				m_BloodSnd.stop();
		}

	}
	
	//���� � ������ HUD, �� ���� ������ ������ �� ��������
	if(!character_physics_support()->IsRemoved())
										setVisible				(!HUDview	());
	//��� ����� ����� ����� �����
	collide::rq_result& RQ = HUD().GetCurrentRayQuery();
	

	if(!input_external_handler_installed() && RQ.O &&  RQ.range<inventory().GetTakeDist()) 
	{
		m_pObjectWeLookingAt			= smart_cast<CGameObject*>(RQ.O);
		
		CGameObject						*game_object = smart_cast<CGameObject*>(RQ.O);
		m_pUsableObject					= smart_cast<CUsableScriptObject*>(game_object);
		inventory().m_pTarget			= smart_cast<PIItem>(game_object);
		m_pPersonWeLookingAt			= smart_cast<CInventoryOwner*>(game_object);
		m_pVehicleWeLookingAt			= smart_cast<CHolderCustom*>(game_object);
		CEntityAlive* pEntityAlive		= smart_cast<CEntityAlive*>(game_object);
		
		if (GameID() == GAME_SINGLE )
		{
			if (m_pUsableObject && m_pUsableObject->tip_text())
			{
				m_sDefaultObjAction = CStringTable().translate( m_pUsableObject->tip_text() );
			}
			else
			{
				if (m_pPersonWeLookingAt && pEntityAlive->g_Alive())
					m_sDefaultObjAction = m_sCharacterUseAction;

				else if (pEntityAlive && !pEntityAlive->g_Alive())
					m_sDefaultObjAction = m_sDeadCharacterUseAction;

				else if (m_pVehicleWeLookingAt)
					m_sDefaultObjAction = m_sCarCharacterUseAction;

				else if (inventory().m_pTarget)
					m_sDefaultObjAction = m_sInventoryItemUseAction;
				else 
					m_sDefaultObjAction = NULL;
			}
		}
	}
	else 
	{
		inventory().m_pTarget	= NULL;
		m_pPersonWeLookingAt	= NULL;
		m_sDefaultObjAction		= NULL;
		m_pUsableObject			= NULL;
		m_pObjectWeLookingAt	= NULL;
		m_pVehicleWeLookingAt	= NULL;
	}

	//��� ������ ���
	UpdateSleep									();

	//��� ������ ����������, ����������� �� �����
	UpdateArtefactsOnBelt						();
	m_pPhysics_support->in_shedule_Update		(DT);
	Check_for_AutoPickUp						();
/*
	if (GameID() == GAME_SINGLE)
	{
		Fvector2 pos2d;
		pos2d.set(Position().x, Position().z);
		CLevelFogOfWar* F = Level().FogOfWarMngr().GetFogOfWar(Level().name());
		if(F)
			F->Open(pos2d);
	};
*/
};

void CActor::renderable_Render	()
{
	inherited::renderable_Render			();
	if (!HUDview())
		CInventoryOwner::renderable_Render	();
}

BOOL CActor::renderable_ShadowGenerate	() 
{
	if(m_holder)
		return FALSE;
	
	return inherited::renderable_ShadowGenerate();
}



void CActor::g_PerformDrop	( )
{
	b_DropActivated			= FALSE;

	PIItem pItem = inventory().ActiveItem();
	if (0==pItem) return;
	pItem->Drop();
}


extern	BOOL	g_ShowAnimationInfo		;
// HUD
void CActor::OnHUDDraw	(CCustomHUD* /**hud/**/)
{
	if(inventory().ActiveItem()  ) {
		inventory().ActiveItem()->renderable_Render();
	}

#if 0//ndef NDEBUG
	if (Level().CurrentControlEntity() == this && g_ShowAnimationInfo)
	{
		string128 buf;
		HUD().Font().pFontSmall->SetColor	(0xffffffff);
		HUD().Font().pFontSmall->OutSet		(170,530);
		HUD().Font().pFontSmall->OutNext	("Position:      [%3.2f, %3.2f, %3.2f]",VPUSH(Position()));
		HUD().Font().pFontSmall->OutNext	("Velocity:      [%3.2f, %3.2f, %3.2f]",VPUSH(m_PhysicMovementControl->GetVelocity()));
		HUD().Font().pFontSmall->OutNext	("Vel Magnitude: [%3.2f]",m_PhysicMovementControl->GetVelocityMagnitude());
		HUD().Font().pFontSmall->OutNext	("Vel Actual:    [%3.2f]",m_PhysicMovementControl->GetVelocityActual());
		switch (m_PhysicMovementControl->Environment())
		{
		case CPHMovementControl::peOnGround:	strcpy(buf,"ground");			break;
		case CPHMovementControl::peInAir:		strcpy(buf,"air");				break;
		case CPHMovementControl::peAtWall:		strcpy(buf,"wall");				break;
		}
		HUD().Font().pFontSmall->OutNext	(buf);

		if (IReceived != 0)
		{
			float Size = 0;
			Size = HUD().Font().pFontSmall->GetSize();
			HUD().Font().pFontSmall->SetSize(Size*2);
			HUD().Font().pFontSmall->SetColor	(0xffff0000);
			HUD().Font().pFontSmall->OutNext ("Input :		[%3.2f]", ICoincidenced/IReceived * 100.0f);
			HUD().Font().pFontSmall->SetSize(Size);
		};
	};
#endif
}

void CActor::RenderIndicator			(Fvector dpos, float r1, float r2, ref_shader IndShader)
{
	if (!g_Alive()) return;

	u32			dwOffset = 0,dwCount = 0;
	FVF::LIT* pv_start				= (FVF::LIT*)RCache.Vertex.Lock(4,hFriendlyIndicator->vb_stride,dwOffset);
	FVF::LIT* pv					= pv_start;
	// base rect

	CBoneInstance& BI = smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(u16(m_head));
	Fmatrix M;
	smart_cast<CKinematics*>(Visual())->CalculateBones	();
	M.mul						(XFORM(),BI.mTransform);

	Fvector pos = M.c; pos.add(dpos);
	const Fvector& T        = Device.vCameraTop;
	const Fvector& R        = Device.vCameraRight;
	Fvector Vr, Vt;
	Vr.x            = R.x*r1;
	Vr.y            = R.y*r1;
	Vr.z            = R.z*r1;
	Vt.x            = T.x*r2;
	Vt.y            = T.y*r2;
	Vt.z            = T.z*r2;

	Fvector         a,b,c,d;
	a.sub           (Vt,Vr);
	b.add           (Vt,Vr);
	c.invert        (a);
	d.invert        (b);
	pv->set         (d.x+pos.x,d.y+pos.y,d.z+pos.z, 0xffffffff, 0.f,1.f);        pv++;
	pv->set         (a.x+pos.x,a.y+pos.y,a.z+pos.z, 0xffffffff, 0.f,0.f);        pv++;
	pv->set         (c.x+pos.x,c.y+pos.y,c.z+pos.z, 0xffffffff, 1.f,1.f);        pv++;
	pv->set         (b.x+pos.x,b.y+pos.y,b.z+pos.z, 0xffffffff, 1.f,0.f);        pv++;
	// render	
	dwCount 				= u32(pv-pv_start);
	RCache.Vertex.Unlock	(dwCount,hFriendlyIndicator->vb_stride);

	RCache.set_xform_world		(Fidentity);
	RCache.set_Shader			(IndShader);
	RCache.set_Geometry			(hFriendlyIndicator);
	RCache.Render	   			(D3DPT_TRIANGLESTRIP,dwOffset,0, dwCount, 0, 2);
};

static float mid_size = 0.097f;
static float fontsize = 15.0f;
static float upsize	= 0.33f;
void CActor::RenderText				(LPCSTR Text, Fvector dpos, float* pdup, u32 color)
{
	if (!g_Alive()) return;
	
	CBoneInstance& BI = smart_cast<CKinematics*>(Visual())->LL_GetBoneInstance(u16(m_head));
	Fmatrix M;
	smart_cast<CKinematics*>(Visual())->CalculateBones	();
	M.mul						(XFORM(),BI.mTransform);
	//------------------------------------------------
	Fvector v0, v1;
	v0.set(M.c); v1.set(M.c);
	Fvector T        = Device.vCameraTop;
	v1.add(T);

	Fvector v0r, v1r;
	Device.mFullTransform.transform(v0r,v0);
	Device.mFullTransform.transform(v1r,v1);
	float size = v1r.distance_to(v0r);
	float OldFontSize = HUD().Font().pFontDI->GetSize		();	
	float delta_up = 0.0f;
	if (size < mid_size) delta_up = upsize;
	else delta_up = upsize*(mid_size/size);
	dpos.y += delta_up;
	if (size > mid_size) size = mid_size;
	float NewFontSize = size/mid_size * fontsize;
	//------------------------------------------------
	M.c.y += dpos.y;

	Fvector4 v_res;	
	Device.mFullTransform.transform(v_res,M.c);

	if (v_res.z < 0 || v_res.w < 0)	return;
	if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

	float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
	float y = (1.f - v_res.y)/2.f * (Device.dwHeight);

	HUD().Font().pFontDI->SetAligment	(CGameFont::alCenter);
	HUD().Font().pFontDI->SetColor		(color);
	HUD().Font().pFontDI->SetSize		(NewFontSize);
	HUD().Font().pFontDI->Out			(x,y,Text);
	//-------------------------------------------------
	HUD().Font().pFontDI->SetSize(OldFontSize);
	*pdup = delta_up;
};

void CActor::SetPhPosition(const Fmatrix &transform)
{
	if(!m_pPhysicsShell){ 
		m_PhysicMovementControl->SetPosition(transform.c);
	}
	//else m_phSkeleton->S
}

void CActor::ForceTransform(const Fmatrix& m)
{
	if(!g_Alive())				return;
	XFORM().set					(m);
	if(m_PhysicMovementControl->CharacterExist()) m_PhysicMovementControl->EnableCharacter	();
	m_PhysicMovementControl->SetPosition		(m.c);
	m_PhysicMovementControl->SetVelocity		(0,0,0);
}

ENGINE_API extern float		psHUD_FOV;
float CActor::Radius()const
{ 
	float R		= inherited::Radius();
	CWeapon* W	= smart_cast<CWeapon*>(inventory().ActiveItem());
	if (W) R	+= W->Radius();
	//	if (HUDview()) R *= 1.f/psHUD_FOV;
	return R;
}

bool		CActor::use_bolts				() const
{
	if (GameID() != GAME_SINGLE) return false;
	return CInventoryOwner::use_bolts();
};

INT		g_iCorpseRemove = 1;

bool  CActor::NeedToDestroyObject() const
{
	if(GameID() == GAME_SINGLE)
	{
		return false;
	}
	else 
	{
		if (g_Alive()) return false;
		if (g_iCorpseRemove == -1) return false;
		if (g_iCorpseRemove == 0 && m_bAllowDeathRemove) return true;
		if(TimePassedAfterDeath()>m_dwBodyRemoveTime && m_bAllowDeathRemove)
			return true;
		else
			return false;
	}
}

ALife::_TIME_ID	 CActor::TimePassedAfterDeath()	const
{
	if(!g_Alive())
		return Level().timeServer() - GetLevelDeathTime();
	else
		return 0;
}


void CActor::OnItemTake			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemTake(inventory_item);
	if (OnClient()) return;

///	if (GameID()==GAME_ARTEFACTHUNT)
//			SpawnAmmoForWeapon(inventory_item);
}

void CActor::OnItemDrop			(CInventoryItem *inventory_item)
{
	CInventoryOwner::OnItemDrop(inventory_item);
//	if (OnClient()) return;

	CArtefact* artefact = smart_cast<CArtefact*>(inventory_item);
	if(artefact && artefact->m_eItemPlace == eItemPlaceBelt)
		MoveArtefactBelt(artefact, false);
/*
	switch (GameID())
	{
	case GAME_DEATHMATCH:
	case GAME_TEAMDEATHMATCH:
	case GAME_ARTEFACTHUNT:
		{
			RemoveAmmoForWeapon(inventory_item);
		}break;
	};
*/	
}


void CActor::OnItemDropUpdate ()
{
	CInventoryOwner::OnItemDropUpdate ();

	//�� ��� ������ ���� ���� �������� �������������� ���� (���� ��������)
	//� � ��� ���� ��� ����
	TIItemContainer::iterator					I = inventory().m_all.begin();
	TIItemContainer::iterator					E = inventory().m_all.end();
	for ( ; I != E; ++I)
		if(!(*I)->GetDrop() && !attached(*I))
			attach(*I);
}

//////////////////////////////////////////////////////////////////////////


void CActor::OnItemRuck		(CInventoryItem *inventory_item, EItemPlace previous_place)
{
	CInventoryOwner::OnItemRuck(inventory_item, previous_place);

	CArtefact* artefact = smart_cast<CArtefact*>(inventory_item);
	if(artefact && previous_place == eItemPlaceBelt)
		MoveArtefactBelt(artefact, false);
}
void CActor::OnItemBelt		(CInventoryItem *inventory_item, EItemPlace previous_place)
{
	CInventoryOwner::OnItemBelt(inventory_item, previous_place);

	CArtefact* artefact = smart_cast<CArtefact*>(inventory_item);
	if(artefact)
		MoveArtefactBelt(artefact, true);
}


void CActor::MoveArtefactBelt(const CArtefact* artefact, bool on_belt)
{
	VERIFY(artefact);

	//�������� �������� �� ����
	if(on_belt)
	{
		VERIFY(m_ArtefactsOnBelt.end() == std::find(m_ArtefactsOnBelt.begin(), m_ArtefactsOnBelt.end(), artefact));
		m_ArtefactsOnBelt.push_back(artefact);
	}
	else
	{
		xr_vector<const CArtefact*>::iterator it = std::remove(m_ArtefactsOnBelt.begin(), m_ArtefactsOnBelt.end(), artefact);
		VERIFY(it != m_ArtefactsOnBelt.end());
		m_ArtefactsOnBelt.erase(it);
	}	
	if (Level().CurrentViewEntity() && Level().CurrentViewEntity() == this)
		HUD().GetUI()->UIMainIngameWnd->m_artefactPanel->InitIcons(m_ArtefactsOnBelt);
}
#define ARTEFACTS_UPDATE_TIME 0.100f
//old==1000

void CActor::UpdateArtefactsOnBelt()
{
	static float update_time = 0;

	float f_update_time = 0;

	if(update_time<ARTEFACTS_UPDATE_TIME)
	{
		update_time += conditions().fdelta_time();
		return;
	}
	else
	{
		f_update_time	= update_time;
		update_time		= 0.0f;
	}

	for(TIItemContainer::iterator it = inventory().m_belt.begin(); 
		inventory().m_belt.end() != it; ++it) 
	{
		CArtefact*	artefact = smart_cast<CArtefact*>(*it);
		if(artefact && artefact->m_bActorPropertiesEnabled)
		{
			conditions().ChangeBleeding			(artefact->m_fBleedingRestoreSpeed*f_update_time);
			conditions().ChangeHealth			(artefact->m_fHealthRestoreSpeed*f_update_time);
			conditions().ChangePower			(artefact->m_fPowerRestoreSpeed*f_update_time);
			conditions().ChangeSatiety			(artefact->m_fSatietyRestoreSpeed*f_update_time);
			conditions().ChangeRadiation		(artefact->m_fRadiationRestoreSpeed*f_update_time);
		}
	}
}

float	CActor::HitArtefactsOnBelt		(float hit_power, ALife::EHitType hit_type)
{
	for(TIItemContainer::iterator it = inventory().m_belt.begin(); 
		inventory().m_belt.end() != it; ++it) 
	{
		CArtefact*	artefact = smart_cast<CArtefact*>(*it);
		if(artefact && artefact->m_bActorPropertiesEnabled)
			hit_power = artefact->m_ArtefactHitImmunities.AffectHit(hit_power, hit_type);
	}

	return hit_power;
}


//////////////////////////////////////////////////////////////////////////

void	CActor::SpawnAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (OnClient()) return;
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

///	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(inventory().GetAny( *(pWM->m_ammoTypes[0]) ));
//	if (!pAmmo) 
		pWM->SpawnAmmo(0xffffffff, NULL, ID());
};

void	CActor::RemoveAmmoForWeapon	(CInventoryItem *pIItem)
{
	if (OnClient()) return;
	if (!pIItem) return;

	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pIItem);
	if (!pWM || !pWM->AutoSpawnAmmo()) return;

	CWeaponAmmo* pAmmo = smart_cast<CWeaponAmmo*>(inventory().GetAny(*(pWM->m_ammoTypes[0]) ));
	if (!pAmmo) return;
	//--- �� ����� ������� � �������� ������	
	/*
	//--- ��������� �� �������� �� ��� � ����-�� ���
	bool CanRemove = true;
	TIItemContainer::const_iterator I = inventory().m_all.begin();//, B = I;
	TIItemContainer::const_iterator E = inventory().m_all.end();
	for ( ; I != E; ++I)
	{
		CInventoryItem* pItem = (*I);//->m_pIItem;
		CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (pItem);
		if (!pWM || !pWM->AutoSpawnAmmo()) continue;
		if (pWM == pIItem) continue;
		if (pWM->m_ammoTypes[0] != pAmmo->CInventoryItem::object().cNameSect()) continue;
		CanRemove = false;
		break;
	};

	if (!CanRemove) return;
	*/
	pAmmo->DestroyObject();
//	NET_Packet			P;
//	u_EventGen			(P,GE_DESTROY,pAmmo->ID());
//	u_EventSend			(P);
};

void	CActor::SetZoomRndSeed		(s32 Seed)
{
	if (0 != Seed) m_ZoomRndSeed = Seed;
	else m_ZoomRndSeed = s32(Level().timeServer_Async());
};

void	CActor::SetShotRndSeed		(s32 Seed)
{
	if (0 != Seed) m_ShotRndSeed = Seed;
	else m_ShotRndSeed = s32(Level().timeServer_Async());
};

Fvector CActor::GetMissileOffset	() const
{
	return m_vMissileOffset;
}

void CActor::SetMissileOffset		(const Fvector &vNewOffset)
{
	m_vMissileOffset.set(vNewOffset);
}

void CActor::spawn_supplies			()
{
	inherited::spawn_supplies		();
	CInventoryOwner::spawn_supplies	();
}


void CActor::AnimTorsoPlayCallBack(CBlend* B)
{
	CActor* actor		= (CActor*)B->CallbackParam;
	actor->m_bAnimTorsoPlayed = FALSE;
}

void CActor::UpdateMotionIcon(u32 mstate_rl)
{
	CUIMotionIcon		&motion_icon=HUD().GetUI()->UIMainIngameWnd->MotionIcon();
	if(mstate_rl&mcClimb)
	{
		motion_icon.ShowState(CUIMotionIcon::stClimb);
	}
	else
	{
		if(mstate_rl&mcCrouch)
		{
			if (!isActorAccelerated(mstate_rl, IsZoomAimingMode()))
				motion_icon.ShowState(CUIMotionIcon::stCreep);
			else
				motion_icon.ShowState(CUIMotionIcon::stCrouch);
		}
		else
		if(mstate_rl&mcSprint)
				motion_icon.ShowState(CUIMotionIcon::stSprint);
		else
		if(mstate_rl&mcAnyMove && isActorAccelerated(mstate_rl, IsZoomAimingMode()))
			motion_icon.ShowState(CUIMotionIcon::stRun);
		else
			motion_icon.ShowState(CUIMotionIcon::stNormal);
	}

/*
						stNormal, --
						stCrouch, --
						stCreep,  --
						stClimb,  --
						stRun,    --
						stSprint, --
*/
}


CPHDestroyable*	CActor::ph_destroyable	()
{
	return smart_cast<CPHDestroyable*>(character_physics_support());
}

CEntityConditionSimple *CActor::create_entity_condition	(CEntityConditionSimple* ec)
{
	if(!ec)
		m_entity_condition		= xr_new<CActorCondition>(this);
	else
		m_entity_condition		= smart_cast<CActorCondition*>(ec);
	
	return		(inherited::create_entity_condition(m_entity_condition));
}

DLL_Pure *CActor::_construct			()
{
	CEntityAlive::_construct		();
	CInventoryOwner::_construct		();
	CStepManager::_construct		();

	return							(this);
}

bool CActor::use_center_to_aim			() const
{
	return							(!(mstate_real&mcCrouch));
}

// shot effector stuff
void CActor::update_camera (CCameraShotEffector* effector)
{
	if (!effector) return;
//	if (Level().CurrentViewEntity() != this) return;

	CCameraBase* pACam = cam_FirstEye();
	if (!pACam) return;

	if (pACam->bClampPitch)
	{
		while (pACam->pitch < pACam->lim_pitch[0])
			pACam->pitch += PI_MUL_2;
		while (pACam->pitch > pACam->lim_pitch[1])
			pACam->pitch -= PI_MUL_2;
	};

	effector->ApplyLastAngles(&(pACam->pitch), &(pACam->yaw));

	if (pACam->bClampYaw)	clamp(pACam->yaw,pACam->lim_yaw[0],pACam->lim_yaw[1]);
	if (pACam->bClampPitch)	clamp(pACam->pitch,pACam->lim_pitch[0],pACam->lim_pitch[1]);
}
void CActor::on_weapon_shot_start		(CWeapon *weapon)
{	
	CWeaponMagazined* pWM = smart_cast<CWeaponMagazined*> (weapon);
	//*
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>	(Cameras().GetCamEffector(eCEShot)); 
	if (!effector) {
		effector					= 
			(CCameraShotEffector*)Cameras().AddCamEffector(
				xr_new<CCameraShotEffector>(weapon->camMaxAngle,
											weapon->camRelaxSpeed,
											weapon->camMaxAngleHorz,
											weapon->camStepAngleHorz,
											weapon->camDispertionFrac)	);
	}
	R_ASSERT						(effector);

	if (pWM)
	{
		if (effector->IsSingleShot())
			update_camera(effector);

		if (pWM->GetCurrentFireMode() == 1)
		{
			effector->SetSingleShoot(TRUE);
		}
		else
		{
			effector->SetSingleShoot(FALSE);
		}
	};

	effector->SetRndSeed			(GetShotRndSeed());
	effector->SetActor				(this);
	effector->Shot					(weapon->camDispersion + weapon->camDispersionInc*float(weapon->ShotsFired()));
	
	if (pWM)
	{
		if (pWM->GetCurrentFireMode() != 1)
		{
			effector->SetActive(FALSE);
			update_camera(effector);
		}		
	}
}

void CActor::on_weapon_shot_stop		(CWeapon *weapon)
{
	//---------------------------------------------
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot)); 
	if (effector && effector->IsActive())
	{
		if (effector->IsSingleShot())
			update_camera(effector);
	}
	//---------------------------------------------
	Cameras().RemoveCamEffector(eCEShot);
}

void CActor::on_weapon_hide				(CWeapon *weapon)
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot)); 
	if (effector && !effector->IsActive())
		effector->Clear				();
}

Fvector CActor::weapon_recoil_delta_angle	()
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	Fvector							result = {0.f,0.f,0.f};

	if (effector)
		effector->GetDeltaAngle		(result);

	return							(result);
}

Fvector CActor::weapon_recoil_last_delta()
{
	CCameraShotEffector				*effector = smart_cast<CCameraShotEffector*>(Cameras().GetCamEffector(eCEShot));
	Fvector							result = {0.f,0.f,0.f};

	if (effector)
		effector->GetLastDelta		(result);

	return							(result);
}

bool CActor::can_attach			(const CInventoryItem *inventory_item) const
{
	const CAttachableItem	*item = smart_cast<const CAttachableItem*>(inventory_item);
	if (!item || /*!item->enabled() ||*/ !item->can_be_attached())
		return			(false);

	//����� �� ������������ ������� ������ ����
	if( m_attach_item_sections.end() == std::find(m_attach_item_sections.begin(),m_attach_item_sections.end(),inventory_item->object().cNameSect()) )
		return false;

	//���� ��� ���� �������������� ����� ������ ���� 
	if(attached(inventory_item->object().cNameSect()))
		return false;

	return true;
}

void CActor::OnDifficultyChanged	()
{
	// immunities
	VERIFY(g_SingleGameDifficulty>=egdNovice && g_SingleGameDifficulty<=egdMaster); 
	LPCSTR diff_name				= get_token_name(difficulty_type_token, g_SingleGameDifficulty);
	string128						tmp;
	strconcat						(tmp,"actor_immunities_",diff_name);
	conditions().LoadImmunities		(tmp,pSettings);
	// hit probability
	strconcat						(tmp,"hit_probability_",diff_name);
	hit_probability					= pSettings->r_float(*cNameSect(),tmp);
}

CVisualMemoryManager	*CActor::visual_memory	() const
{
	return							(&memory().visual());
}
