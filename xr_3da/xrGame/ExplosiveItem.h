//////////////////////////////////////////////////////////////////////
// ExplosiveItem.h: ����� ��� ���� ������� ���������� ��� 
//					��������� ��������� ����� (��������,
//					������ � ����� � �.�.)
//////////////////////////////////////////////////////////////////////

#pragma once

#include "Explosive.h"

class CExplosiveItem: 
	virtual public CInventoryItem,
			public CExplosive
{
private:
	typedef CInventoryItem inherited;
public:
	CExplosiveItem(void);
	virtual ~CExplosiveItem(void);

	virtual void				Load				(LPCSTR section);
	virtual BOOL				net_Spawn			(CSE_Abstract* DC)			{return CInventoryItem::net_Spawn(DC);}
	virtual void				net_Destroy			();
	virtual void				net_Export			(NET_Packet& P)		{CInventoryItem::net_Export(P);}
	virtual void				net_Import			(NET_Packet& P)		{CInventoryItem::net_Import(P);}
	virtual CGameObject			*cast_game_object	()					{return this;}

	virtual void OnEvent		(NET_Packet& P, u16 type);
	virtual	void Hit			(float P, Fvector &dir,	CObject* who, s16 element,
										  Fvector position_in_object_space, float impulse, 
										  ALife::EHitType hit_type = ALife::eHitTypeWound);
	
	virtual void UpdateCL();
	virtual void renderable_Render(); 
};