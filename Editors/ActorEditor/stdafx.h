//----------------------------------------------------
// file: stdafx.h
//----------------------------------------------------
#ifndef stdafxH
#define stdafxH

#pragma once

#pragma warn -pck

#define sqrtf(a) sqrt(a)

#ifndef O_SEQUENTIAL
#define O_SEQUENTIAL 0
#endif

#define DIRECTINPUT_VERSION 0x0700

#define         R_R1    1
#define         R_R2    2
#define         RENDER  R_R1

// Std C++ headers
#include <fastmath.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <process.h>
#include <utime.h>

// iseful macros
// MSC names for functions
#ifdef _eof
#undef _eof
#endif
__inline int _eof	(int _a)   		                        { return ::eof(_a); }
#ifdef _access
#undef _access
#endif
__inline int _access(const char *_a, int _b)                { return ::access(_a,_b); }
#ifdef _lseek
#undef _lseek
#endif
__inline long _lseek(int handle, long offset, int fromwhere){ return ::lseek(handle, offset, fromwhere);}
#ifdef _dup
#undef _dup
#endif
__inline int _dup    (int handle)                           { return ::dup(handle);}
__inline float modff(float a, float *b){
	double x,y;
    y = modf(double(a),&x);
    *b = x;
    return float(y);
}
__inline float expf	(float val)                           	{ return ::exp(val);}

#define ENGINE_API
#define DLL_API			__declspec(dllimport)
#define PropertyGP(a,b)	__declspec( property( get=a, put=b ) )
#define THROW			R_ASSERT2(0,"THROW");
#define THROW2(a)		R_ASSERT2(0,a);

// core
#include <xrCore.h>
#include <xrCDB.h>
#include <Sound.h>
#include <PSystem.h>

// DirectX headers
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <dsound.h>

// some user components
#include "engine\_d3d_extensions.h"

#include "D3DX_Wrapper.h"

DEFINE_VECTOR(bool,boolVec,boolIt);
DEFINE_VECTOR(BOOL,BOOLVec,BOOLIt);
DEFINE_VECTOR(BOOL*,LPBOOLVec,LPBOOLIt);
DEFINE_VECTOR(int,IntVec,IntIt);
DEFINE_VECTOR(int*,LPIntVec,LPIntIt);
DEFINE_VECTOR(float,FloatVec,FloatIt);
DEFINE_VECTOR(float*,LPFloatVec,LPFloatIt);
DEFINE_VECTOR(Fplane,PlaneVec,PlaneIt);
DEFINE_VECTOR(Fvector2,Fvector2Vec,Fvector2It);
DEFINE_VECTOR(Fvector,FvectorVec,FvectorIt);
DEFINE_VECTOR(Fvector*,LPFvectorVec,LPFvectorIt);
DEFINE_VECTOR(Fcolor,FcolorVec,FcolorIt);
DEFINE_VECTOR(Fcolor*,LPFcolorVec,LPFcolorIt);
DEFINE_VECTOR(AnsiString,AStringVec,AStringIt);
DEFINE_VECTOR(AnsiString*,LPAStringVec,LPAStringIt);
DEFINE_VECTOR(LPSTR,LPSTRVec,LPSTRIt);
DEFINE_VECTOR(LPCSTR,LPCSTRVec,LPCSTRIt);
DEFINE_VECTOR(string64,string64Vec,string64It);

DEFINE_VECTOR(s8,S8Vec,S8It);
DEFINE_VECTOR(s8*,LPS8Vec,LPS8It);
DEFINE_VECTOR(s16,S16Vec,S16It);
DEFINE_VECTOR(s16*,LPS16Vec,LPS16It);
DEFINE_VECTOR(s32,S32Vec,S32It);
DEFINE_VECTOR(s32*,LPS32Vec,LPS32It);
DEFINE_VECTOR(u8,U8Vec,U8It);
DEFINE_VECTOR(u8*,LPU8Vec,LPU8It);
DEFINE_VECTOR(u16,U16Vec,U16It);
DEFINE_VECTOR(u16*,LPU16Vec,LPU16It);
DEFINE_VECTOR(u32,U32Vec,U32It);
DEFINE_VECTOR(u32*,LPU32Vec,LPU32It);

