#include "stdafx.h"
#include "xr_effgamma.h"
#include "device.h"

IC WORD clr2gamma(float c)
{
	int C=iFloor(c);
	clamp(C,0,65535);
	return WORD(C);
}
void CGammaControl::Update() 
{
	if (HW.pDevice) {
		D3DGAMMARAMP G;
		GenLUT(G);
		HW.pDevice->SetGammaRamp(D3DSGR_NO_CALIBRATION,&G);
	}
}
void CGammaControl::GenLUT(D3DGAMMARAMP &G)
{
	float Gamma		= 1.f;
	if (psDeviceFlags&rsFullscreen)	Gamma=(fGamma+2*0.6f)/2;
	float fScale = (1.f / 255.f) * 256.f * Gamma;
	for (int i=0; i<256; i++) {
		float	c	= float(i)*fScale + fBrightness;
		G.red[i]	= clr2gamma(c*cBalance.r);
		G.green[i]	= clr2gamma(c*cBalance.g);
		G.blue[i]	= clr2gamma(c*cBalance.b);
	}
}
