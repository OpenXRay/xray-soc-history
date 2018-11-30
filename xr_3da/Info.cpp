#include "stdafx.h"
#include "info.h"
#include "xr_gamefont.h"
#include "xr_ini.h"

//////////////////////////////////////////////////////////////////////
void CInfo::Load	(CInifile* INI, LPCSTR S)
{
	if (!INI->SectionExists(S)) return;

	// settings
	LPCSTR L = INI->ReadSTRING(S,"params");
	DWORD r,g,b;
	sscanf(L.c_str(),"%f,%f,%f,%d,%d,%d",&px,&py,&s,&r,&g,&b);
	c = D3DCOLOR_XRGB(r,g,b);

	// lines
	for (int i=0; ;i++)
	{
		char n[128];
		sprintf(n,"line%d",i);
		if (!INI->LineExists(S,n)) break;

		LPCSTR ln = INI->ReadSTRING(S,n);
		text.push_back(ln.substr(1,ln.length()-2));
	}
}

void CInfo::Render	(CFontBase* F)
{
	if (text.empty()) return;
	F->Color	(c);
	F->Size		(s);
	float		iy = py;
	for (int i=0; i<text.size(); i++) {
		F->Out(px,iy,(char*)text[i].c_str());
		iy += s*2.5f;
	}
}
