#pragma once

#include "game_sv_base.h"

class	game_sv_CS					: public game_sv_GameState
{
protected:
public:
	virtual		void				Create					(LPCSTR options);

	// Events
	virtual		void				OnRoundStart			();										// ����� ������

	virtual		void				OnTeamScore				(u32 team);								// ������� ��������
	virtual		void				OnTeamsInDraw			();										// �����

	virtual		BOOL				OnTouch					(u16 eid_who, u16 eid_what);
	virtual		BOOL				OnDetouch				(u16 eid_who, u16 eid_what);

	virtual		void				OnPlayerConnect			(u32 id_who);
	virtual		void				OnPlayerBuy				(u32 id_who, u32 eid_who, LPCSTR what);
	virtual		void				OnPlayerReady			(u32 id_who);
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed);

	virtual		void				OnFraglimitExceed		()								{};
	virtual		void				OnTimelimitExceed		();

	// Mains
	virtual		void				Update					();

	u8 AutoTeam();
};
