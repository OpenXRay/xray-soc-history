// file: xr_zexplosion.cpp

#include "stdafx.h"
#include "xr_zexplosion.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "flightscontroller.h"
#include "fstaticrender.h"

static BE_INIT_INFO default_be_param = {

	100,   20,          // counts
	0.25f, 0.05f,       // sizes

	1.6f, 0.8f,        // accels
	1.0f, 0.055f,	   // speeds
	0.8f, 0.9f,		   // accel limits

	0.9f, 0.3f,        // rotation accels
	3.f,  0.5f,	       // rotation speeds
	0.6f, 0.9f,		   // rotation accel limits

	1.2f, 2.7f,		   // scale attack & decay
	2.f, 0.1f, 2.f,	   // scale of direction
	0.8f,		       // maxalpha
	0.05f,	           // startup radius
	2.f,
	0.f, 0.f, 0.f,

	-1,                 // texture
};

#define MAKERGBA (_a)\
	( _a + (_a<<8) + (_a<<16) )

CXR_PSB_Explosion::CXR_PSB_Explosion( Fvector& where, BE_INIT_INFO *_param )
{
	state = EFF_STATE_START;
	type = EFFECT_EXPLOSION_PARTICLE;

	if( 0==_param )
		_param = &default_be_param;

	bHasAlpha = TRUE;
	//bHasAlpha = _param->hTexture ? _param->texture->bHasAlpha:FALSE;

	stage = PSB_VALID;

	memset( &light, 0, sizeof(light) );
	light.type=D3DLIGHT_POINT;
	light.ambient.set(1,1,1,1);
	light.diffuse.set(1,1,1,1);
	light.position.set(where);
	light.attenuation0=0;
	light.attenuation1=5;
	light.attenuation2=3;
	light.range=_param->max_radius*2.f;
	pRender->Lights.Add(light);

	r = _param->max_radius;
	center = where;
	hTexture = _param->hTexture;
	axis.set( 0, 1, 0 );
	maxalpha = _param->maxalpha;
	accelstop = _param->accelstop;
	accelstart = _param->accelstart;
	raccelstop = _param->raccelstop;
	raccelstart = _param->raccelstart;
	particles.SetSize( _param->pcount_from + (rand()%_param->pcount_difference) );
	buffer_f.SetSize( particles.count * 12 );
	render_faces = 0;

	s_attack = _param->s_attack;
	s_decay = _param->s_decay;

	int vfill = 0;

	for( int i=0; i<particles.count; i++){

		particles[i].size = _param->psize_from + _param->psize_difference * rand() / (float)RAND_MAX;
		particles[i].accel = _param->paccel_from + _param->paccel_difference * rand() / (float)RAND_MAX;
		particles[i].speed = _param->pspeed_from + _param->pspeed_difference * rand() / (float)RAND_MAX;
		particles[i].raccel = _param->praccel_from + _param->praccel_difference * rand() / (float)RAND_MAX;
		particles[i].rspeed = _param->prspeed_from + _param->prspeed_difference * rand() / (float)RAND_MAX;
		particles[i].direction.x = (float)( RAND_MAX/2 - rand() ) * _param->sx;
		particles[i].direction.y = (float)( RAND_MAX/2 - rand() ) * _param->sy;
		particles[i].direction.z = (float)( RAND_MAX/2 - rand() ) * _param->sz;
		particles[i].direction.normalize();
		particles[i].direction.x += _param->az;
		particles[i].direction.y += _param->ay;
		particles[i].direction.z += _param->az;
		particles[i].direction.normalize();
		particles[i].center = particles[i].direction;
		particles[i].center.mul( _param->gen_radius * rand() / (float)RAND_MAX );
		particles[i].center.add( where );
		particles[i].svcenter = particles[i].center;
		particles[i].scale = 0;

		particles[i].stage =
			ESTAGE_VALID | ESTAGE_SCALE_UP |
			ESTAGE_ACCEL_ENABLED | ESTAGE_RACCEL_ENABLED;
	}
}

CXR_PSB_Explosion::~CXR_PSB_Explosion()
{
//	Device.Lights.RemoveDynamic(light);
}

void CXR_PSB_Explosion::Render()
{

	if( !(stage&PSB_VALID) )
		return;

	VERIFY( HW.pDevice );

	Device.SetMode		(vAlpha | vZTest);
	Device.Texture.Set	(hTexture);
	Draw();

}

