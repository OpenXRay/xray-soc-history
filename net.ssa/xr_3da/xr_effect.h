#ifndef __XR_EFFECT_H__
#define __XR_EFFECT_H__

typedef enum EFFECT_STATE{
	EFF_STATE_START,
	EFF_STATE_STOP
} EFFECT_STATE;

typedef enum EFFECT_TYPE{
	EFFECT_NONE,
	EFFECT_EXPLOSION_SPRITE,
	EFFECT_EXPLOSION_PARTICLE,
	EFFECT_TELEPORT,
	EFFECT_STEP_TRAIL,
} EFFECT_TYPE;

class						CEffectManager;

class CEffectBase
{
	friend class			CEffectManager;
	friend class			CSpriteParticle;
	friend class			CXR_PSB_Explosion;
	friend class			CXR_ZTeleporter;
	friend class			CXR_PSB_Lightning;

	EFFECT_STATE			state;
	EFFECT_TYPE				type;

	BOOL					bHasAlpha;

	CEffectManager*			parent;

public:
							CEffectBase		( );
	virtual					~CEffectBase		( );

	virtual	void			Render			( ) {;}
	virtual	void			OnMove			( ) {;}
};

#endif //__XR_EFFECT_H__
