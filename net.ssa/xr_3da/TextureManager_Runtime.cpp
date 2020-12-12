#include "stdafx.h"
#include "texturemanager.h"

void	__fastcall CShaderManager::Set(Shader& S)
{
	if (S.S!=cache.S.S)	{
		cache.changes		|=	c_shader;
		cache.S.S			=	S.S;
		dwPassesRequired	=	cache.S.S->dwPasses;
		cache.S.S->Activate	();
		Device.Statistic.dwShader_Changes++;
	}
	if (S.T!=cache.S.T) {
		cache.changes		|=	c_textures;
		cache.S.T			=	S.T;
	}
	if (S.C!=cache.S.C)	{
		cache.changes		|=	c_constants;
		cache.S.C			=	S.C;
	}
	if (S.M!=cache.S.M)	{
		cache.changes		|=	c_matrices;
		cache.S.M			=	S.M;
	}
}

void	__fastcall	CShaderManager::SetupPass(DWORD ID)
{
	if ((ID==cache.pass)&&(0==cache.changes))	return;

	VERIFY	(ID<dwPassesRequired);
	cache.pass				= ID;

	// Shader CODE
	cache.shader->Apply		(ID);

	// Textures 
	CTextureArray::VECTOR&  T = cache.textures->Pass(ID);
	for (DWORD i=0; i<T.size(); i++) 
	{
		if (cache.surfaces[i]!=T[i]) 
		{
			cache.surfaces[i] =T[i];
			T[i]->Apply(i);
		}
	}

	// Constants
	if (cache.shader->Flags.bPixelShader)
	{
		svector<Fcolor,8>	data;

		CConstantArray::VECTOR&  C = cache.constants->Pass(ID);
		for (DWORD j=0; j<C.size(); j++)	data.push_back(C[j]->const_float);
		CHK_DX(HW.pDevice->SetPixelShaderConstant(0,data.begin(),data.size()));
	} else {
		CHK_DX(HW.pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,cache.constants->Pass(ID)[0]->const_dword))
	}

	// Matrices
	
}
