#include "stdafx.h"
#include "3dsoundcontainer.h"
#include "xr_trims.h"
#include "xr_sndman.h"
#include "xr_ini.h"

void C3DSoundContainer::Init(int cnt)
{
	VERIFY(cnt);
	snd_indices.resize(cnt);
	for(int i=0;i<cnt; i++)	snd_indices[i] = SND_UNDEFINED;
}

void C3DSoundContainer::Create( CInifile* ini, const char* section, char* item, xr_token* tokens)
{
	char Names	[1024];
	char Name	[64];
	strcpy(Names,ini->ReadSTRING(section, item).c_str());
	
	for(int i=0;i<_GetItemCount(Names);i++){
		_GetItem(Names,i,Name);
		int id = ini->ReadTOKEN	(Name, "style", tokens);
		snd_indices[id]= pSounds->Create3D(ini, Name);			VERIFY	(snd_indices[id]!=SND_UNDEFINED);
	}
}

void C3DSoundContainer::Delete( )
{
	for(int i=0;i<snd_indices.size(); i++)
		if (snd_indices[i]!=SND_UNDEFINED) pSounds->Delete3D(snd_indices[i]);
	snd_indices.erase(snd_indices.begin(),snd_indices.end());
}
