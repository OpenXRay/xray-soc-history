////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_soldier.cpp
//	Created 	: 25.04.2002
//  Modified 	: 25.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Soldier"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_soldier.h"
#include "ai_soldier_selectors.h"
#include "..\\ai_monsters_misc.h"
#include "..\\..\\xr_weapon_list.h"
#include "..\\..\\hudmanager.h"
#include "..\\..\\..\\xr_trims.h"

bool				CAI_Soldier::bPatternFunctionLoaded = false;
CPatternFunction	CAI_Soldier::pfRelation;
CBaseFunction		*CAI_Soldier::fpaBaseFunctions[MAX_FUNCTION_COUNT];
CHealthFunction		CAI_Soldier::pfHealth;
CArmorFunction		CAI_Soldier::pfArmor;
CMoraleFunction		CAI_Soldier::pfMorale;
CStrengthFunction	CAI_Soldier::pfStrength;
CAccuracyFunction	CAI_Soldier::pfAccuracy;
CReactionFunction	CAI_Soldier::pfReaction;

CAI_Soldier::CAI_Soldier()
{
	dwHitTime = DWORD(-1);
	tHitDir.set(0,0,1);
	tSavedEnemy = 0;
	tSavedEnemyPosition.set(0,0,0);
	tpSavedEnemyNode = 0;
	dwSavedEnemyNodeID = DWORD(-1);
	dwLostEnemyTime = 0;
	bBuildPathToLostEnemy = false;
	m_dwLastRangeSearch = 0;
	m_dwLastSuccessfullSearch = 0;
	m_fAggressiveness = ::Random.randF(0,1);
	m_fTimorousness = ::Random.randF(0,1);
	m_bFiring = false;
	m_bLessCoverLook = false;
	q_look.o_look_speed = r_spine_speed = r_torso_speed = PI_DIV_2;
	m_tpCurrentGlobalAnimation = 
	m_tpCurrentTorsoAnimation = 
	m_tpCurrentLegsAnimation = 0;
	m_tpCurrentGlobalBlend = 
	m_tpCurrentTorsoBlend = 
	m_tpCurrentLegsBlend = 0;
	m_bActionStarted = false;
	m_bJumping = false;
	tpaDynamicObjects.clear();
	tpaDynamicSounds.clear();
	m_tpEventSay = Engine.Event.Handler_Attach("level.entity.say",this);
	m_tpEventAssignPath = Engine.Event.Handler_Attach("level.entity.path.assign",this);
	m_dwLastUpdate = 0;
	m_fSensetivity = .0f;
	m_iSoundIndex = -1;
	m_cGestureState = GESTURE_STATE_NONE;
	m_dwLastRadioTalk = 0;
	m_tpSoundBeingPlayed = 0;
	m_dwLastSoundRefresh = 0;
	m_fDistanceWent = 0.f;
	m_cStep = 0;
	AI_Path.fSpeed = 0;
	r_torso_speed = q_look.o_look_speed = PI_DIV_2;
	m_tActionType = ACTION_TYPE_NONE;
	m_tFightType = FIGHT_TYPE_NONE;
	m_dwLastUpdate = 0;
	m_dwCurrentUpdate = Level().timeServer();
	m_dwUpdateCount = 0;
	m_iCurrentSuspiciousNodeIndex = -1;
	m_tpaNodeStack.clear();
}

CAI_Soldier::~CAI_Soldier()
{
	for (int i=0; i<SND_HIT_COUNT; i++) pSounds->Delete(sndHit[i]);
	for (i=0; i<SND_DIE_COUNT; i++) pSounds->Delete(sndDie[i]);
	for (i=0; i<SND_RADIO_COUNT; i++) pSounds->Delete(sndRadio[i]);
	Engine.Event.Handler_Detach (m_tpEventSay,this);
	Engine.Event.Handler_Detach (m_tpEventAssignPath,this);
	Msg("FSM report for %s :",cName());
	for (int i=0; i<(int)tStateList.size(); i++)
		Msg("%3d %6d",tStateList[i].eState,tStateList[i].dwTime);
	Msg("Total updates : %d",m_dwUpdateCount);
}

// when soldier is dead
void CAI_Soldier::Death()
{
	// perform death operations
	inherited::Death( );
	CGroup &Group = Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()];
	vfSetFire(false,Group);
	eCurrentState = aiSoldierDie;
	
	// removing from group
	//Level().Teams[g_Team()].Squads[g_Squad()].Groups[g_Group()].Member_Remove(this);
	
	Fvector	dir;
	AI_Path.Direction(dir);
	SelectAnimation(clTransform.k,dir,AI_Path.fSpeed);
	
	// Play sound
	pSounds->PlayAtPos(sndDie[Random.randI(SND_DIE_COUNT)],this,vPosition);
}

