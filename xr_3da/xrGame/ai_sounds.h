////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_sounds.h
//	Created 	: 15.08.2002
//  Modified 	: 15.08.2002
//	Author		: Dmitriy Iassenev
//	Description : Sounds for AI personalities
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_SOUNDS__
#define __XRAY_AI_SOUNDS__

#define DECLARE_WEAPON_SOUNDS(A,B)\
	SOUND_TYPE_WEAPON_##A = B,\
	SOUND_TYPE_WEAPON_RECHARGING_##A		= SOUND_TYPE_WEAPON_RECHARGING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_SHOOTING_##A			= SOUND_TYPE_WEAPON_SHOOTING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_TAKING_##A			= SOUND_TYPE_WEAPON_TAKING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_HIDING_##A			= SOUND_TYPE_WEAPON_HIDING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_CHANGING_##A			= SOUND_TYPE_WEAPON_CHANGING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_EMPTY_CLICKING_##A	= SOUND_TYPE_WEAPON_EMPTY_CLICKING | SOUND_TYPE_WEAPON_##A,\
	SOUND_TYPE_WEAPON_BULLET_RICOCHET_##A	= SOUND_TYPE_WEAPON_BULLET_RICOCHET | SOUND_TYPE_WEAPON_##A,

#define DECLARE_MONSTER_SOUNDS(A,B)\
	SOUND_TYPE_MONSTER_##A					= B,\
	SOUND_TYPE_MONSTER_DYING##A				= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_DYING,\
	SOUND_TYPE_MONSTER_INJURING##A			= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_INJURING,\
	SOUND_TYPE_MONSTER_WALKING_NORMAL##A	= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_WALKING_NORMAL,\
	SOUND_TYPE_MONSTER_WALKING_CROUCH##A	= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_WALKING_CROUCH,\
	SOUND_TYPE_MONSTER_WALKING_LIE##A		= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_WALKING_LIE,\
	SOUND_TYPE_MONSTER_RUNNING_NORMAL##A	= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_RUNNING_NORMAL,\
	SOUND_TYPE_MONSTER_RUNNING_CROUCH##A	= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_RUNNING_CROUCH,\
	SOUND_TYPE_MONSTER_RUNNING_LIE##A		= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_RUNNING_LIE,\
	SOUND_TYPE_MONSTER_JUMPING_NORMAL##A	= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_JUMPING_NORMAL,\
	SOUND_TYPE_MONSTER_JUMPING_CROUCH##A	= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_JUMPING_CROUCH,\
	SOUND_TYPE_MONSTER_FALLING##A			= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_FALLING,\
	SOUND_TYPE_MONSTER_TALKING##A			= SOUND_TYPE_MONSTER_##A | SOUND_TYPE_MONSTER_TALKING,


enum ESoundTypes {
	SOUND_TYPE_WEAPON					= 0x80000000,
	SOUND_TYPE_MONSTER					= 0x40000000,
	SOUND_TYPE_WORLD					= 0x20000000,
	
	SOUND_TYPE_WEAPON_RECHARGING		= SOUND_TYPE_WEAPON | 0x10000000,
	SOUND_TYPE_WEAPON_SHOOTING			= SOUND_TYPE_WEAPON | 0x08000000,
	SOUND_TYPE_WEAPON_TAKING			= SOUND_TYPE_WEAPON | 0x04000000,
	SOUND_TYPE_WEAPON_HIDING			= SOUND_TYPE_WEAPON | 0x02000000,
	SOUND_TYPE_WEAPON_CHANGING			= SOUND_TYPE_WEAPON | 0x01000000,
	SOUND_TYPE_WEAPON_EMPTY_CLICKING	= SOUND_TYPE_WEAPON | 0x00800000,
	SOUND_TYPE_WEAPON_BULLET_RICOCHET	= SOUND_TYPE_WEAPON | 0x00400000,

	SOUND_TYPE_MONSTER_DYING			= SOUND_TYPE_MONSTER | 0x10000000,
	SOUND_TYPE_MONSTER_INJURING			= SOUND_TYPE_MONSTER | 0x08000000,
	SOUND_TYPE_MONSTER_WALKING_NORMAL	= SOUND_TYPE_MONSTER | 0x04000000,
	SOUND_TYPE_MONSTER_WALKING_CROUCH	= SOUND_TYPE_MONSTER | 0x02000000,
	SOUND_TYPE_MONSTER_WALKING_LIE		= SOUND_TYPE_MONSTER | 0x01000000,
	SOUND_TYPE_MONSTER_RUNNING_NORMAL	= SOUND_TYPE_MONSTER | 0x00800000,
	SOUND_TYPE_MONSTER_RUNNING_CROUCH	= SOUND_TYPE_MONSTER | 0x00400000,
	SOUND_TYPE_MONSTER_RUNNING_LIE		= SOUND_TYPE_MONSTER | 0x00200000,
	SOUND_TYPE_MONSTER_JUMPING_NORMAL	= SOUND_TYPE_MONSTER | 0x00100000,
	SOUND_TYPE_MONSTER_JUMPING_CROUCH	= SOUND_TYPE_MONSTER | 0x00080000,
	SOUND_TYPE_MONSTER_FALLING			= SOUND_TYPE_MONSTER | 0x00040000,
	SOUND_TYPE_MONSTER_TALKING			= SOUND_TYPE_MONSTER | 0x00020000,
	
	SOUND_TYPE_WORLD_DOOR_OPENING		= SOUND_TYPE_WORLD | 0x10000000,
	SOUND_TYPE_WORLD_DOOR_CLOSING		= SOUND_TYPE_WORLD | 0x08000000,
	SOUND_TYPE_WORLD_WINDOW_BREAKING	= SOUND_TYPE_WORLD | 0x04000000,
	SOUND_TYPE_WORLD_OBJECT_FALLING		= SOUND_TYPE_WORLD | 0x02000000,

	DECLARE_WEAPON_SOUNDS(PISTOL			,0x00000001)
	DECLARE_WEAPON_SOUNDS(GUN				,0x00000002)
	DECLARE_WEAPON_SOUNDS(SUBMACHINEGUN		,0x00000004)
	DECLARE_WEAPON_SOUNDS(MACHINEGUN		,0x00000008)
	DECLARE_WEAPON_SOUNDS(SNIPERRIFLE		,0x00000010)
	DECLARE_WEAPON_SOUNDS(GRENADELAUNCHER	,0x00000020)
	DECLARE_WEAPON_SOUNDS(ROCKETLAUNCHER	,0x00000040)
	DECLARE_WEAPON_SOUNDS(RAILGUN			,0x00000080)
	
	DECLARE_MONSTER_SOUNDS(HUMAN			,0x00000001)
//				grenade
//	SOUND_TYPE_WEAPON_GRENADE_CHANGE_WEAPON,
//	SOUND_TYPE_WEAPON_GRENADE_SHOT,
//	SOUND_TYPE_WORLD_GRENADE_FALLING,
};

#endif
