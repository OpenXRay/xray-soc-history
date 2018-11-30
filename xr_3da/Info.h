#ifndef __XR_INFO_H__
#define __XR_INFO_H__
#pragma once

// refs
class CInifile;
class CFontBase;

class ENGINE_API CInfo
{
public:
	float	px,py,s;
	DWORD	c;
	vector<string>		text;

	void		Load	(CInifile* INI, LPCSTR S);
	void		Render	(CFontBase* F);
};
#endif // __XR_INFO_H__
