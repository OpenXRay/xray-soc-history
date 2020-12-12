#include "stdafx.h"
#include "xr_effect.h"
#include "xr_creator.h"

CEffectBase::CEffectBase( )
{
	state			= EFF_STATE_STOP;
	type			= EFFECT_NONE;
	bHasAlpha		= FALSE;

	VERIFY			( pCreator );
	parent			= &pCreator->Effects;
}

CEffectBase::~CEffectBase()
{
}
