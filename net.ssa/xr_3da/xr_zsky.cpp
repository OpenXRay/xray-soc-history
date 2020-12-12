// file: xr_zsky.cpp

#include "stdafx.h"
#include "xr_zsky.h"
#include "xr_creator.h"
#include "x_ray.h"
#include "environment.h"

static SKY_PARAM default_param = {
	0, 32, 12.0f, .0f, 65.f, -20.f
};


CEffectZSky::CEffectZSky( SKY_PARAM *_param )
{
	char fname[MAX_PATH];
	SKY_PARAM zskyparam;

	if( _param == 0 )
		if( Load( &zskyparam, pCreator->pIniLevel, "zsky" ) )
			_param = &zskyparam;

	param		= _param ? (*_param) : default_param;
	VERIFY		( param.segments>=3 );
	VERIFY		( (param.segments%4)==0 );
	VERIFY		( (param.r>0.01f) );
	VERIFY		( (param.hb<param.hs) );
	VERIFY		( (param.hc>param.hs) );

	param.r		= pCreator->Environment.fViewportFar*.98f;

	vertexcount = param.segments + 1;
	vertices	= new FLvertex[vertexcount];
	vertices_d	= new FLvertex[vertexcount];
	vertices_l	= new FLvertex[vertexcount];
	vertices_b	= new FLvertex[vertexcount];
	vertices_b1	= new FLvertex[vertexcount];
	VERIFY		( vertices );
	VERIFY		( vertices_d );
	VERIFY		( vertices_l );
	VERIFY		( vertices_b );
	VERIFY		( vertices_b1 );

	indexcount	= param.segments * 3;
	indices		= (WORD*)malloc( sizeof(WORD)*indexcount );
	indices_b	= (WORD*)malloc( sizeof(WORD)*indexcount );
	VERIFY		( indices );
	VERIFY		( indices_b );

	sx			= 0.3f;
	sz			= 0.3f;

	if( pCreator->Environment.vSunDir.y > 0.01f ){
		sx		= pCreator->Environment.vSunDir.x * 0.5f;
		sz		= pCreator->Environment.vSunDir.z * 0.5f;
	}

	// calculate sky color
	Fcolor		a;
	FPcolor		c1, c2;
	a.set		(pCreator->Environment.Ambient);
	a.a			= 0;
	c1.set		(a.get_ideal());		c1.a		= 127;
	c2.set		(c1);					c2.a		= 255;

	// ---
	vertices[0].set		(0, param.hc, 0, c1, c1, 0.f, 0.f);
	vertices_b[0].set	(0, param.hb, 0, c2, c2, .5f, .5f);
	vertices_b1[0].set	(0, param.hb, 0, c1, c1, 0.f, 0.f);

	for( int i=0; i<param.segments; i++){
		float _a = PI_MUL_2 * i / (float) param.segments;
		float _sin = sinf( _a );
		float _cos = cosf( _a );

		vertices[i+1].set	(param.r * _cos, param.hs, param.r * _sin, c1, c1, _cos * 2.f, _sin * 2.f);
		vertices_b[i+1].set	(vertices[i+1].x, vertices[i+1].y, vertices[i+1].z, c2, c2, (1.f+_cos) * .5f, (1.f+_sin) * .5f);
		vertices_b1[i+1].set(vertices[i+1].x, vertices[i+1].y, vertices[i+1].z, c1, c1, _cos * 2.f, _sin * 2.f);
	}

	for( i=0; i<vertexcount; i++){
		vertices_d[i] = vertices[i];
		vertices_l[i] = vertices[i];
		vertices_d[i].color.set( c2 );
		vertices_d[i].specular.set( c2 );
		vertices_l[i].color.set( c1 );
		vertices_l[i].specular.set( c1 );
	}

	for( i=0; i<param.segments; i++){
		indices[i*3  ] = 0;
		indices[i*3+1] = i+1;
		indices[i*3+2] = (i==(param.segments-1)) ? 1 : (i+2);
	}

	for( i=0; i<param.segments; i++){
		indices_b[i*3  ] = 0;
		indices_b[i*3+2] = i+1;
		indices_b[i*3+1] = (i==(param.segments-1)) ? 1 : (i+2);
	}

	sprintf( fname, "sky\\%04d\\def", param.textureset );
	hTexture = Device.Texture.Create(fname, "std_aop");

	sprintf( fname, "sky\\%04d\\dk", param.textureset );
	hDarkTex = Device.Texture.Create(fname);

	sprintf( fname, "sky\\%04d\\lt", param.textureset );
	hLightTex = Device.Texture.Create(fname, "std_aop");

	sprintf( fname, "sky\\%04d\\earth", param.textureset );
	hTexture_B = Device.Texture.Create(fname);

	sprintf( fname, "sky\\%04d\\defb", param.textureset );
	hTexture_B1 = Device.Texture.Create(fname, "std_aop");
//	hTexture_B1 = Device.Texture.Create("sky\\0001\\dk","flares");
}