void CXR_PSB_Explosion::OnMove()
{
	Fvector vmove;
	Fvector vrotate;
	int valid_p,i;

	if( !(stage&PSB_VALID) )
		return;

	for( i=0, valid_p = 0; i<particles.count; i++){

		if( !(particles[i].stage&ESTAGE_VALID) )
			continue;

		particles[i].svcenter = particles[i].center;
		valid_p++;

		if( (particles[i].stage&ESTAGE_ACCEL_ENABLED) )
			particles[i].speed += particles[i].accel * Device.fTimeDelta;

		if( (particles[i].stage&ESTAGE_RACCEL_ENABLED) )
			particles[i].rspeed += particles[i].raccel * Device.fTimeDelta;

		vrotate.set( axis );
		vrotate.mul( Device.fTimeDelta * particles[i].rspeed );
		vmove.crossproduct( particles[i].direction, vrotate );
		particles[i].direction.add( vmove );
		particles[i].center.add( vmove );

		// for better quality (but this is odd at all)
		particles[i].direction.normalize();

		vmove.set( particles[i].direction );
		vmove.mul( Device.fTimeDelta*particles[i].speed );
		particles[i].center.add( vmove );

		if( (particles[i].stage&ESTAGE_SCALE_UP) ){
			particles[i].scale += s_attack * Device.fTimeDelta;
			if( particles[i].scale >= 1.f ){
				particles[i].scale = 1.f;
				particles[i].stage &= ~ESTAGE_SCALE_UP;
			}
			if( particles[i].scale >= accelstop )
				particles[i].stage &= ~ESTAGE_ACCEL_ENABLED;
			if( particles[i].scale >= raccelstop )
				particles[i].stage &= ~ESTAGE_RACCEL_ENABLED;
		}

		else if( (particles[i].stage&ESTAGE_SCALE_DOWN) ){
			particles[i].scale -= s_decay * Device.fTimeDelta;
			if( particles[i].scale <= 0.f ){
				particles[i].scale = 0.f;
				particles[i].stage &= ~ESTAGE_SCALE_DOWN;
				particles[i].stage &= ~ESTAGE_VALID;
			}
			if( particles[i].scale < accelstart )
				particles[i].stage |= ESTAGE_ACCEL_ENABLED;
			if( particles[i].scale < raccelstart )
				particles[i].stage |= ESTAGE_RACCEL_ENABLED;
		}

		else {
			if( particles[i].center.distance_to(center) >= r )
				particles[i].stage |= ESTAGE_SCALE_DOWN;
		}
	}

	if( !valid_p ){
		state = EFF_STATE_STOP;
		stage &= ~PSB_VALID;
	}

}

void CXR_PSB_Explosion::Draw()
{
	int vfill		 = 0;
	int render_faces = 0;

	FTLvertex P;
	float cx,cy,sz;

	FTLvertex *v = Device.VB.TLB_Start	();

	for( int i=0; i<particles.count; i++){
		if( !( (particles[i].stage&ESTAGE_VALID) && particles[i].scale>=0.01f ) )
			continue;

		float ksize = particles[i].size*particles[i].scale;
		BYTE k_blend = (particles[i].scale > maxalpha) ?
			( (BYTE)( 255 * maxalpha ) ) :
			( (BYTE)( 255 * particles[i].scale ) );

		FPcolor color = {k_blend,k_blend,k_blend,0};

		for( int k=0; k<2; k++){
			buffer_f[render_faces++] = vfill   + k*4;
			buffer_f[render_faces++] = vfill+2 + k*4;
			buffer_f[render_faces++] = vfill+1 + k*4;
			buffer_f[render_faces++] = vfill   + k*4;
			buffer_f[render_faces++] = vfill+2 + k*4;
			buffer_f[render_faces++] = vfill+3 + k*4;
		}
		vfill				+= 8;

		// main particle
		P.s_transform		( particles[i].center, Device.mFullTransform );

		cx					= X_TO_REAL(P.sx);
		cy					= Y_TO_REAL(P.sy);
		sz					= ksize / P.rhw * Device.dwWidth;

		v->set	(cx - sz,cy + sz,P.sz,P.rhw,color,color,0,0); v++;
		v->set	(cx + sz,cy + sz,P.sz,P.rhw,color,color,1,0); v++;
		v->set	(cx + sz,cy - sz,P.sz,P.rhw,color,color,1,1); v++;
		v->set	(cx - sz,cy - sz,P.sz,P.rhw,color,color,0,1); v++;

		// shadow particle
		P.s_transform		( particles[i].svcenter, Device.mFullTransform );

		cx					= X_TO_REAL(P.sx);
		cy					= Y_TO_REAL(P.sy);
		sz					= 0.6f*ksize / P.rhw * Device.dwWidth;

		v->set	(cx - sz,cy + sz,P.sz,P.rhw,color,color,0,0); v++;
		v->set	(cx + sz,cy + sz,P.sz,P.rhw,color,color,1,0); v++;
		v->set	(cx + sz,cy - sz,P.sz,P.rhw,color,color,1,1); v++;
		v->set	(cx - sz,cy - sz,P.sz,P.rhw,color,color,0,1); v++;
	}

	Device.VB.TLB_EndIndexed(v,&buffer_f[0],render_faces);

	if (!render_faces) {
		stage &= ~PSB_VALID;
		state = EFF_STATE_STOP;
	}

}

