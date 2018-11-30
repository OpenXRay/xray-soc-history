// ShadowController.cpp: implementation of the CShadowController class.
//
//////////////////////////////////////////////////////////////////////
 
#include "stdafx.h"
#include "fshadowform.h"
#include "xr_creator.h"
#include "environment.h"
#include "ShadowController.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShadowController::CShadowController()
{
	hSVTexture = Device.Texture.Create("null","shadow_vol");
}

CShadowController::~CShadowController()
{
	Device.Texture.Delete(hSVTexture);
}

void CShadowController::Add(FShadowForm* pForm, Fmatrix *pTransform)
{
	VERIFY(pForm);
	VERIFY(pTransform);
	Shadows.push_back(pForm);
	Transforms.push_back(pTransform);
}

void CShadowController::Render()
{
//	static float angle = 0;

	if (Shadows.empty()) return;

	// ************** Firstly - build shadow volumes
	Fvector SunDir;
//	angle += Device.fTimeDelta;
//	SunDir.set(cos(angle),-1,sin(angle));
//	SunDir.normalize();
	SunDir.set(pCreator->Environment.vSunDir);

	// Dont bother with interpolating color
	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE,		D3DSHADE_FLAT		));

	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE,	TRUE				));

	// Set up stencil compare fuction, reference value, and masks
	// Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true
	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILFUNC,		D3DCMP_ALWAYS		));
	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILZFAIL,		D3DSTENCILOP_KEEP	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILFAIL,		D3DSTENCILOP_KEEP	));

	// If ztest passes, inc/decrement stencil buffer value
	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,       0x1			));
	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILMASK,      0xffffffff	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILWRITEMASK, 0xffffffff	));

	// ************** Secondly - render volumes
	// Turn depth buffer off, and stencil buffer on
	Device.SetMode(vAlpha|vZTest);
	Device.Texture.Set(hSVTexture);

	for (int i=0; i<Shadows.size(); i++) {
		HW.pDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, precalc_identity.d3d()	);
		HW.pDevice->SetTransform( D3DTRANSFORMSTATE_WORLD,     precalc_identity.d3d()	);

		// it drops down the View projection :(
		Shadows[i]->MakeShadowVolume(SunDir,*Transforms[i]);

		// restore transforms
		CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION,	Device.mProject.d3d()	));
		CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATE_VIEW,		Device.mView.d3d()		));
		CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, Transforms[i]->d3d() ));

		// Need to set transform
		Shadows[i]->Render(1.0f);
	}

	//restore Gouraud shading
    CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD	));

	// Desc: Draws a big grey polygon over scene, and blend it with pixels with
	//       stencil >=1, which are in shadow.  Could optimize this by keeping track
	//       of rendered 2D extent rect of all shadow vols.

	Device.Texture.Set(0);	// default shader
	Device.SetMode(vAlpha);

    // Only write where stencil val >= 1 (count indicates # of shadows that
    // overlap that pixel)
    CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,  0x1					));
    CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILFUNC, D3DCMP_LESSEQUAL		));
    CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_KEEP	));

	//*** Shadow
	FTLvertex	shadow[4];
	FPcolor		dwColor;
	dwColor.set(0,0,0,96);

	float dwMaxY=float(Device.dwHeight);
	float dwMaxX=float(Device.dwWidth);

	shadow[0].set(0,dwMaxY,0.95f,1.0f,dwColor,dwColor, 0, 0);		// LB
	shadow[1].set(0,0,0.95f,1.0f,dwColor,dwColor, 0, 0);			// LT
	shadow[2].set(dwMaxX,dwMaxY,0.95f,1.0f,dwColor,dwColor, 0, 0);	// RB
	shadow[3].set(dwMaxX,0,0.95f,1.0f,dwColor,dwColor, 0, 0);		// RT

	Device.Draw.PrimitiveSP(D3DPT_TRIANGLESTRIP,FTLVERTEX,shadow, 4);
	//***
    CHK_DX(HW.pDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE,    FALSE ));

	// clear lists
	Device.Statistic.dwShadows += Shadows.size();
	Shadows.clear();
	Transforms.clear();
}
