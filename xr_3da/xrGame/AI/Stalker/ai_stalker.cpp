////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_stalker.cpp
//	Created 	: 25.02.2003
//  Modified 	: 25.02.2003
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Stalker"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_stalker.h"
#include "../ai_monsters_misc.h"
#include "../../weapon.h"
#include "../../CharacterPhysicsSupport.h"
#include "../../pda.h"
#include "../../ai_script_actions.h"
#include "../../game_level_cross_table.h"
#include "../../game_graph.h"

CAI_Stalker::CAI_Stalker			()
{
	m_pPhysics_support				= xr_new<CCharacterPhysicsSupport>(CCharacterPhysicsSupport::EType::etStalker,this);
	Init							();
	m_PhysicMovementControl.AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
	shedule.t_min					= 200;
	shedule.t_max					= 1;
	m_dwParticularState				= u32(-1);
	InitTrade						();
	m_tpSelectorFreeHunting			= xr_new<PathManagers::CVertexEvaluator<aiSearchRange | aiEnemyDistance> >();
	m_tpSelectorReload				= xr_new<PathManagers::CVertexEvaluator<aiSearchRange | aiCoverFromEnemyWeight> >();
	m_tpSelectorCover				= xr_new<PathManagers::CVertexEvaluator<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance> >();
	m_tpSelectorRetreat				= xr_new<PathManagers::CVertexEvaluator<aiSearchRange | aiCoverFromEnemyWeight | aiEnemyDistance | aiEnemyViewDeviationWeight> >();
}

CAI_Stalker::~CAI_Stalker			()
{
	xr_delete						(m_pPhysics_support);
	xr_delete						(m_tpSelectorFreeHunting);
	xr_delete						(m_tpSelectorReload);
	xr_delete						(m_tpSelectorCover);
	xr_delete						(m_tpSelectorRetreat);
}

