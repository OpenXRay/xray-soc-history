#pragma once

enum
{
	GAME_ANY			= 0,
	GAME_SINGLE			= 1,
	GAME_DEATHMATCH		= 2,
	GAME_CTF			= 3,
	GAME_ASSAULT		= 4,	// Team1 - assaulting, Team0 - Defending
	GAME_CS				= 5
};

#ifndef _EDITOR
extern DWORD	GAME;
extern int		g_team;	
extern int		g_fraglimit;
extern int		g_timelimit;
extern DWORD	g_flags;
#endif

#pragma pack(push,1)
struct	game_PlayerState
{
	u16			team;
	s16			kills;
	s16			deaths;
	s16			money_total;
	s16			money_for_round;
};

struct	game_TeamState
{
	u16			score;
	u16			num_targets;
};
#pragma pack(pop)

class	game_sv_GameState
{
	u32								round;
	vector<game_TeamState>			teams;
public:
	// Main
	virtual		void				Lock					()								= 0;
	virtual		void				Unlock					()								= 0;
	virtual		game_PlayerState*	get_it					(DWORD it)						= 0;
	virtual		game_PlayerState*	get_id					(DWORD id)						= 0;	// DPNID
	virtual		u32					get_count				()								= 0;

	// Utilities
	virtual		u32					get_alive_count			(u32 team);

	// Events
	virtual		void				OnStartRound			()								= 0;	// ����� ������
	virtual		void				OnTeamScore				(u32 team)						= 0;	// ������� ��������
	virtual		void				OnTeamsInDraw			()								= 0;	// �����
	virtual		void				OnTargetTouched			(u32 id_who, u32 id_target)		= 0;
	virtual		void				OnPlayerKillPlayer		(u32 id_killer, u32 id_killed)	= 0;
	virtual		void				OnTimeElapsed			()								= 0;

	// Main
	virtual		void				Update					()								= 0;
	virtual		void				net_Export_State		(NET_Packet& P);						// full state
	virtual		void				net_Export_Update		(NET_Packet& P, DWORD id);				// just incremental update for specific client
};

class	game_cl_GameState
{
};
