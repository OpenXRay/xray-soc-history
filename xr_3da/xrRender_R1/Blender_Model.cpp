#include "stdafx.h"
#pragma hdrstop

#include "Blender_Model.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Model::CBlender_Model()
{
	description.CLS		= B_MODEL;
	description.version	= 1;
	oAREF.value			= 32;
	oAREF.min			= 0;
	oAREF.max			= 255;
	oBlend.value		= FALSE;
}

CBlender_Model::~CBlender_Model()
{
	
}

void	CBlender_Model::Save	( IWriter& fs	)
{
	IBlender::Save		(fs);
	xrPWRITE_PROP		(fs,"Use alpha-channel",	xrPID_BOOL,		oBlend);
	xrPWRITE_PROP		(fs,"Alpha ref",			xrPID_INTEGER,	oAREF);
}

void	CBlender_Model::Load	( IReader& fs, u16 version)
{
	IBlender::Load		(fs,version);

	switch (version)	
	{
	case 0: 
		oAREF.value			= 32;
		oAREF.min			= 0;
		oAREF.max			= 255;
		oBlend.value		= FALSE;
		break;
	case 1:
	default:
		xrPREAD_PROP	(fs,xrPID_BOOL,		oBlend);
		xrPREAD_PROP	(fs,xrPID_INTEGER,	oAREF);
		break;
	}
}

void	CBlender_Model::Compile	(CBlender_Compile& C)
{
	IBlender::Compile		(C);
	if (C.bEditor)
	{
		C.PassBegin		();
		{
			C.PassSET_ZB		(TRUE,TRUE);
			if (oBlend.value)	C.PassSET_Blend_BLEND	(TRUE,oAREF.value);
			else				C.PassSET_Blend_SET		();
			C.PassSET_LightFog	(TRUE,TRUE);
			C.StageBegin		();
			C.StageSET_Color	(D3DTA_TEXTURE,	  D3DTOP_MODULATE,		D3DTA_DIFFUSE);
			C.StageSET_Alpha	(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
			C.StageSET_TMC		(oT_Name,	"$null",	"$null",	0		);
			C.StageEnd			();
		}
		C.PassEnd			();
	} else {
		LPCSTR	vsname		= 0;
		LPCSTR	psname		= 0;
		u32		mskin		= RImplementation.m_skinning;
		switch (C.iElement)
		{
		case SE_R1_NORMAL_HQ:	
			vsname = psname =	"model_def_hq";
			if (1==mskin)		vsname		= "model_def_hq_1";
			if (2==mskin)		vsname		= "model_def_hq_2";
			if (oBlend.value)	C.r_Pass	(vsname,psname,TRUE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
			else				C.r_Pass	(vsname,psname,TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_Sampler_clf		("s_lmap",	"$user$projector",true);
			C.r_End				();
			break;
		case SE_R1_NORMAL_LQ:
			vsname = psname =	"model_def_lq"; 
			if (1==mskin)		vsname		= "model_def_lq_1";
			if (2==mskin)		vsname		= "model_def_lq_2";
			if (oBlend.value)	C.r_Pass	(vsname,psname,TRUE,TRUE,TRUE,TRUE,D3DBLEND_SRCALPHA,	D3DBLEND_INVSRCALPHA,	TRUE,oAREF.value);
			else				C.r_Pass	(vsname,psname,TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_End				();
			break;
		case SE_R1_LPOINT:
			vsname				= "model_def_point";
			psname				= "add_point";
			if (1==mskin)		vsname		= "model_def_point_1";
			if (2==mskin)		vsname		= "model_def_point_2";
			if (oBlend.value)	C.r_Pass	(vsname,psname,FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,	D3DBLEND_ONE,TRUE,oAREF.value);
			else				C.r_Pass	(vsname,psname,FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,	D3DBLEND_ONE,TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_Sampler_clf		("s_lmap",	TEX_POINT_ATT);
			C.r_Sampler_clf		("s_att",	TEX_POINT_ATT);
			C.r_End				();
			break;
		case SE_R1_LSPOT:
			vsname				= "model_def_spot";
			psname				= "add_spot";
			if (1==mskin)		vsname		= "model_def_spot_1";
			if (2==mskin)		vsname		= "model_def_spot_2";
			if (oBlend.value)	C.r_Pass	(vsname,psname,FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,	D3DBLEND_ONE,TRUE,oAREF.value);
			else				C.r_Pass	(vsname,psname,FALSE,TRUE,FALSE,TRUE,D3DBLEND_ONE,	D3DBLEND_ONE,TRUE);
			C.r_Sampler			("s_base",	C.L_textures[0]);
			C.r_Sampler_clf		("s_lmap",	"internal\\internal_light_att",		true);
			C.r_Sampler_clf		("s_att",	TEX_SPOT_ATT);
			C.r_End				();
			break;
		case SE_R1_LMODELS:
			break;
		}
	}
}
