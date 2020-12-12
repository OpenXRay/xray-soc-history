#pragma once

class	ENGINE_API	CShader
{
protected:
	struct RFlags
	{
		DWORD	iPriority	:	2;
		DWORD	bStrictB2F	:	1;
		DWORD	bLighting	:	1;
		DWORD	bPixelShader:	1;
	};
public:
	DWORD						dwRefCount;
	DWORD						dwFrame;
	RFlags						Flags;

	svector<DWORD,8>			shPasses;		// up to 8 passes

	IC void						Activate	()
	{
		if (Device.dwFrame != dwFrame) {
			dwFrame = Device.dwFrame;

			for (DWORD C=0; C<shConstants.size();	C++)	shConstants	[C]->Calculate	();
			for (DWORD M=0; M<shMatrices.size();	M++)	shMatrices	[M]->Calculate	();
		}
	}
	IC void						Apply		(DWORD dwPass)
	{	HW.pDevice->ApplyStateBlock(shPasses[dwPass]);	}
	IC void						Release		()
	{	internal_Release();			}

	CShader						();
	~CShader					();
};

