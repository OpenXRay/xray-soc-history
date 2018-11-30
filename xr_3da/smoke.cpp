#include "stdafx.h"
#include "smoke.h"
#include "xr_ini.h"
#include "xr_sound.h"
#include "Device.h"
#include "mainapp.h"
#include "xr_sndman.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define rnd() ( ( (FLOAT)rand() ) / RAND_MAX )

int __cdecl _CompareSpriteByTime( const void *a, const void *b)
{
	CSmokeUnit *obj_a = *((CSmokeUnit**)a);
	CSmokeUnit *obj_b = *((CSmokeUnit**)b);
	if (obj_a->life_time > obj_b->life_time) return -1;
	if (obj_a->life_time < obj_b->life_time) return 1;
	return 0;
}

static sxr_smoke_data default_sp_data = {
	-1,			// texture
	20,			// sprite_count
	{0,1,0},	// направление
	0.25f,		// start_delay
	0.3f,		// мин. размер спрайтов
	3.5f,		// макс. размер спрайтов
	3.0f,		// life_time
	0.1f,		// fire_smoke
	2.75f,		// начальная скорость вылета частицы
	0.0f,		// степень затухания
	3.2f,		// скорость вращения
	0.15f,		// разброс параметров
	true,		// спрайт
	6,			// спрайт: стартовый кадр
	5,			// спрайт: кол-во кадров анимации
	1,			// спрайт: время анимации
	16.f/64.f	// спрайт: размер одного кадра
};

CSmoke::CSmoke( Fvector &_pos, sxr_smoke_data* _data  )
{
	state				= fsStay;
	srand				( timeGetTime() );

	pos.set				(_pos);
	VERIFY				( _data );

	data				= _data;
	delay				= data->start_delay;

	sprites.SetSize		( data->sprite_count );
	for (int i=0; i<sprites.count; i++ ){
		float	rot		= (data->rot_speed)?((rnd()>.5f)?data->rot_speed + rnd()*data->delta : -(data->rot_speed + rnd()*data->delta)):0.0f;
		sprites[i]		= new CSmokeUnit( data->hTexture, (data->rot_speed>0.0001f)?rnd():(PI/2.f), rot );
		sprites[i]->frame = data->anim_start+int(data->anim_count*rnd());
		if (data->anim)	sprites[i]->SetSAnimator( sprites[i]->frame-1, 1, 1, false, data->anim_tex_size  );
		InitSprite		( i );
	}
}

CSmoke::~CSmoke()
{
	for (int i=0; i<sprites.count; i++ )
		if (sprites[i]) delete sprites[i];
}

void CSmoke::InitSprite	( int i )
{
	float k = (sprites[i]->frame - data->anim_start + 1)/float(data->anim_count);
	k = sqrtf(k);
	sprites[i]->center.set( pos );
	sprites[i]->max_time= k*(rnd()+0.5f)*data->life_time;
	sprites[i]->life_time= sprites[i]->max_time;
	sprites[i]->speed	= data->speed;
	sprites[i]->max_size= data->max_size;
	sprites[i]->state	= ssStay;
	Fvector	d;
	d.x = data->dir.x+2*rnd()*((rnd()>.5f)?data->delta:-data->delta);
	d.y = data->dir.y+rnd()*data->delta;//*((rnd()>.5f)?data->delta:-data->delta);
	d.z = data->dir.z+2*rnd()*((rnd()>.5f)?data->delta:-data->delta);
	d.normalize_safe();
	sprites[i]->dir.set	(d);
}

void CSmoke::Render( )
{
	Fvector			vmove;
	int				live_cnt = 0;

//	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,		D3DBLEND_INVSRCALPHA	));
//	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,	D3DBLEND_ONE));

	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,		D3DBLEND_SRCALPHA	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,	D3DBLEND_INVSRCCOLOR));

//	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,		D3DBLEND_SRCCOLOR));
//	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,	D3DBLEND_ONE));

	delay			-= Device.fTimeDelta;
	for (int i=0; i<sprites.count; i++ ){
		if (sprites[i]->state==ssMove){
			live_cnt++;
			sprites[i]->life_time -= Device.fTimeDelta;
			if (sprites[i]->life_time>0){
				float dt = sprites[i]->life_time/sprites[i]->max_time;
				sprites[i]->alpha	= (1-dt)/data->delta_stay;
				sprites[i]->size	= data->min_size+(sprites[i]->max_size-data->min_size)*(1-dt);//data->min_size+(sprites[i]->max_size-data->min_size)*(1-dt);
				if ((1-dt)>data->delta_stay){
					sprites[i]->alpha	= dt/(1-data->delta_stay);
					vmove.set( sprites[i]->dir );
					vmove.mul( Device.fTimeDelta*sprites[i]->speed*dt );
					sprites[i]->center.add( vmove );
				}
			}else{
				sprites[i]->state=ssStay;
			}
			sprites[i]->RenderUnit	( );
		}
		if ((delay<=0)&&(sprites[i]->state==ssStay)&&(state==fsMove)){
			delay = data->start_delay;
			InitSprite	( i );
			sprites[i]->state=ssMove;
		}
	}

    CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,		D3DBLEND_SRCALPHA	));
    CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,	D3DBLEND_INVSRCALPHA));

	if ((state==fsProcessStay)&&(!live_cnt)) state = fsStay;
}


void CSmoke::LoadTemplate( CXR_Inifile* ini, LPSTR section, sxr_smoke_data* d  )
{
	FILE_NAME		temp_name;

	CopyMemory		( d, &default_sp_data, sizeof(sxr_smoke_data) );

	d->hTexture		= -1;
	if ( ini->ReadTrimString( section, "texture", temp_name))
		d->hTexture	= Device.Texture.Create( temp_name );
	d->sprite_count	= ini->ReadInt	(section, "sprite_count",	d->sprite_count	);
	d->min_size		= ini->ReadFloat(section, "min_size",		d->min_size		);
	d->max_size		= ini->ReadFloat(section, "max_size",		d->max_size		);
	ini->ReadVector					(section, "dir",			&d->dir, d->dir.x, d->dir.y, d->dir.z );
	d->start_delay	= ini->ReadFloat(section, "start_delay",	d->start_delay	);
	d->life_time	= ini->ReadFloat(section, "life_time",		d->life_time	);
	d->delta_stay	= ini->ReadFloat(section, "delta_stay",		d->delta_stay	);
	d->speed		= ini->ReadFloat(section, "speed",			d->speed		);
	d->decay		= ini->ReadFloat(section, "decay",			d->decay		);
	d->rot_speed	= ini->ReadFloat(section, "rot_speed",		d->rot_speed	);
	d->delta		= ini->ReadFloat(section, "delta",			d->delta		);
	ini->ReadTokens2				(section, "anim",			BOOL_token, &d->anim );
	d->anim_start	= ini->ReadInt	(section, "anim_start",		d->anim_start	);
	d->anim_count	= ini->ReadInt	(section, "anim_count",		d->anim_count	);
	d->anim_time	= ini->ReadFloat(section, "anim_time",		d->anim_time	);
	d->anim_tex_size= ini->ReadFloat(section, "anim_tex_size",	d->anim_tex_size);
}

void CSmoke::UnloadTemplate( sxr_smoke_data* data )
{
	Device.Texture.Delete( data->hTexture );
}