CEffectZSky::~CEffectZSky()
{
	if(vertexcount){
		vertexcount =0;
		_DELETEARRAY( vertices );
		_DELETEARRAY( vertices_d );
		_DELETEARRAY( vertices_l );
		_DELETEARRAY( vertices_b );
		_DELETEARRAY( vertices_b1 );
	}

	if(indexcount){
		VERIFY( indices );
		VERIFY( indices_b );
		indexcount =0;
		free( indices );
		free( indices_b );
	}

	Device.Texture.Delete (hTexture);
	Device.Texture.Delete (hDarkTex);
	Device.Texture.Delete (hLightTex);
	Device.Texture.Delete (hTexture_B);
	Device.Texture.Delete (hTexture_B1);
}

void CEffectZSky::OnMove()
{
	VERIFY( vertexcount );
	VERIFY( indexcount );

	for( int i=0; i<vertexcount; i++){
		vertices[i].tu += Device.fTimeDelta * speed[0];
		vertices[i].tv += Device.fTimeDelta * speed[1];

		vertices_d[i].tu = (vertices[i].tu-sx) * 0.985f + sx;
		vertices_d[i].tv = (vertices[i].tv-sz) * 0.985f + sz;
		vertices_l[i].tu = (vertices[i].tu-sx) * 1.03f + sx;
		vertices_l[i].tv = (vertices[i].tv-sz) * 1.03f + sz;

		vertices_b1[i].tu += Device.fTimeDelta * speedb[0];
		vertices_b1[i].tv += Device.fTimeDelta * speedb[1];
	}

	sx += Device.fTimeDelta * speed[0];
	sz += Device.fTimeDelta * speed[1];
}

void CEffectZSky::Render()
{
	VERIFY( vertexcount );
	VERIFY( indexcount );

	// ------------- setup & render
	Fmatrix mTransform;
	mTransform.translate(Device.vCameraPosition);
	CHK_DX( HW.pDevice->SetTransform ( D3DTRANSFORMSTATE_WORLD,	mTransform.d3d() ) );

	if (psENVState&EFF_SKY_FAST) {
		Device.SetMode(vFog);

		// main pass
		Device.Texture.Set(hTexture);
		Device.Draw.TriList(FLVERTEX,	vertices, vertexcount, indices, indexcount);

		// bottom pass 0
		Device.Texture.Set(hTexture_B);
		Device.Draw.TriList(FLVERTEX,	vertices_b, vertexcount, indices_b, indexcount);

	} else {
		Device.SetMode(vFog);

		// lmap pass 0
		Device.Texture.Set (hDarkTex);
		Device.Draw.TriList(FLVERTEX,vertices_d, vertexcount, indices, indexcount);

		// lmap pass 1
		Device.EnableMode(vAlpha);
		Device.Texture.Set (hLightTex);
		Device.Draw.TriList(FLVERTEX,vertices_l, vertexcount, indices, indexcount);

		// main pass
		Device.Texture.Set(hTexture);
		Device.Draw.TriList(FLVERTEX,vertices, vertexcount, indices, indexcount);

		// bottom pass 0
		Device.DisableMode(vAlpha);
		Device.Texture.Set(hTexture_B);
		Device.Draw.TriList(FLVERTEX,vertices_b, vertexcount, indices_b, indexcount);

		// bottom pass 1
		Device.EnableMode(vAlpha);
		Device.Texture.Set(hTexture_B1);
		Device.Draw.TriList(FLVERTEX,vertices_b1, vertexcount, indices_b, indexcount);
	}
}

//----------------

BOOL CEffectZSky::Load( SKY_PARAM *param, CInifile *ini, LPSTR section )
{
	VERIFY( param );
	VERIFY( section );
	VERIFY( pCreator );

	param->textureset	= ini->ReadINT	( section, "texset", default_param.textureset );
	param->segments		= ini->ReadINT	( section, "segments", default_param.segments );
	param->r			= ini->ReadFLOAT( section, "r", default_param.r );
	param->hc			= ini->ReadFLOAT( section, "hc", default_param.hc );
	param->hs			= ini->ReadFLOAT( section, "hs", default_param.hs );
	param->hb			= ini->ReadFLOAT( section, "hb", default_param.hb );

	Update				( );

	return true;
}


void CEffectZSky::Update()
{
	speed[0]			= -pCreator->Environment.fWindStrength * cosf(pCreator->Environment.fWindDir) * 0.0001f;
	speed[1]			= -pCreator->Environment.fWindStrength * sinf(pCreator->Environment.fWindDir) * 0.0001f;
	speedb[0]			= 5.f*speed[0];
	speedb[1]			= 5.f*speed[1];
}
