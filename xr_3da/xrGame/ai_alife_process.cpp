////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_process.cpp
//	Created 	: 14.01.2003
//  Modified 	: 14.01.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life simulation processing
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife.h"
#include "a_star.h"

//#define WRITE_TO_LOG

void CAI_ALife::Update(u32 dt)
{
	inherited::Update(dt);
	if (!m_bLoaded)
		return;
#ifdef WRITE_TO_LOG
	Msg("* %7.2fs",Level().timeServer()/1000.f);
#endif
	u64	qwStartTime = CPU::GetCycleCount();
	if (m_tpScheduledObjects.size()) {
		int i=0;
		do {
			i++;
			m_dwObjectsBeingProcessed = ((m_dwObjectsBeingProcessed + 1) % m_tpScheduledObjects.size());
			vfProcessNPC(m_tpScheduledObjects[m_dwObjectsBeingProcessed]);
		}
		while (((CPU::GetCycleCount() - qwStartTime)*(i + 1)/i < m_qwMaxProcessTime) && (i < (int)m_tpScheduledObjects.size()));
	}
#ifdef WRITE_TO_LOG
	u64 t2x = CPU::GetCycleCount() - qwStartTime;
	Msg("* %.3f microseconds",CPU::cycles2microsec*t2x);
#endif
}

void CAI_ALife::vfProcessNPC(CALifeMonsterAbstract	*tpALifeMonsterAbstract)
{
	CALifeHumanAbstract *tpALifeHumanAbstract = dynamic_cast<CALifeHumanAbstract *>(tpALifeMonsterAbstract);
	if (tpALifeHumanAbstract) {
		CALifeHuman *tpALifeHuman = dynamic_cast<CALifeHuman *>(tpALifeMonsterAbstract);
		if (tpALifeHuman)
			if (tpALifeHuman->m_bIsTrader)
				vfUpdateTrader(tpALifeHuman);
			else
				vfUpdateHuman(tpALifeHuman);
		else
			vfUpdateHumanGroup(dynamic_cast<CALifeHumanGroup *>(tpALifeMonsterAbstract));
	}
	else
		vfUpdateMonster(tpALifeMonsterAbstract);
	tpALifeMonsterAbstract->m_tTimeID = Level().timeServer();
}

void CAI_ALife::vfUpdateMonster(CALifeMonsterAbstract *tpALifeMonsterAbstract)
{
	vfChooseNextRoutePoint	(tpALifeMonsterAbstract);
	vfCheckForTheBattle		(tpALifeMonsterAbstract);
}

void CAI_ALife::vfUpdateTrader(CALifeHuman *tpALifeHuman)
{
}

void CAI_ALife::vfUpdateHumanGroup(CALifeHumanGroup *tpALifeHumanGroup)
{
}

void CAI_ALife::vfUpdateHuman(CALifeHuman *tpALifeHuman)
{
	switch (tpALifeHuman->m_tTaskState) {
		case eTaskStateNone : {
			if (!tpALifeHuman->m_tpTaskIDs.size()) {
				if (!tpALifeHuman->m_tpaVertices.size()) {
					CALifeHuman *tpTrader = tpfGetNearestSuitableTrader(tpALifeHuman);
					Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,tpTrader->m_tGraphID,tpALifeHuman->m_tpaVertices);
					tpALifeHuman->m_dwCurNode = 0;
				}
				else {
					if ((tpALifeHuman->m_dwCurNode >= (tpALifeHuman->m_tpaVertices.size() - 1)) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID)) {
						tpALifeHuman->m_tpaVertices.clear();
						tpALifeHuman->m_dwCurNode = u32(-1);
						vfCommunicateWithTrader(tpALifeHuman,tpfGetNearestSuitableTrader(tpALifeHuman));
					}
				}
			}
			else {
				tpALifeHuman->m_dwCurTask = 0;
				tpALifeHuman->m_tTaskState = eTaskStateGoing;
				Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tTaskRegistry.m_tpMap[tpALifeHuman->m_tpTaskIDs[tpALifeHuman->m_dwCurTask]].tGraphID,tpALifeHuman->m_tpaVertices);
				tpALifeHuman->m_dwCurNode = 0;
			}
			break;
		}
		case eTaskStateGoing : {
			if ((tpALifeHuman->m_dwCurNode >= (tpALifeHuman->m_tpaVertices.size() - 1)) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID)) {
				TASK_PAIR_IT T = m_tTaskRegistry.m_tpMap.find(tpALifeHuman->m_tpTaskIDs[tpALifeHuman->m_dwCurTask]);
				if (T != m_tTaskRegistry.m_tpMap.end()) {
					Level().AI.m_tpAStar->ffFindMinimalPath(tpALifeHuman->m_tGraphID,m_tObjectRegistry.m_tppMap[m_tTaskRegistry.m_tpMap[tpALifeHuman->m_tpTaskIDs[tpALifeHuman->m_dwCurTask]].tCustomerID]->m_tGraphID,tpALifeHuman->m_tpaVertices);
					tpALifeHuman->m_dwCurNode = 0;
				}
				tpALifeHuman->m_tTaskState = eTaskStateReturningSuccess;
			}
			break;
		}
		
		case eTaskStateReturningSuccess : {
			if ((tpALifeHuman->m_dwCurNode >= (tpALifeHuman->m_tpaVertices.size() - 1)) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID)) {
				tpALifeHuman->m_tpaVertices.clear();
				tpALifeHuman->m_dwCurNode = u32(-1);
				vfCommunicateWithTrader(tpALifeHuman,tpfGetNearestSuitableTrader(tpALifeHuman));
				tpALifeHuman->m_tTaskState = eTaskStateNone;
			}
			break;
		}
		
		case eTaskStateReturningFailed : {
			if ((tpALifeHuman->m_dwCurNode >= (tpALifeHuman->m_tpaVertices.size() - 1)) && (tpALifeHuman->m_tGraphID == tpALifeHuman->m_tNextGraphID)) {
				tpALifeHuman->m_tpaVertices.clear();
				tpALifeHuman->m_dwCurNode = u32(-1);
				vfCommunicateWithTrader(tpALifeHuman,tpfGetNearestSuitableTrader(tpALifeHuman));
				tpALifeHuman->m_tTaskState = eTaskStateNone;
			}
			break;
		}
		default : NODEFAULT;
	};
	vfChooseNextRoutePoint	(tpALifeHuman);
	vfCheckForTheBattle		(tpALifeHuman);
	vfCheckForItems			(tpALifeHuman);
	vfCheckForDeletedEvents	(tpALifeHuman);
}