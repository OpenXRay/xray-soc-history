// ShaderPrimitive.h: interface for the CShaderPrimitive class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADERPRIMITIVE_H__7D7E1AAF_9C13_4689_89F1_C97842484BD4__INCLUDED_)
#define AFX_SHADERPRIMITIVE_H__7D7E1AAF_9C13_4689_89F1_C97842484BD4__INCLUDED_
#pragma once

#define SHARED_DEF_VCOUNT	256*4
#define SHARED_DEF_ICOUNT	256*6

class ENGINE_API CShaderPrimitive  
{
	vector<CPrimitive>	hash;
public:
	// returns handle
	int			Create	(DWORD FVF, int vCount=SHARED_DEF_VCOUNT, int iCount=SHARED_DEF_ICOUNT);

	CPrimitive*	Access	(int handle);
};

#endif // !defined(AFX_SHADERPRIMITIVE_H__7D7E1AAF_9C13_4689_89F1_C97842484BD4__INCLUDED_)
