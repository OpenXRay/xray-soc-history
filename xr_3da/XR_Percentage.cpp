// XR_Percentage.cpp: implementation of the CProgressBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "XR_Percentage.h"
#include "ddutil.h"

static const char *szBBack		= "data\\maps\\loader\\main.bmp";
static const char *szBProgress	= "data\\maps\\loader\\progress.bmp";
static const char *szBBar		= "data\\maps\\loader\\bar.bmp";

#define BAR_X	9
#define BAR_Y	9

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgressBar::CProgressBar()
{
	lastp=0;
	if (Device.bReady)	OnDeviceCreate();

	Device.seqDevCreate.Add(this);
	Device.seqDevDestroy.Add(this);
}

CProgressBar::~CProgressBar()
{
	Device.seqDevCreate.Remove(this);
	Device.seqDevDestroy.Remove(this);

	if	(Device.bReady)	OnDeviceDestroy();
}

void CProgressBar::OnDeviceCreate()
{
	pProgress	= 	LoadSurface(szBProgress	);
	pBackground	= 	LoadSurface(szBBack		);
	pBar		=	LoadSurface(szBBar		);
}

void CProgressBar::OnDeviceDestroy()
{
	_RELEASE(pBar);
	_RELEASE(pBackground);
	_RELEASE(pProgress);
}

void CProgressBar::Set(int p)
{
	if (p<lastp) return;
	lastp=p;

	HW.pContext->SetClearColor(0);
	HW.pContext->Clear(D3DCLEAR_TARGET);
	DrawSurface(Device.dwWidth/2-512/2,Device.dwHeight/2-384/2,pBackground,szBBack);
	DrawSurface(Device.dwWidth-256,Device.dwHeight-128,pProgress,szBProgress);
	float fp = float(p)/float(100);
	float total_bars = float(241-74)/float(BAR_X);
	int need_bars  = int(total_bars*fp);
	for (int i=0; i<need_bars; i++) {
		DrawSurface(Device.dwWidth-256+74+i*BAR_X,Device.dwHeight-128+40,pBar,szBBar);
	}
	HW.pContext->UpdateFrame(0);
}
