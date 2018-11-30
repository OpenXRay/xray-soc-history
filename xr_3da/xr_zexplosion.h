// file: xr_zexplosion.h

#ifndef __INCDEF_XR_ZEXPLOSION_H__
#define __INCDEF_XR_ZEXPLOSION_H__

#include "xr_effect.h"
#include "xr_list.h"

// refs
class CSound;

struct BE_INIT_INFO
{
	int pcount_from,pcount_difference;
	float psize_from,psize_difference;

	float paccel_from,paccel_difference;
	float pspeed_from,pspeed_difference;
	float accelstop,accelstart;

	float praccel_from,praccel_difference;
	float prspeed_from,prspeed_difference;
	float raccelstop,raccelstart;

	float s_attack,s_decay;
	float sx,sy,sz;
	float maxalpha;
	float gen_radius;
	float max_radius;
	float ax,ay,az;

	int	  hTexture;
};

#define ESTAGE_SCALE_UP       0x00000001
#define ESTAGE_SCALE_DOWN     0x00000002
#define ESTAGE_ACCEL_ENABLED  0x00000004
#define ESTAGE_RACCEL_ENABLED 0x00000008
#define ESTAGE_VALID          0x00000100

struct EPARTICLE
{
	int stage;
	float size;
	float scale;
	float accel;
	float speed;
	float raccel;
	float rspeed;
	Fvector direction;
	Fvector center;
	Fvector svcenter;
};

#define PSB_VALID 0x00000001

class CXR_PSB_Explosion : public CEffectBase
{
public:
	CXR_PSB_Explosion( Fvector& where, BE_INIT_INFO *_param = 0 );
	virtual ~CXR_PSB_Explosion();
	virtual void Render();
	virtual void OnMove();

protected:

	void Draw();

	Fvector axis;
	//Fmatrix rotation;

	int	  hTexture;
	CSound *sound;
	Flight light;

	Fvector center;
	float r;
	float s_attack;
	float s_decay;
	float maxalpha;
	float accelstop;
	float accelstart;
	float raccelstop;
	float raccelstart;

	int render_faces;
	int stage;

	CList<EPARTICLE>particles;
	CList<WORD>		buffer_f;

public:
	static void LoadTemplate( CInifile* ini, char *section, BE_INIT_INFO *i );
	static void UnloadTemplate( BE_INIT_INFO *i );

};


#endif /*__INCDEF_XR_ZEXPLOSION_H__*/

