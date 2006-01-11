//////////////////////////////////////////////////////////////////////
// ExplosiveItem.cpp:	����� ��� ���� ������� ���������� ��� 
//						��������� ��������� ����� (��������,
//						������ � ����� � �.�.)
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ExplosiveItem.h"


CExplosiveItem::CExplosiveItem(void)
{
}
CExplosiveItem::~CExplosiveItem(void)
{
}

void CExplosiveItem::Load(LPCSTR section)
{
	inherited::Load							(section);
	CExplosive::Load						(section);
	m_flags.set								(FUsingCondition, TRUE);
	CDelayedActionFuse::Initialize			(pSettings->r_float(section,"time_to_explode"),pSettings->r_float(section,"condition_to_explode"));
	VERIFY(pSettings->line_exist			(section,"set_timer_particles"));
}

void CExplosiveItem::net_Destroy()
{
	inherited::net_Destroy();
	CExplosive::net_Destroy();
}

//void CExplosiveItem::Hit(float P, Fvector &dir,	CObject* who, s16 element,
//						Fvector position_in_object_space, float impulse, 
//						ALife::EHitType hit_type)
void	CExplosiveItem::Hit					(SHit* pHDS)
{
//	inherited::Hit(P,dir,who,element,position_in_object_space,impulse,hit_type);
	if(CDelayedActionFuse::isActive())pHDS->power=0.f;
	inherited::Hit(pHDS);
	if(!CDelayedActionFuse::isActive()&&CDelayedActionFuse::CheckCondition(GetCondition())&&CExplosive::Initiator()==u16(-1))
	{
		//��������� ����, ��� ������� ����
		SetInitiator( pHDS->who->ID());

	}
}
void	CExplosiveItem::StartTimerEffects	()
{
	CParticlesPlayer::StartParticles(pSettings->r_string(*cNameSect(),"set_timer_particles"),Fvector().set(0,1,0),ID());

}
void  CExplosiveItem::OnEvent (NET_Packet& P, u16 type)
{
	CExplosive::OnEvent (P, type);
	inherited::OnEvent (P, type);

}
void CExplosiveItem::UpdateCL()
{
	CExplosive::UpdateCL();
	inherited::UpdateCL();
}
void CExplosiveItem::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
	if(CDelayedActionFuse::isActive()&&CDelayedActionFuse::Update(GetCondition()))
	{
		Fvector normal;
		FindNormal(normal);
		CExplosive::GenExplodeEvent(Position(), normal);
		CParticlesPlayer::StopParticles(ID());
	}
}
void CExplosiveItem::renderable_Render()
{
	inherited::renderable_Render();
}
void CExplosiveItem::net_Relcase(CObject* O )
{
	CExplosive::net_Relcase(O);
	inherited::net_Relcase(O);
}