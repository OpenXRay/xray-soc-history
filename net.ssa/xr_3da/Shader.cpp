// Shader.cpp: implementation of the CShader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Shader.h"
#include "device.h"
#include "ffileops.h"
#include "xr_trims.h"
#include "xr_tokens.h"
#include "xr_ini.h"
#include "scripting\script.h"

CShader*  xrLoadingShader = NULL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CShader::CShader(const char *name)
{
	VERIFY(name);
	VERIFY(strlen(name)<64);
	strcpy(cName,name);
	strlwr(cName);
	dwRefCount	= 0;			// texture manager increments it
	dwPassCount	= 0;
	ZeroMemory	(dwSBH,sizeof(dwSBH));
}

CShader::~CShader()
{
	VERIFY(dwSBH[0]==0);
}

void CShader::Load()
{
	Msg("* Loading shader '%s'...",cName);

	// Recording state block(s)
	VERIFY(HW.pDevice);
	xrLoadingShader = this;

	char fN[96],fP[96];
	fP[0]=0; fN[0]='_';	fN[1]=0; 

	strcat(fN,cName);
	char* pos_at = strchr(fN,'@');
	if (pos_at) {
		strcpy(fP,pos_at+1);
		*pos_at = 0;
	}
	Device.Texture.pShaders->Call(fN,fP);

	xrLoadingShader = NULL;

	Msg("*     %d pass shader",dwPassCount);
}

void CShader::Unload()
{
	for (DWORD i=0; i<dwPassCount; i++) {
		if (dwSBH[i]) CHK_DX(HW.pDevice->DeleteStateBlock(dwSBH[i]));
		dwSBH[i] = 0;
	}
	dwPassCount=0;
}

void CShader::Apply(DWORD dwPass)
{
	VERIFY(dwPass<dwPassCount);
	CHK_DX(HW.pDevice->ApplyStateBlock(dwSBH[dwPass]));
}

void CShader::AddPass(DWORD SBH)
{
	dwSBH[dwPassCount++]=SBH;
	VERIFY(dwPassCount<32);
}