//-------------------------

void CXR_PSB_Explosion::LoadTemplate( CInifile* ini, char *section, BE_INIT_INFO *i )
{
	char temp_tex_name[_MAX_PATH];
	VERIFY( i );
	VERIFY( section );
	VERIFY( ini );

	i->pcount_from = ini->ReadInt( section, "pcount_from", default_be_param.pcount_from );
	i->pcount_difference = ini->ReadInt( section, "pcount_difference", default_be_param.pcount_difference );
	i->psize_from = ini->ReadFloat( section, "psize_from", default_be_param.psize_from );
	i->psize_difference = ini->ReadFloat( section, "psize_difference", default_be_param.psize_difference );

	i->paccel_from = ini->ReadFloat( section, "paccel_from", default_be_param.paccel_from );
	i->paccel_difference = ini->ReadFloat( section, "paccel_difference", default_be_param.paccel_difference );
	i->pspeed_from = ini->ReadFloat( section, "pspeed_from", default_be_param.pspeed_from );
	i->pspeed_difference = ini->ReadFloat( section, "pspeed_difference", default_be_param.pspeed_difference );
	i->accelstop = ini->ReadFloat( section, "accelstop", default_be_param.accelstop );
	i->accelstart = ini->ReadFloat( section, "accelstart", default_be_param.accelstart );

	i->praccel_from = ini->ReadFloat( section, "praccel_from", default_be_param.praccel_from );
	i->praccel_difference = ini->ReadFloat( section, "praccel_difference", default_be_param.praccel_difference );
	i->prspeed_from = ini->ReadFloat( section, "prspeed_from", default_be_param.prspeed_from );
	i->prspeed_difference = ini->ReadFloat( section, "prspeed_difference", default_be_param.prspeed_difference );
	i->raccelstop = ini->ReadFloat( section, "raccelstop", default_be_param.raccelstop );
	i->raccelstart = ini->ReadFloat( section, "raccelstart", default_be_param.raccelstart );

	i->s_attack = ini->ReadFloat( section, "s_attack", default_be_param.s_attack );
	i->s_decay = ini->ReadFloat( section, "s_decay", default_be_param.s_decay );
	i->sx = ini->ReadFloat( section, "sx", default_be_param.sx );
	i->sy = ini->ReadFloat( section, "sy", default_be_param.sy );
	i->sz = ini->ReadFloat( section, "sz", default_be_param.sz );
	i->maxalpha = ini->ReadFloat( section, "maxalpha", default_be_param.maxalpha );
	i->gen_radius = ini->ReadFloat( section, "gen_radius", default_be_param.gen_radius );
	i->max_radius = ini->ReadFloat( section, "max_radius", default_be_param.max_radius );
	i->ax = ini->ReadFloat( section, "ax", default_be_param.ax );
	i->ay = ini->ReadFloat( section, "ay", default_be_param.ay );
	i->az = ini->ReadFloat( section, "az", default_be_param.az );

	i->hTexture = ini->ReadString( section, "texture", temp_tex_name)?Device.Texture.Create( temp_tex_name, "vengine" ):-1;

	VERIFY( i->pcount_from > 0 );
	VERIFY( i->psize_from > 0.01f );

	VERIFY( i->s_attack > 0.01f );
	VERIFY( i->s_decay > 0.01f );
	VERIFY( i->maxalpha > 0.01f );

	if( i->hTexture==-1 )
		Msg( "! CXR_PSB_Explosion::LoadTemplate(): failed to load texture '%s'\n", temp_tex_name );
}

void CXR_PSB_Explosion::UnloadTemplate( BE_INIT_INFO *i )
{
	VERIFY( i );
	Device.Texture.Delete (i->hTexture);
	i->hTexture = -1;
}
