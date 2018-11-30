#pragma once

class ENGINE_API	CInifile;
struct ENGINE_API	xr_token;

class ENGINE_API C3DSoundContainer{
	friend class			CSoundManager;
	vector<int>				snd_indices;
	void	Create			( CInifile* ini, const char* section, char* item, xr_token* tokens);
	void	Delete			( );
public:
	void	Init			(int cnt);
	int		&operator[]		( int i ){ VERIFY(i>=0 && i<snd_indices.size()); return snd_indices[i]; };
};

