#include "stdafx.h"
#include "poltergeist.h"
#include "../../../PhysicsShell.h"

#define IMPULSE			2.f
#define TRACE_DISTANCE  10.f

void CPoltergeist::PhysicalImpulse(const Fvector &position)
{
	Level().ObjectSpace.GetNearest(position, 10.f); 
	xr_vector<CObject*> &tpObjects = Level().ObjectSpace.q_nearest;

	if (tpObjects.empty()) return;
	
	u32 index = Random.randI(tpObjects.size());
	
	CPhysicsShellHolder  *obj = dynamic_cast<CPhysicsShellHolder *>(tpObjects[index]);
	if (!obj || !obj->m_pPhysicsShell) return;

	Fvector dir;
	dir.sub(obj->Position(), position);
	dir.normalize();
	obj->m_pPhysicsShell->applyImpulse(dir,IMPULSE * obj->m_pPhysicsShell->getMass());
}

void CPoltergeist::StrangeSounds(const Fvector &position)
{
	if (m_strange_sound.feedback) return;
	
	Fvector dir;
	dir.random_dir();

	Collide::rq_result	l_rq;
	if (Level().ObjectSpace.RayPick(position, dir, TRACE_DISTANCE, Collide::rqtStatic, l_rq)) {
		if (l_rq.range < TRACE_DISTANCE) {
			
			// �������� ���� ����������
			CDB::TRI*	pTri	= Level().ObjectSpace.GetStaticTris() + l_rq.element;
			SGameMtlPair* mtl_pair = GMLib.GetMaterialPair(CMaterialManager::self_material_idx(),pTri->material);
			if (!mtl_pair) return;

			// ������ ����
			if (!mtl_pair->CollideSounds.empty()) {
				SELECT_RANDOM(m_strange_sound, mtl_pair, CollideSounds);
				
				Fvector pos;
				pos.mad(position, dir, ((l_rq.range - 0.1f > 0) ? l_rq.range - 0.1f  : l_rq.range));
				m_strange_sound.play_at_pos(this,pos);
			}			


		}
	}
}

