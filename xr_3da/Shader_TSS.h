// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)
#define AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_
#pragma once

#include "shader.h"
#include "blender_params.h"

class ENGINE_API CShader_TSS_Pass
{
public:
	DWORD						SB;
	
	CShader_TSS_Pass() : SB(0)	{}

	IC void						Release			()
	{
		CHK_DX(HW.pDevice->DeleteStateBlock(SB));
		SB				=0;
	}
	IC void						Apply			()
	{
		CHK_DX(HW.pDevice->ApplyStateBlock(SB));
	}
};

class ENGINE_API CShader_TSS	: public CShader
{
protected:
	virtual void				internal_Activate	()
	{
		for (DWORD I=0; I<Passes.size(); I++)
			Passes[I].Calculate(Device.fTimeGlobal);
	}
	virtual void				internal_Apply		(DWORD dwPass)
	{
		Passes[dwPass].Apply	();
	}
	virtual void				internal_Release	()
	{
		for (DWORD I=0; I<Passes.size(); I++)
			Passes[I].Release	();
		Passes.clear	();
	}
public:
	svector<CShader_TSS_Pass,8>	Passes;
	
	CShader_TSS					();
	virtual ~CShader_TSS		();
};

#endif // !defined(AFX_SHADER_H__9CBD70DD_E147_446B_B4EE_5DA321EB726F__INCLUDED_)
