#pragma once

namespace AIFF {

#pragma pack(push,8)

enum {
	Header								= 0,
	NaviPlanes							= 1,
		NaviPlane_DEF					= 10,
		NaviPlane_OBSTACLES				= 11,
		NaviPlane_POINTS				= 12,
	NaviPoints							= 2,
	NaviPath							= 3,
	NaviWaydata							= 4,

	ForceDWORD							= DWORD(-1)
};

struct naviHeader {
	WORD	version;		// = 1
	WORD	reserved[31];	// reserved for future usage
};
#pragma pack(pop)


};