void CAI_Soldier::vfLoadSelectors(LPCSTR section)
{
	SelectorAttack.Load(section);
	SelectorDefend.Load(section);
	SelectorFindEnemy.Load(section);
	SelectorFollowLeader.Load(section);
	SelectorFreeHunting.Load(section);
	SelectorMoreDeadThanAlive.Load(section);
	SelectorNoWeapon.Load(section);
	SelectorPatrol.Load(section);
	SelectorPursuit.Load(section);
	SelectorReload.Load(section);
	SelectorRetreat.Load(section);
	SelectorSenseSomething.Load(section);
	SelectorUnderFireCover.Load(section);
	SelectorUnderFireLine.Load(section);
}

void CAI_Soldier::Load	(LPCSTR section)
{ 
	// load parameters from ".ini" file
	inherited::Load		(section);
	
	// initialize start position
	Fvector			P = vPosition;
	P.x				+= ::Random.randF();
	P.z				+= ::Random.randF();
	
	vfLoadSounds();
	vfLoadAnimations();
	vfLoadSelectors(section);
	vfAssignBones(pSettings,section);
	
	// visibility
	m_dwMovementIdleTime = pSettings->ReadINT(section,"MovementIdleTime");
	m_fMaxInvisibleSpeed = pSettings->ReadFLOAT(section,"MaxInvisibleSpeed");
	m_fMaxViewableSpeed = pSettings->ReadFLOAT(section,"MaxViewableSpeed");
	m_fMovementSpeedWeight = pSettings->ReadFLOAT(section,"MovementSpeedWeight");
	m_fDistanceWeight = pSettings->ReadFLOAT(section,"DistanceWeight");
	m_fSpeedWeight = pSettings->ReadFLOAT(section,"SpeedWeight");
	m_fCrouchVisibilityMultiplier = pSettings->ReadFLOAT(section,"CrouchVisibilityMultiplier");
	m_fLieVisibilityMultiplier = pSettings->ReadFLOAT(section,"LieVisibilityMultiplier");
	m_fVisibilityThreshold = pSettings->ReadFLOAT(section,"VisibilityThreshold");
	m_fLateralMultiplier = pSettings->ReadFLOAT(section,"LateralMultiplier");
	m_fShadowWeight = pSettings->ReadFLOAT(section,"ShadowWeight");
	
	//fire
	m_dwFireRandomMin  = pSettings->ReadINT(section,"FireRandomMin");
	m_dwFireRandomMax  = pSettings->ReadINT(section,"FireRandomMax");
	m_dwNoFireTimeMin  = pSettings->ReadINT(section,"NoFireTimeMin");
	m_dwNoFireTimeMax  = pSettings->ReadINT(section,"NoFireTimeMax");
	m_fMinMissDistance = pSettings->ReadFLOAT(section,"MinMissDistance");
	m_fMinMissFactor   = pSettings->ReadFLOAT(section,"MinMissFactor");
	m_fMaxMissDistance = pSettings->ReadFLOAT(section,"MaxMissDistance");
	m_fMaxMissFactor   = pSettings->ReadFLOAT(section,"MaxMissFactor");

	m_fMinRadioIinterval = pSettings->ReadFLOAT(section,"MinRadioIinterval");
	m_fMaxRadioIinterval = pSettings->ReadFLOAT(section,"MaxRadioIinterval");
	m_fRadioRefreshRate	 = pSettings->ReadFLOAT(section,"RadioRefreshRate");
	
	m_dwMaxDynamicObjectsCount = min(pSettings->ReadINT(section,"DynamicObjectsCount"),MAX_DYNAMIC_OBJECTS);
	m_dwMaxDynamicSoundsCount = min(pSettings->ReadINT(section,"DynamicSoundsCount"),MAX_DYNAMIC_SOUNDS);

	if (!bPatternFunctionLoaded) {
		bPatternFunctionLoaded = true;
		fpaBaseFunctions[0] = &pfHealth;
		fpaBaseFunctions[1] = &pfArmor;
		fpaBaseFunctions[2] = &pfMorale;
		fpaBaseFunctions[3] = &pfStrength;
		fpaBaseFunctions[4] = &pfAccuracy;
		fpaBaseFunctions[5] = &pfReaction;
		//pfRelation.vfLoadEF(pSettings->ReadSTRING(section,"Relation"),fpaBaseFunctions);
	}

	//Msg("Evaluation Function Relation : %8.2f",pfRelation.dfGetValue(this,fpaBaseFunctions));

	//tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[0] = tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[1] = tSoldierAnimations.tNormal.tGlobal.tpaWalkForward[2] = PKinematics(pVisual)->ID_Cycle(pSettings->ReadSTRING(section,"TestAnimation"));
	//m_fAddAngle = pSettings->ReadFLOAT(section,"AddAngle");
}

BOOL CAI_Soldier::net_Spawn	(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	if (!inherited::net_Spawn(bLocal,server_id,o_pos,o_angle,P,flags))	return FALSE;
	//tSavedEnemyPosition = vPosition;
	tStateStack.push(eCurrentState = aiSoldierLookingOver);
	vfAddStateToList(eCurrentState);
	return TRUE;
}