void CAI_Stalker::Init()
{
	inherited::Init					();
	m_pPhysics_support->in_Init		();
	m_tMovementDirection			= eMovementDirectionForward;
	m_tDesirableDirection			= eMovementDirectionForward;
	m_tLookType						= eLookTypePathDirection;
	m_tBodyState					= eBodyStateStand;
	m_dwDirectionStartTime			= 0;
	m_dwAnimationSwitchInterval		= 500;
	m_body.speed					= PI_MUL_2;
	m_head.speed					= 3*PI_DIV_2;

	m_hit_time						= 0;

	m_dwActionRefreshRate			= 1000;
	m_fAttackSuccessProbability		= .7f;
	m_dwNoFireTime					= 0;

	m_tSavedEnemy					= 0;
	m_tSavedEnemyPosition.set		(0,0,0);
	m_dwLostEnemyTime				= 0;
	m_tpSavedEnemyNode				= 0;
	m_dwSavedEnemyNodeID			= u32(-1);
	m_tpItemsToTake.clear			();
	m_dwItemToTakeIndex				= u32(-1);
	m_bActionStarted				= false;
	m_iSoundIndex					= -1;
	m_dwSoundTime					= 0;

	m_previous_query_time				= 0;
	m_dwRandomFactor				= 100;
	m_dwInertion					= 20000;

	m_tMovementType					= eMovementTypeStand;
//	m_tPathState					= ePathStateSearchNode;
//	m_tPathType						= ePathTypeDodge;
	m_tMentalState					= eMentalStateFree;

	m_dwActionStartTime				= 0;
	m_dwActionEndTime				= 0;
	m_bHammerIsClutched				= false;

	m_bIfSearchFailed				= false;
	m_bStateChanged					= true;

	_A=_B=_C=_D=_E=_F=_G=_H=_I=_J=_K=_L=_M=false;

	m_fAttackSuccessProbability0	= .8f;
	m_fAttackSuccessProbability1	= .6f;
	m_fAttackSuccessProbability2	= .4f;
	m_fAttackSuccessProbability3	= .2f;
	m_dwRandomState					= 2+0*::Random.randI(5);

//	m_fAccuracy						= 0.f;
//	m_fIntelligence					= 0.f;

	m_fTimeToStep					= 0;
	
	m_wMyMaterialID					= GMLib.GetMaterialIdx("creatures\\human");
	m_wLastMaterialID				= GMLib.GetMaterialIdx("default");

	m_dwLookChangedTime				= 0;

	m_fHitFactor					= 1.f;

	m_dwLastEnemySearch				= 0;
	m_dwLastSoundUpdate				= 0;

	m_tpCurrentSound				= 0;
	m_bPlayHumming					= false;

	m_dwLastUpdate					= 0;

	m_dwStartFireTime				= 0;

	m_tTaskState					= eTaskStateChooseTask;
	m_bCanFire						= true;

	m_movement_params.insert		(std::make_pair(eMovementParameterStand						,STravelParams(0.f									,PI_MUL_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkFree					,STravelParams(m_fWalkFreeFactor					,PI_DIV_8	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunFree					,STravelParams(m_fRunFreeFactor						,PI_DIV_8/2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerStand			,STravelParams(m_fWalkFactor						,PI			)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerCrouch			,STravelParams(m_fWalkFactor*m_fCrouchFactor		,3*PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerStand			,STravelParams(m_fRunFactor							,PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerCrouch			,STravelParams(m_fRunFactor*m_fCrouchFactor			,PI			)));
	m_movement_params.insert		(std::make_pair(eMovementParameterPanic						,STravelParams(m_fPanicFactor						,PI_DIV_8/2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkFreeDamaged			,STravelParams(m_fDamagedWalkFreeFactor				,PI_DIV_8	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunFreeDamaged			,STravelParams(m_fDamagedRunFreeFactor				,PI_DIV_8/2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerStandDamaged	,STravelParams(m_fDamagedWalkFactor					,PI			)));
	m_movement_params.insert		(std::make_pair(eMovementParameterWalkDangerCrouchDamaged	,STravelParams(m_fWalkFactor*m_fCrouchFactor		,3*PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerStandDamaged		,STravelParams(m_fDamagedRunFactor					,PI_DIV_2	)));
	m_movement_params.insert		(std::make_pair(eMovementParameterRunDangerCrouchDamaged	,STravelParams(m_fRunFactor*m_fCrouchFactor			,PI			)));
	m_movement_params.insert		(std::make_pair(eMovementParameterPanicDamaged				,STravelParams(m_fDamagedPanicFactor				,PI_DIV_8/2	)));
	set_velocity_mask				(eMovementParameterWalkFree);
	set_desirable_mask				(eMovementParameterWalkFree);
//	set_velocity_mask				(eMovementParameterWalkDangerStand);
//	set_desirable_mask				(eMovementParameterWalkDangerStand);
}

void CAI_Stalker::Die				()
{
	SelectAnimation					(XFORM().k,direction(),speed());
	m_dwDeathTime					= Level().timeServer();

	ref_sound						&S  = m_tpSoundDie[::Random.randI((u32)m_tpSoundDie.size())];
	S.play_at_pos					(this,Position());
	S.feedback->set_volume			(1.f);
	inherited::Die					();
	m_bHammerIsClutched				= !::Random.randI(0,2);

	//��������� ������������� ������ � ���������
	m_inventory.SetSlotsUseful		(false);
//#ifdef DEBUG
//	Msg								("Death position : [%f][%f][%f]",VPUSH(Position()));
//#endif
}

void CAI_Stalker::Load				(LPCSTR section)
{ 
	setEnabled						(false);
	inherited::Load					(section);
	m_tpSelectorFreeHunting->Load	(section,"selector_free_hunting");
	m_tpSelectorReload->Load		(section,"selector_reload");
	m_tpSelectorRetreat->Load		(section,"selector_retreat");
	m_tpSelectorCover->Load			(section,"selector_cover");
	
	// visibility
	m_dwMovementIdleTime			= pSettings->r_s32(section,"MovementIdleTime");
	m_fMaxInvisibleSpeed			= pSettings->r_float(section,"MaxInvisibleSpeed");
	m_fMaxViewableSpeed				= pSettings->r_float(section,"MaxViewableSpeed");
	m_fMovementSpeedWeight			= pSettings->r_float(section,"MovementSpeedWeight");
	m_fDistanceWeight				= pSettings->r_float(section,"DistanceWeight");
	m_fSpeedWeight					= pSettings->r_float(section,"SpeedWeight");
	m_fVisibilityThreshold			= pSettings->r_float(section,"VisibilityThreshold");
	m_fLateralMultiplier			= pSettings->r_float(section,"LateralMultiplier");
	m_fShadowWeight					= pSettings->r_float(section,"ShadowWeight");

	m_fCrouchFactor					= pSettings->r_float(section,"CrouchFactor");
	m_fWalkFactor					= pSettings->r_float(section,"WalkFactor");
	m_fRunFactor					= pSettings->r_float(section,"RunFactor");
	m_fWalkFreeFactor				= pSettings->r_float(section,"WalkFreeFactor");
	m_fRunFreeFactor				= pSettings->r_float(section,"RunFreeFactor");
	m_fPanicFactor					= pSettings->r_float(section,"PanicFactor");
	m_fDamagedWalkFactor			= pSettings->r_float(section,"DamagedWalkFactor");
	m_fDamagedRunFactor				= pSettings->r_float(section,"DamagedRunFactor");
	m_fDamagedWalkFreeFactor		= pSettings->r_float(section,"DamagedWalkFreeFactor");
	m_fDamagedRunFreeFactor			= pSettings->r_float(section,"DamagedRunFreeFactor");
	m_fDamagedPanicFactor			= pSettings->r_float(section,"DamagedPanicFactor");

	//fire
	m_dwFireRandomMin  				= pSettings->r_s32(section,"FireRandomMin");
	m_dwFireRandomMax  				= pSettings->r_s32(section,"FireRandomMax");
	m_dwNoFireTimeMin  				= pSettings->r_s32(section,"NoFireTimeMin");
	m_dwNoFireTimeMax  				= pSettings->r_s32(section,"NoFireTimeMax");
	m_fMinMissDistance 				= pSettings->r_float(section,"MinMissDistance");
	m_fMinMissFactor   				= pSettings->r_float(section,"MinMissFactor");
	m_fMaxMissDistance 				= pSettings->r_float(section,"MaxMissDistance");
	m_fMaxMissFactor				= pSettings->r_float(section,"MaxMissFactor");
	
	if (pSettings->line_exist(section,"State"))
		m_dwParticularState			= pSettings->r_u32(section,"State");

	eye_fov							= pSettings->r_float(section,"eye_fov");
	eye_range						= pSettings->r_float(section,"eye_range");

	m_tpaTerrain.clear				();
	LPCSTR							S = pSettings->r_string(section,"terrain");
	u32								N = _GetItemCount(S);
	R_ASSERT						(!(N % (LOCATION_TYPE_COUNT + 2)) && N);
	STerrainPlace					tTerrainPlace;
	tTerrainPlace.tMask.resize		(LOCATION_TYPE_COUNT);
	m_tpaTerrain.reserve			(32);
	string16						I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; ++j, ++i)
			tTerrainPlace.tMask[j]	= _LOCATION_ID(atoi(_GetItem(S,i,I)));
		tTerrainPlace.dwMinTime		= atoi(_GetItem(S,i++,I))*1000;
		tTerrainPlace.dwMaxTime		= atoi(_GetItem(S,i++,I))*1000;
		m_tpaTerrain.push_back		(tTerrainPlace);
	}

	m_dwMaxDynamicObjectsCount		= _min(pSettings->r_s32(section,"DynamicObjectsCount"),MAX_DYNAMIC_OBJECTS);
	m_dwMaxDynamicSoundsCount		= _min(pSettings->r_s32(section,"DynamicSoundsCount"),MAX_DYNAMIC_SOUNDS);

	// skeleton physics
	m_pPhysics_support				->in_Load(section);

	m_tpSoundStep[0].g_type			= SOUND_TYPE_MONSTER_WALKING_HUMAN;
	m_tpSoundStep[1].g_type			= SOUND_TYPE_MONSTER_WALKING_HUMAN;
//	::Sound->create					(m_tpSoundStep[0],	TRUE,	"Actor\\StepL",						SOUND_TYPE_MONSTER_WALKING_HUMAN);
//	::Sound->create					(m_tpSoundStep[1],	TRUE,	"Actor\\StepR",						SOUND_TYPE_MONSTER_WALKING_HUMAN);

	g_vfLoadSounds					(m_tpSoundDie,pSettings->r_string(section,"sound_death"),100);
	g_vfLoadSounds					(m_tpSoundHit,pSettings->r_string(section,"sound_hit"),100);
	g_vfLoadSounds					(m_tpSoundHumming,pSettings->r_string(section,"sound_humming"),100);
	g_vfLoadSounds					(m_tpSoundAlarm,pSettings->r_string(section,"sound_alarm"),100);
	g_vfLoadSounds					(m_tpSoundSurrender,pSettings->r_string(section,"sound_surrender"),100);

	// prefetching
//	cNameVisual_set					("actors\\different_stalkers\\stalker_black_mask");
//	cNameVisual_set					("actors\\different_stalkers\\stalker_ecolog");
//	cNameVisual_set					("actors\\different_stalkers\\stalker_hood_multiplayer");
//	cNameVisual_set					("actors\\different_stalkers\\stalker_no_hood_singleplayer");
//	cNameVisual_set					("actors\\different_stalkers\\stalker_scientist");
//	cNameVisual_set					("actors\\different_stalkers\\stalker_svoboda");
//	cNameVisual_set					("actors\\different_stalkers\\stalker_svoboda2");
//	cNameVisual_set					("actors\\different_stalkers\\stalker_svoboda3");
//	cNameVisual_set					("actors\\different_stalkers\\stalker_svoboda4");
//	cNameVisual_set					("actors\\different_stalkers\\stalker_ik_test");
//	cNameVisual_set					("actors\\different_stalkers\\stalker_ik_test_koan");
}

BOOL CAI_Stalker::net_Spawn			(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return						(FALSE);
	Init							();

	//����������� PDA � InventoryOwner
	if (!CInventoryOwner::net_Spawn(DC)) return FALSE;

	m_PhysicMovementControl.SetPLastMaterial		(&m_wLastMaterialID);

	CSE_Abstract					*e	= (CSE_Abstract*)(DC);
	CSE_ALifeHumanAbstract			*tpHuman = dynamic_cast<CSE_ALifeHumanAbstract*>(e);
	R_ASSERT						(tpHuman);

	m_head.current.yaw = m_head.target.yaw = m_body.current.yaw = m_body.target.yaw	= angle_normalize_signed(-tpHuman->o_Angle.y);
	m_body.current.pitch			= m_body.target.pitch	= 0;

	m_tGraphID						= tpHuman->m_tGraphID;
	m_tNextGraphID					= tpHuman->m_tNextGraphID;
	m_dwBornTime					= Level().timeServer();

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if (u32(-1) == m_dwParticularState) {
		R_ASSERT2					(
			ai().get_game_graph() && 
			ai().get_level_graph() && 
			ai().get_cross_table() && 
			(ai().level_graph().level_id() != u32(-1)),
			"There is no AI-Map, level graph, cross table, or graph is not compiled into the game graph!"
		);
		m_tNextGraphID				= m_tGraphID = ai().cross_table().vertex(level_vertex_id()).game_vertex_id();
	}
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	CStalkerAnimations::Load		(PSkeletonAnimated(Visual()));
	vfAssignBones					(pSettings,cNameSect());

	setEnabled						(true);

	m_pPhysics_support->in_NetSpawn();
	m_PhysicMovementControl.SetPosition	(Position());
	m_PhysicMovementControl.SetVelocity	(0,0,0);

	if (!Level().CurrentViewEntity())
		Level().SetEntity(this);

	// load damage params
	if (pSettings->line_exist(cNameSect(),"damage")) {
		string32 buf;
		CInifile::Sect& dam_sect	= pSettings->r_section(pSettings->r_string(cNameSect(),"damage"));
		for (CInifile::SectIt it=dam_sect.begin(); dam_sect.end() != it; ++it)
		{
			if (0==strcmp(*it->first,"default")){
				m_fHitFactor	= (float)atof(*it->second);
			}else{
				int bone	= PKinematics(Visual())->LL_BoneID(*it->first); 
				R_ASSERT2(BI_NONE!=bone,*it->first);
				CBoneInstance& B = PKinematics(Visual())->LL_GetBoneInstance(u16(bone));
				B.set_param(0,(float)atof(_GetItem(*it->second,0,buf)));
				B.set_param(1,(float)atoi(_GetItem(*it->second,1,buf)));
			}
		}
	}

	m_tpKnownCustomers				= tpHuman->m_tpKnownCustomers;

	return							(TRUE);
}

void CAI_Stalker::net_Destroy()
{
	inherited::net_Destroy	();
	m_pPhysics_support->in_NetDestroy();
	m_inventory.Clear		();
}

void CAI_Stalker::net_Export		(NET_Packet& P)
{
	R_ASSERT						(Local());

	// export last known packet
	R_ASSERT						(!NET.empty());
	net_update& N					= NET.back();
	P.w_float						(m_inventory.TotalWeight());
	P.w_u32							(0);
	P.w_u32							(0);

	P.w_float_q16					(fEntityHealth,-1000,1000);

	P.w_u32							(N.dwTimeStamp);
	P.w_u8							(0);
	P.w_vec3						(N.p_pos);
	P.w_angle8						(N.o_model);
	P.w_angle8						(N.o_torso.yaw);
	P.w_angle8						(N.o_torso.pitch);

	float					f1 = 0;
	ALife::_GRAPH_ID		l_game_vertex_id = game_vertex_id();
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&l_game_vertex_id,			sizeof(l_game_vertex_id));
	P.w						(&f1,						sizeof(f1));
	P.w						(&f1,						sizeof(f1));
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

	P.w_u32							(0);
	P.w_u32							(0);
	P.w_u32							(0);
//	P.w_u32							(0);
}

void CAI_Stalker::net_Import		(NET_Packet& P)
{
	R_ASSERT						(Remote());
	net_update						N;

	u8 flags;

	float health;
	P.r_float_q16		(health,-1000,1000);
	fEntityHealth = health;

	P.r_u32							(N.dwTimeStamp);
	P.r_u8							(flags);
	P.r_vec3						(N.p_pos);
	P.r_angle8						(N.o_model);
	P.r_angle8						(N.o_torso.yaw);
	P.r_angle8						(N.o_torso.pitch);

	P.r								(&m_tNextGraphID,		sizeof(m_tNextGraphID));
	P.r								(&m_tGraphID,			sizeof(m_tGraphID));

	if (NET.empty() || (NET.back().dwTimeStamp<N.dwTimeStamp))	{
		NET.push_back				(N);
		NET_WasInterpolating		= TRUE;
	}

	float							fDummy;
	u32								dwDummy;
	P.r_float						(fDummy);
	P.r_u32							(dwDummy);
	P.r_u32							(dwDummy);

	P.r_u32							(dwDummy);
	P.r_u32							(dwDummy);
	P.r_u32							(dwDummy);
//	P.r_u32							(dwDummy);

	setVisible						(TRUE);
	setEnabled						(TRUE);
}

void CAI_Stalker::UpdateCL(){

	inherited::UpdateCL();
	m_pPhysics_support->in_UpdateCL();
		if (this == Level().CurrentViewEntity()) {
			Exec_Visibility();
		}
}

void CAI_Stalker::Hit(float P, Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type){

	if (m_pPhysics_support->isAlive())
		inherited::Hit(P,dir,who,element,p_in_object_space,impulse,hit_type);
	m_pPhysics_support->in_Hit(P,dir,who,element,p_in_object_space,impulse);
}

void CAI_Stalker::shedule_Update		( u32 DT )
{
	// Queue shrink
	VERIFY				(_valid(Position()));
	u32	dwTimeCL		= Level().timeServer()-NET_Latency;
	VERIFY				(!NET.empty());
	while ((NET.size()>2) && (NET[1].dwTimeStamp<dwTimeCL)) NET.pop_front();

	// Queue setup
	float dt			= float(DT)/1000.f;
	if (dt > 3)
		return;

	Fvector				vNewPosition = Position();
	VERIFY				(_valid(Position()));
	// *** general stuff
	inherited::inherited::shedule_Update	(DT);
	
	if (Remote())		{
	} else {
		// here is monster AI call
		VERIFY				(_valid(Position()));
		m_fTimeUpdateDelta				= dt;
		Device.Statistic.AI_Think.Begin	();
		if (GetScriptControl())
			ProcessScripts				();
		else
			Think						();
		m_dwLastUpdateTime				= Level().timeServer();
		Device.Statistic.AI_Think.End	();
		Engine.Sheduler.Slice			();
		VERIFY				(_valid(Position()));

		// Look and action streams
		if (fEntityHealth>0) {
			VERIFY				(_valid(Position()));
			Exec_Look				(dt);
			VERIFY				(_valid(Position()));
			Exec_Visibility			();

			//////////////////////////////////////
			Fvector C; float R;
			//////////////////////////////////////
			// � ����� - ����!!!! (���� :-))))
			// m_PhysicMovementControl.GetBoundingSphere	(C,R);
			//////////////////////////////////////
			Center(C);
			R = Radius();
			//////////////////////////////////////
			feel_touch_update		(C,R);

			net_update				uNext;
			uNext.dwTimeStamp		= Level().timeServer();
			uNext.o_model			= m_body.current.yaw;
			uNext.o_torso			= m_head.current;
			uNext.p_pos				= vNewPosition;
			uNext.fHealth			= fEntityHealth;
			NET.push_back			(uNext);
		}
		else 
		{
			net_update			uNext;
			uNext.dwTimeStamp	= Level().timeServer();
			uNext.o_model		= m_body.current.yaw;
			uNext.o_torso		= m_head.current;
			uNext.p_pos			= vNewPosition;
			uNext.fHealth		= fEntityHealth;
			NET.push_back		(uNext);
		}
	}
	VERIFY				(_valid(Position()));

	// inventory update
	if (m_dwDeathTime && (m_inventory.TotalWeight() > 0)) {
		CWeapon *tpWeapon = dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
		if (!tpWeapon || !tpWeapon->GetAmmoElapsed() || !m_bHammerIsClutched || (Level().timeServer() - m_dwDeathTime > 500)) {
			xr_vector<CInventorySlot>::iterator I = m_inventory.m_slots.begin(), B = I;
			xr_vector<CInventorySlot>::iterator E = m_inventory.m_slots.end();
			for ( ; I != E; ++I)
				if ((I - B) == (int)m_inventory.GetActiveSlot()) 
					(*I).m_pIItem->Drop();
				else
					if((*I).m_pIItem) m_inventory.Ruck((*I).m_pIItem);

			/*TIItemList &l_list = m_inventory.m_ruck;
			for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
				if ((*l_it)->Useful())
					(*l_it)->Drop();*/
		}
		else {
			m_inventory.Action(kWPN_FIRE,	CMD_START);
			xr_vector<CInventorySlot>::iterator I = m_inventory.m_slots.begin(), B = I;
			xr_vector<CInventorySlot>::iterator E = m_inventory.m_slots.end();
			for ( ; I != E; ++I)
				if ((I - B) != (int)m_inventory.GetActiveSlot())
					m_inventory.Ruck((*I).m_pIItem);
			//		(*I).m_pIItem->Drop();
			
			/*TIItemList &l_list = m_inventory.m_ruck;
			for(PPIItem l_it = l_list.begin(); l_list.end() != l_it; ++l_it)
				if ((*l_it)->Useful())
					(**l_it).Drop();*/
		}
	}
	VERIFY				(_valid(Position()));

	if (g_Alive()) {
		R_ASSERT					(GAMEMTL_NONE != m_wLastMaterialID);
		SGameMtlPair				*mtl_pair = GMLib.GetMaterialPair(m_wMyMaterialID,m_wLastMaterialID);
		R_ASSERT3					(mtl_pair,"Undefined material pair: Actor # ", *GMLib.GetMaterial(m_wLastMaterialID)->m_Name);
		// ref_sound step
		if (eMovementTypeStand != m_tMovementType) {
			if(m_fTimeToStep < 0) {
				m_bStep				= !m_bStep;
				float k				= (eBodyStateCrouch == m_tBodyState) ? 0.75f : 1.f;
				float tm			= (eMovementTypeRun == m_tMovementType) ? (PI/(k*10.f)) : (PI/(k*7.f));
				m_fTimeToStep		= tm;
				if (mtl_pair->StepSounds.size()>=2){
					m_tpSoundStep[m_bStep].clone		(mtl_pair->StepSounds[m_bStep]);
					m_tpSoundStep[m_bStep].play_at_pos	(this,Position());
				}
			}
			m_fTimeToStep -= dt;
		}
		float	s_k			= ffGetStartVolume(SOUND_TYPE_MONSTER_WALKING)*((eBodyStateCrouch == m_tBodyState) ? CROUCH_SOUND_FACTOR : 1.f);
		float	s_vol		= s_k*((eMovementTypeRun == m_tMovementType) ? 1.f : ACCELERATED_SOUND_FACTOR);
		if (m_tpSoundStep[0].feedback)		{
			m_tpSoundStep[0].set_position	(Position());
			m_tpSoundStep[0].set_volume	(s_vol);
		}
		if (m_tpSoundStep[1].feedback)		{
			m_tpSoundStep[1].set_position	(Position());
			m_tpSoundStep[1].set_volume	(s_vol);
		}
	}
	VERIFY				(_valid(Position()));
//	m_inventory.Update(DT);
	UpdateInventoryOwner(DT);
	VERIFY				(_valid(Position()));

	m_pPhysics_support->in_shedule_Update(DT);
	VERIFY				(_valid(Position()));
}

float CAI_Stalker::Radius()const
{ 
	float R		= inherited::Radius();
	CWeapon* W	= dynamic_cast<CWeapon*>(m_inventory.ActiveItem());
	if (W) R	+= W->Radius();
	return R;
}

/////////////////////////
//PDA functions
void CAI_Stalker::ReceivePdaMessage(u16 who, EPdaMsg msg, EPdaMsgAnger anger)
{
	EPdaMsg pda_msg = ePdaMsgAccept;

	if(GetPDA()->NeedToAnswer(who))
	{
		switch(msg)
		{
			case ePdaMsgTrade:
				pda_msg = ePdaMsgAccept;
				break;
			case ePdaMsgNeedHelp:
				pda_msg = ePdaMsgDecline;
				break;
			case ePdaMsgGoAway:
				pda_msg = ePdaMsgDeclineRude;
				break;
		}

		//������� �� ���������
		SendPdaMessage(who, pda_msg, anger);
	}
}