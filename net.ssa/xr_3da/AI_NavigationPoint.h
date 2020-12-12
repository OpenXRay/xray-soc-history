#pragma once

enum EAI_Navigation
{
	NP_Standart=0,
	NP_Lift,
	NP_JumpPad,

	NP_Last,
	NP_ForceDword = DWORD(-1)
};

/*
	// General NaviVM structure

	// 1st --- Planes itself,	CHUNK #0
	DWORD	number_of_planes;
	plane #1
	...
	plane #[number_of_planes-1]

	// 2nd --- WayPoints

	// 2nd --- Pathes between waypoints
	....

	// ***** Virtual memory structure for one PLANE
	// chunk_header
	Fvector	p1,p2,p4;
	WORD	number_of_obstacles;
	WORD	number_of_points;
	// chunk_obstacles
	//	chunk_1
	//	chunk_n
	// chunk_points

	// ***** Virtual memory structure for one WAYPOINT
	Fvector	position;		// position in 3D space
	WORD	type;			// type of navigation point
	WORD	link1;			// every point is linked to another ONE.
	WORD	link2;			// ...and probably to yet another (bi-directional)
	WORD	path_count;		// number of waypoints which can be reached
	DWORD	path_offset;	// offset in the file to data containing pathfinding info


	// --- list of waypoints which can be reached
	// --- here CAN'T be MORE THAN ONE path to point
	// --- need to be sorted in [lowID...highID] order
	// waypoint #0
	DWORD	ID;
	DWORD	offset;
	// waypoint #1
	DWORD	ID;
	DWORD	offset;
	// waypoint #[path_count-1]
	DWORD	ID;
	DWORD	offset;

	// data itself (note: not including self_id at start and ID at end)
	WORD	count;
	WORD	cost;	// quantized cost of path. real (m) = float(cost)/131.07
	WORD	id1;
	WORD	id2;
	...
	WORD	id#count;
*/

