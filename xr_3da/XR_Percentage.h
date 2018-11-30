// XR_Percentage.h: interface for the CProgressBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XR_PERCENTAGE_H__9F8102C0_D828_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_XR_PERCENTAGE_H__9F8102C0_D828_11D3_B4E3_4854E82A090D__INCLUDED_
#pragma once

class CProgressBar :
	public pureDeviceCreate,
	public pureDeviceDestroy
{
	LPDIRECTDRAWSURFACE7	pBackground;
	LPDIRECTDRAWSURFACE7	pProgress;
	LPDIRECTDRAWSURFACE7	pBar;
public:
	int			lastp;
	UINT		timer;

	void		Set(int p);

	virtual		void	OnDeviceCreate	();
	virtual		void	OnDeviceDestroy	();

	CProgressBar();
	~CProgressBar();
};

#endif // !defined(AFX_XR_PERCENTAGE_H__9F8102C0_D828_11D3_B4E3_4854E82A090D__INCLUDED_)
