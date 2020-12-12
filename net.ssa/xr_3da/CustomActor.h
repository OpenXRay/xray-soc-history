// CustomActor.h: interface for the CCustomActor class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "entity.h"
#include "weapon.h"

enum	ECameraStyle;

// networking
#define A_NET_EXPORTTIME	0.2f	// 5 times per second
struct  A_NET_export {
	Fvector		vPos;	// current position
	Fvector		vDir;	// current direction of movement combined with speed
	float		yaw;
	DWORD		MState;
};

class ENGINE_API CCustomActor : public CEntity, public pureRender
{
protected:
	enum EPlayerState{
		psStand,
		psCrouch,
		psDead
	};
	
	enum EMoveCommand{
		mcFwd		= 0x0001,
		mcBack		= 0x0002,
		mcLStrafe	= 0x0004,
		mcRStrafe	= 0x0008,
		mcJump		= 0x0010,
		mcCrouch	= 0x0020,
		mcAccel		= 0x0100
	};
	
private:
	typedef CEntity		inherited;
protected:
	// game media
	int					sndStep;
	int					sndWeaponChange;
	int					sndHit_20;
	int					sndHit_50;
	int					sndHit_99;
	int					sndDie;
	int					sndRespawn;

	// weapons
	CWeapon*			Weapon;

	// player state
	EPlayerState		eState;
public:
						CCustomActor	(void *p);
	virtual				~CCustomActor	( );
	virtual void		Load			(CInifile* ini, const char* section);

	// information
	const char *		GetWeaponName	(void)	{ return "RailGun"; }
	int					GetWeaponAmmo	(void)	{ return Weapon->iAmmoElapsed;	}
	int					GetArmor		( )		{ return iArmor; }
	int					GetHealth		( )		{ return iHealth; }

	virtual void		Die				( );
	virtual void		HitSound		( int iHitAmount );

	virtual void		HitAnother		(CEntity* pActor, int Power, Fvector &dir );
	virtual	void		Respawn			( );

	virtual void		FireStart		( );
	virtual void		FireEnd			( );
	virtual BOOL		TakeItem		( DWORD CID );

	virtual void		NetworkExport	( )	= 0;
	virtual void		OnNetworkMessage( )	= 0;

	virtual void		OnRender		();
};

