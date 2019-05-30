#ifndef __XR_GAMMA_H__
#define __XR_GAMMA_H__

//-----------------------------------------------------------------------------------------------------------
//Gamma control
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API				CGammaControl
{
	float						fGamma;
	float						fBrightness;
	Fcolor						cBalance;

public:
	CGammaControl		() :
	  fGamma(1.f),
	  fBrightness(1.f)
	{ Balance(.5f,.5f,.5f); };

	IC void	Balance		(float _r, float _g, float _b)
	{	cBalance.set	(_r*512.f,_g*512.f,_b*512.f,1);	}
	IC void	Balance		(Fcolor &C)
	{	Balance(C.r,C.g,C.b); }

	IC void Gamma		(float G) { fGamma=G; }
	IC void Brightness	(float B) { fBrightness=(B - .5f)*255.f; }

	void	GetIP		(float& G, float &B, Fcolor& Balance)
	{
		G			= fGamma;
		B			= fBrightness;
		Balance.set	(cBalance);
	}

	void	GenLUT		(D3DGAMMARAMP &G);

	void	Update		();
};

#endif
