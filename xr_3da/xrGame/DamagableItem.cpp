#include "stdafx.h"
#include "DamagableItem.h"

CDamagableItem::CDamagableItem()
{
	m_max_health=0.f;
	m_levels_num=u16(-1);
	m_level_applied=u16(-1);
}



u16 CDamagableItem::DamageLevel()
{
float health=Health();if(health<0.f)health=0.f;
return	u16(((m_max_health-Health())*m_levels_num)/m_max_health);
}

void CDamagableItem::Init(float max_health,u16 level_num)
{
	m_max_health=max_health;
	m_level_applied=0;
	m_levels_num=level_num;
}

void CDamagableItem::HitEffect()
{
	u16 new_lewel=DamageLevel();
	if(new_lewel>m_level_applied)ApplyDamage(new_lewel);
}
void CDamagableItem::ApplyDamage(u16 level)
{
	m_level_applied=level;
}

void CDamagableHealthItem::Init(float max_health,u16 level_num)
{
	inherited::Init(max_health,level_num);
	m_health=max_health;
}

void CDamagableHealthItem::Hit(float P)
{
	if(m_level_applied==m_levels_num) return;
	m_health-=P;
	if(m_health<0.f)m_health=0.f;
	HitEffect();
}

void CDamagableItem::RestoreEffect()
{
u16 dl=DamageLevel();
for(u16 i=1;i<=dl;i++)ApplyDamage(i);
}