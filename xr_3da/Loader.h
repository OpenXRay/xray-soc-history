// Loader.h: interface for the CLoader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOADER_H__452157F4_C664_42F5_9ED3_EAB34B64E9BE__INCLUDED_)
#define AFX_LOADER_H__452157F4_C664_42F5_9ED3_EAB34B64E9BE__INCLUDED_

#pragma once

class ENGINE_API CInifile;

class ENGINE_API CLoader
{
public:
	BOOL	Load			(char *name, CInifile *INI);
	char*	GetTName		(DWORD T) { return textures[T];	}

	CLoader	();
	~CLoader();
};

extern CLoader*	pLoader;

#endif // !defined(AFX_LOADER_H__452157F4_C664_42F5_9ED3_EAB34B64E9BE__INCLUDED_)
