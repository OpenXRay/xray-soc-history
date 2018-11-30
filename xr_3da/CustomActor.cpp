// CustomActor.cpp: implementation of the CCustomActor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomActor.h"
#include "xr_creator.h"
#include "bodyinstance.h"
#include "x_ray.h"
#include "xr_smallfont.h"
#include "networkclient.h"
#include "net_messages.h"
#include "std_classes.h"
#include "weapon.h"
#include "xr_mac.h"
#include "cameramanager.h"
#include "xr_area.h"
#include "xr_trims.h"
#include "xr_tokens.h"
#include "xr_sndman.h"
#include "objects\customitem.h"
#include "fmesh.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCustomActor::CCustomActor(void *p) : inherited()
{
	Weapon				= 0;
	eState				= psStand;

//	Device.seqRender.Add(this,REG_PRIORITY_LOW-1111);
}

CCustomActor::~CCustomActor()
{
//	Device.seqRender.Remove(this);
}

void CCustomActor::Respawn()
{
	eState				= psStand;
	// game
	iHealth				= 100;
	iArmor				= 0;

	Weapon->SetDefaults();
}

void CCustomActor::Load(CInifile* ini, const char * section)
{
	Msg("Loading actor: %s",section);

	inherited::Load(ini,section);

	R_ASSERT	(pVisual->Type==MT_SKELETON);
	PKinematics(pVisual)->LL_PlayCycle(
		-1,
		PKinematics(pVisual)->LL_MotionID("idle"),
		1,1,1
		);

	Weapon		= (CWeapon *)pCreator->Objects.LoadOne(pSettings,"rail_gun");
	R_ASSERT	(Weapon);
	Weapon->SetParent(this);

	// sounds
	sndStep				= pSounds->Create3D((string(section)+"\\step").c_str());
	sndWeaponChange		= pSounds->Create3D((string(section)+"\\weaponchange").c_str());
	sndHit_20			= pSounds->Create3D((string(section)+"\\hit20").c_str());
	sndHit_50			= pSounds->Create3D((string(section)+"\\hit50").c_str());
	sndHit_99			= pSounds->Create3D((string(section)+"\\hit99").c_str());
	sndDie				= pSounds->Create3D((string(section)+"\\die").c_str());
	sndDie				= pSounds->Create3D((string(section)+"\\respawn").c_str());
}

BOOL	CCustomActor::TakeItem		(DWORD CID)
{
	CCustomItem* O = (CCustomItem*) pCreator->Objects.GetObjectByCID(CID);
	int iValue = O->iValue;
	switch (O->clsid_target) {
	case CLSID_OBJECT_W_RAIL:
	case CLSID_OBJECT_A_RAIL:
		Weapon->AddAmmo(iValue);
		break;
	case CLSID_OBJECT_HEALTH:
		if (iHealth<iMAX_Health){
			iHealth += iValue;
			if (iHealth>iMAX_Health) iHealth = iMAX_Health;
		}else return false;
		break;
	case CLSID_OBJECT_ARMOR:
		if (iArmor<iMAX_Armor){
			iArmor += iValue;
			if (iArmor>iMAX_Armor) iArmor = iMAX_Armor;
		}else return false;
		break;
	default:
		return false;
	}
	return true;
}

void CCustomActor::FireStart()
{
	Weapon->FireStart();
}

void CCustomActor::FireEnd()
{
	Weapon->FireEnd();
}

void CCustomActor::Die()
{
	// Play sound
	pSounds->Play3DAtPos(sndDie,vPosition);
}

void CCustomActor::HitAnother(CEntity * pActor, int Power, Fvector &dir)
{
	pActor->Hit(Power, dir);
}

void CCustomActor::HitSound(int iHitAmount)
{
	// Play hit-sound
	int iSelectedSound;
	if (iHitAmount<=20) iSelectedSound = sndHit_20;
	else if (iHitAmount<=50) iSelectedSound = sndHit_50;
	else iSelectedSound = sndHit_99;
	pSounds->Play3DAtPos(iSelectedSound,vPosition);
}

void CCustomActor::OnRender()
{
//	Fmatrix mScale,X; mScale.scale(15,15,15);
//	X.mul(mTransform,mScale);
	PKinematics(pVisual)->DebugRender(mTransform);
}
