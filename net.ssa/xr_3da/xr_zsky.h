// file: xr_zsky.h

#ifndef __INCDEF_XR_ZSKY_H__
#define __INCDEF_XR_ZSKY_H__

struct SKY_PARAM
{
	int textureset;
	int segments;
	float hc,hs,r,hb;
};

class CEffectZSky
{
	SKY_PARAM param;

	int				vertexcount;
	FLvertex		*vertices;
	FLvertex		*vertices_d;
	FLvertex		*vertices_l;
	FLvertex		*vertices_b;
	FLvertex		*vertices_b1;

	int				indexcount;
	WORD			*indices;
	WORD			*indices_b;

	float			speed[2];
	float			speedb[2];

	float			sx, sz;

	int				hTexture;
	int				hDarkTex;
	int				hLightTex;

	int				hTexture_B;
	int				hTexture_B1;

public:
					CEffectZSky( SKY_PARAM *_param = 0 );
	virtual			~CEffectZSky();
	BOOL			Load( SKY_PARAM *param, CInifile *ini, LPSTR section = "zsky" );

	void __fastcall OnMove( );
	void __fastcall Render( );
	void			Update( );
};


#endif /*__INCDEF_XR_ZSKY_H__*/

