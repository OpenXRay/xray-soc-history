// Shader.h: interface for the CShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADER_H__A2EE3106_CB83_4FEC_9C32_363051C8C5E5__INCLUDED_)
#define AFX_SHADER_H__A2EE3106_CB83_4FEC_9C32_363051C8C5E5__INCLUDED_
#pragma once

class ENGINE_API CShader
{
	char				cName[64];
	DWORD				dwSBH[32];
public:
	DWORD				dwPassCount;
	DWORD				dwRefCount;

	BOOL				isEqual		(const char *name) { return (stricmp(cName,name)==0); };

	void				Load		(void);
	void				AddPass		(DWORD SBH);

	void				Unload		(void);

	void				Apply		(DWORD dwPass);

	CShader				(const char *name);
	~CShader			();
};

extern CShader*			xrLoadingShader;

#endif // !defined(AFX_SHADER_H__A2EE3106_CB83_4FEC_9C32_363051C8C5E5__INCLUDED_)