#include "Log.h"
#include "engine.h"
#include "defines.h"

struct str_pred : public binary_function<char*, char*, bool>
{
    IC bool operator()(LPCSTR x, LPCSTR y) const
    {	return strcmp(x,y)<0;	}
};
struct astr_pred : public binary_function<AnsiString&, AnsiString&, bool>
{
    IC bool operator()(AnsiString& x, AnsiString& y) const
    {	return x<y;	}
};

#ifdef _EDITOR
	class PropValue;
	class PropItem;
	DEFINE_VECTOR(PropItem*,PropItemVec,PropItemIt);

	#include "device.h"
	#include "properties.h"
	#include "render.h"
	DEFINE_VECTOR(FVF::L,FLvertexVec,FLvertexIt);
	DEFINE_VECTOR(FVF::TL,FTLvertexVec,FTLvertexIt);
	DEFINE_VECTOR(FVF::LIT,FLITvertexVec,FLITvertexIt);

	#include "xrXRC.h"

	#include "CustomObject.h"
#endif

#ifdef _LEVEL_EDITOR
	#define _EDITOR_FILE_NAME_ 	"level"
	#define _EDITOR_NAME_ 		"Level Editor"
#else
	#ifdef _SHADER_EDITOR
		#define _EDITOR_FILE_NAME_ 	"shader"
		#define _EDITOR_NAME_ 		"Shader Editor"
    #else
		#ifdef _PARTICLE_EDITOR
			#define _EDITOR_FILE_NAME_ 	"particle"
			#define _EDITOR_NAME_ 		"Particle Editor"
        #else
			#ifdef _ACTOR_EDITOR
				#define _EDITOR_FILE_NAME_ 	"actor"
				#define _EDITOR_NAME_ 		"Actor Editor"
            #else
            	#ifdef _LEVEL_OPTIONS
					#define _EDITOR_FILE_NAME_ 	"options"
					#define _EDITOR_NAME_ 		"Level Options"
                #endif
    		#endif
		#endif
    #endif
#endif

#define DEFINE_INI(storage){string256 buf;	strconcat(buf,_EDITOR_FILE_NAME_,".ini"); FS.update_path(buf,"$local_root$",buf); storage->IniFileName=buf;}
#define NONE_CAPTION "<none>"
#define MULTIPLESEL_CAPTION "<multiple selection>"

// external dependencies
#pragma comment(lib,"xrCoreB.lib")
#pragma comment(lib,"xrSoundB.lib")
#pragma comment(lib,"xrCDBB.lib")
#pragma comment(lib,"ParticleDLLB.lib")

#pragma comment(lib,"dinput.lib")
#pragma comment(lib,"freeimage.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"dxt.lib")
#pragma comment(lib,"xrProgressive.lib")
#pragma comment(lib,"MagicFMDLLB.lib")
#pragma comment(lib,"ETools.lib")
#pragma comment(lib,"LWO.lib")
#pragma comment(lib,"xrHemisphere.lib")

// path definition
#define _server_root_		"$server_root$"
#define _server_data_root_	"$server_data_root$"
#define _local_root_		"$local_root$"

#define _game_data_			"$game_data$"
#define _game_textures_		"$game_textures$"
#define _game_levels_		"$game_levels$"
#define _game_sounds_		"$game_sounds$"
#define _game_meshes_		"$game_meshes$"
#define _game_shaders_		"$game_shaders$"

#define _import_			"$import$"
#define _textures_			"$textures$"
#define _objects_			"$objects$"
#define _maps_				"$maps$"
#define _temp_				"$temp$"
#define _omotion_			"$omotion$"
#define _omotions_			"$omotions$"
#define _smotion_			"$smotion$"
#define _smotions_			"$smotions$"
#define _detail_objects_	"$detail_objects$"
#endif

#pragma hdrstop

