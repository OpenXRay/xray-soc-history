// ShadowController.h: interface for the CShadowController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWCONTROLLER_H__68267FC5_0F0E_4469_B476_D1255AB3109D__INCLUDED_)
#define AFX_SHADOWCONTROLLER_H__68267FC5_0F0E_4469_B476_D1255AB3109D__INCLUDED_
#pragma once

class FShadowForm;

class CShadowController
{
	vector<FShadowForm*>	Shadows;
	vector<Fmatrix*>		Transforms;
	int						hSVTexture;
public:
	void				Add(FShadowForm* pForm, Fmatrix *mWorld);
	void				Render(void);

	CShadowController	();
	~CShadowController	();
};

#endif // !defined(AFX_SHADOWCONTROLLER_H__68267FC5_0F0E_4469_B476_D1255AB3109D__INCLUDED_)
