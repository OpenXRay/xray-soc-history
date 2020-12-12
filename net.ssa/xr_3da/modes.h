#ifndef _MODES_
#define _MODES_

#include "dc_h.sb"

extern ENGINE_API void __fastcall __EnableMode(DWORD dwMask);
extern ENGINE_API void __fastcall __DisableMode(DWORD dwMask);

// light
IC void EnableLight(void)
{	CHK_DX(HW.pDevice->SetRenderState(D3DRS_LIGHTING,TRUE));			}
IC void DisableLight(void)
{	CHK_DX(HW.pDevice->SetRenderState(D3DRS_LIGHTING,FALSE));			}

// specular
IC void EnableSpecular(void)
{	CHK_DX(HW.pDevice->SetRenderState(D3DRS_SPECULARENABLE,TRUE));		}
IC void DisableSpecular(void)
{	CHK_DX(HW.pDevice->SetRenderState(D3DRS_SPECULARENABLE,FALSE));	}

// fog
IC void EnableFog(void)
{	CHK_DX(HW.pDevice->SetRenderState(D3DRS_FOGENABLE,TRUE));		}
IC void DisableFog(void)
{	CHK_DX(HW.pDevice->SetRenderState(D3DRS_FOGENABLE,FALSE));	}

// z-test
IC void EnableZTest(void)
{	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL));	}
IC void DisableZTest(void)
{	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS));		}

// z-write
IC void EnableZWrite(void)
{	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZWRITEENABLE,TRUE));		}
IC void DisableZWrite(void)
{	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ZWRITEENABLE,FALSE));		}

// alpha-blending
IC void EnableAlpha(void)
{
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	TRUE));		
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	TRUE));
}
IC void DisableAlpha(void)
{
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	FALSE));
	CHK_DX(HW.pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	FALSE));		
}

#endif
