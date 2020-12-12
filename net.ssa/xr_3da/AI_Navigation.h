// AI_Navigation.h: interface for the CAI_Navigation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AI_NAVIGATION_H__A2B8111D_1E10_4F6C_9905_25447553DA08__INCLUDED_)
#define AFX_AI_NAVIGATION_H__A2B8111D_1E10_4F6C_9905_25447553DA08__INCLUDED_
#pragma once

#pragma pack(push,8)
struct CAI_NaviPoint {
	Fvector	position;		// position in 3D space
	WORD	type;			// type of navigation point
	WORD	link1;			// every point is linked to another ONE.
	WORD	link2;			// ...and probably to yet another (bi-directional)
	WORD	path_count;		// number of waypoints which can be reached
	DWORD	path_offset;	// offset in the chunk to data containing pathfinding info
};
struct CAI_NaviPath {
	DWORD	ID;
	DWORD	offset;
	IC bool operator < (CAI_NaviPath& O) { return ID<O.ID; }
};
struct CAI_NaviWaydata {
	WORD	cost;			// quantized cost of path. real (m) = float(cost)/130 (504m max)
	WORD	count;
};
#pragma pack(pop)

class ENGINE_API CAI_Navigation  
{
private:
	vector<CAI_NaviPlane*>	naviPlanes;
	CAI_NaviPoint*			naviPoints;
	BYTE*					naviPath;
	BYTE*					naviWaydata;
private:
	int						classifyPoint();	// returns PlaneID
public:
	void					Load(CVirtualFileStream&	fs);

	BOOL					BuildPath(CAI_Mover* pMover, Fvector& vTo, float maxLength=30.f);

	CAI_Navigation			();
	~CAI_Navigation			();
};

#endif // !defined(AFX_AI_NAVIGATION_H__A2B8111D_1E10_4F6C_9905_25447553DA08__INCLUDED_)
