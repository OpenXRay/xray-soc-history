#include "stdafx.h"
#include "level.h"
#include "entity.h"
#include "custommonster.h"
#include "ai_space.h"

CGroup::CGroup()
{
	vTargetDirection.set	(0,0,1);
	vTargetPosition.set		(0,0,0);
	vCentroid.set			(0,0,0);
	m_bLeaderViewsEnemy		= false;
	m_dwLeaderChangeCount	= 0;
	m_tLastHitDirection.set	(1,0,0);
	m_tHitPosition.set		(1,0,0);
	m_dwLastHitTime			= u32(-1);
	m_dwFiring				= 0;
	m_bEnemyNoticed			= false;
	m_bLessCoverLook		= false;
	m_tpaPatrolPath.clear	();
	m_dwLastActionTime		= 0;
	m_dwLastAction			= 0;
	m_dwActiveCount			= 0;
	m_dwAliveCount			= 0;
	m_dwStandingCount		= 0;
}

const Fvector& CGroup::GetCentroid()
{
	vCentroid.set	(0,0,0);
	for (u32 I=0; I<Members.size(); I++) 
		vCentroid.add	(Members[I]->Position());
	vCentroid.div	(float(Members.size()));
	return vCentroid;
}

void CGroup::Member_Add(CEntity* E){
	Members.push_back(E);
}

void CGroup::Member_Remove(CEntity* E){
	EntityIt it = std::find(Members.begin(),Members.end(),E);
	if (it!=Members.end()) Members.erase(it);
}

void CGroup::GetAliveMemberPlacement(MemberPlacement& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	MP.reserve		(Members.size());
	vCentroid.set	(0,0,0);
	for (u32 I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			CEntity*		E = Members[I];
			const Fvector&	P = E->Position();
			vCentroid.add	(P);
			if (E!=Me)	MP.push_back(P);
		}
	vCentroid.div	(float(Members.size()));
}

void CGroup::GetAliveMemberPlacementN(MemberNodes& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	MP.reserve		(Members.size());
	for (u32 I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			CEntity*	E = Members[I];
			if (E!=Me)	MP.push_back(E->AI_NodeID);
		}
}

void CGroup::GetAliveMemberDedication(MemberPlacement& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	MP.reserve		(Members.size());
	vCentroid.set	(0,0,0);
	for (u32 I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR) {
				CEntity*		E = Members[I];
				const Fvector&	P = E->Position();
				MP.push_back				(P);
			}
			else {
				CEntity*		E = Members[I];
				const Fvector&	P = E->Position();
				vCentroid.add	(P);
				if (E!=Me)	{
					CCustomMonster* M = dynamic_cast<CCustomMonster*>(E);
					if (M)
						if (M->AI_Path.DestNode != u32(-1))
							MP.push_back(getAI().tfGetNodeCenter(M->AI_Path.DestNode));
						else {
							Fvector tTemp;
							tTemp.set(0,0,0);
							MP.push_back(tTemp);
						}
				}
			}
		}
	vCentroid.div	(float(Members.size()));
}

void CGroup::GetAliveMemberDedicationN(MemberNodes& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	MP.reserve		(Members.size());
	for (u32 I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR)
				MP.push_back(Members[I]->AI_NodeID);
			else {
				CEntity*		E = Members[I];
				if (E!=Me)	{
					CCustomMonster* M	= dynamic_cast<CCustomMonster*>(E);
					if (M)
						MP.push_back		(M->AI_Path.DestNode);
				}
			}
		}
}

void CGroup::GetAliveMemberInfo(MemberPlacement& P0, MemberNodes& P1, MemberPlacement& P2, MemberNodes& P3, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	P0.clear();
	P1.clear();
	P2.clear();
	P3.clear();
	P0.reserve(Members.size());
	P1.reserve(Members.size());
	P2.reserve(Members.size());
	P3.reserve(Members.size());
	for (u32 I=0; I<Members.size(); I++)
		if (Members[I]->g_Health() > 0) {
			if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR) {
				CEntity*E = Members[I];
				const Fvector&	P = E->Position();
				P0.push_back(P);
				P1.push_back(E->AI_NodeID);
				P2.push_back(P);
				P3.push_back(E->AI_NodeID);
			}
			else {
				CEntity*E = Members[I];
				if (E!=Me) {
					CCustomMonster* M = dynamic_cast<CCustomMonster*>(E);
					if (M) {
						P0.push_back(E->Position());
						P1.push_back(M->AI_NodeID);
						if (M->AI_Path.DestNode != u32(-1)) {
							P2.push_back(getAI().tfGetNodeCenter(M->AI_Path.DestNode));
							P3.push_back(M->AI_Path.DestNode);
						}
						else {
							Fvector tTemp;
							tTemp.set(0,0,0);
							P2.push_back(tTemp);
							P3.push_back(0);
						}
					}
				}
			}
		}
}