// soldier update
void CAI_Soldier::Update(DWORD DT)
{
	inherited::Update(DT);
	vfUpdateSounds(DT);
}

void CAI_Soldier::Exec_Movement	( float dt )
{
	if (fabsf(vPosition.x) > 10000.f) {
		if (ps_Size() > 1)
			vPosition = ps_Element(ps_Size() - 2).vPosition;
		Msg("%s",cName());
	}
	AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,dt);
	/**
	if (eCurrentState != aiSoldierJumping)
		AI_Path.Calculate(this,vPosition,vPosition,m_fCurSpeed,dt);
	else {
		UpdateTransform();
		if (m_bActionStarted) {
			m_bActionStarted = false;
			Fvector tAcceleration, tVelocity;
			tVelocity.set(0,1,0);
			Movement.SetPosition(vPosition);
			Movement.SetVelocity(tVelocity);
			tAcceleration.set(0,0,0);
			Movement.SetPosition(vPosition);
			Movement.Calculate	(tAcceleration,0,m_cBodyState == BODY_STATE_STAND ? m_fJumpSpeed : m_fJumpSpeed*.8f,dt > .1f ? .1f : dt,false);
			Movement.GetPosition(vPosition);
		}
		else {
			Fvector tAcceleration;
			tAcceleration.set(0,m_cBodyState == BODY_STATE_STAND ? m_fJumpSpeed : m_fJumpSpeed*.8f,0);
			Movement.SetPosition(vPosition);
			Movement.Calculate	(tAcceleration,0,0,dt > .1f ? .1f : dt,false);
			Movement.GetPosition(vPosition);
		}
		UpdateTransform	();
	}
	/**/
}

void CAI_Soldier::OnEvent(EVENT E, DWORD P1, DWORD P2)
{

	if (E == m_tpEventSay) {
		if (0==P2 || DWORD(this)==P2) {
			char* caTextToShow;
			caTextToShow = (char *)P1;
			Level().HUD()->outMessage(0xffffffff,cName(),"%s",caTextToShow);
		}
	}
	else
		if (E == m_tpEventAssignPath) {
			if (0==P2 || DWORD(this)==P2) {
				char *buf2, *buf, monster_name[100];
				buf2 = buf = (char *)P1;
				int iArgCount = _GetItemCount(buf2);
				if (iArgCount >= 4) {
					DWORD team,squad,group;
					for ( ; ; buf2++)
						if (*buf2 == ',') {
							memcpy(monster_name,buf,(buf2 - buf)*sizeof(char));
							monster_name[buf2++ - buf] = 0;
							break;
						}
						
					if ((strlen(monster_name)) && (strcmp(monster_name,cName())))
						return;
					
					if (!(strlen(monster_name))) {
						sscanf(buf2,"%d,%d,%d",&team,&squad,&group);
						
						if (((int)team != g_Team()) || ((int)squad != g_Squad()) || ((int)group != g_Group()))
							return;
						
						for (int komas=0; komas<3; buf2++)
							if (*buf2 == ',')
								komas++;
					}
				}
				
				INIT_SQUAD_AND_LEADER;
				CGroup &Group = Squad.Groups[g_Group()];
				
				m_tpPath = 0;
				CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(Leader);
				m_dwLoopCount = 0;
				if (tpCustomMonster && (tpCustomMonster->m_tpPath))
					m_tpPath = tpCustomMonster->m_tpPath;
				else {
					for (int i=0; i<(int)Group.Members.size(); i++) {
						CCustomMonster *tpCustomMonster = dynamic_cast<CCustomMonster *>(Group.Members[i]);
						if (tpCustomMonster && (tpCustomMonster->m_tpPath)) {
							m_tpPath = tpCustomMonster->m_tpPath;
							break;
						}
					}
					if (!m_tpPath) {
						for (int i=0, iCount = 1; buf2[i]; i++)
							if (buf2[i] == ',')
								iCount++;
						if (iCount == 1) {
							m_tpPath = &(Level().m_PatrolPaths[buf2]);
							if (!m_tpPath) {
								Msg("Cannot find specified path (%s)",buf2);
								THROW;
							}
						}
						else {
							iCount = ::Random.randI(0,iCount);
							for (int i=0, iCountX = 0; buf2[i]; i++) {
								if (iCountX == iCount) {
									int j=i;
									for (; buf2[i]; i++)
										if (buf2[i] == ',') {
											buf2[i] = 0;
											break;
										}
									buf2 += j;
									m_tpPath = &(Level().m_PatrolPaths[buf2]);
									if (!m_tpPath) {
										Msg("Cannot find specified path (%s)",buf2);
										THROW;
									}
									break;
								}
								if (buf2[i] == ',')
									iCountX++;
							}
						}
					}
				}
				if (!m_tpPath) {
					Msg("Cannot find specified path");
					THROW;
				}
			}
		}
}