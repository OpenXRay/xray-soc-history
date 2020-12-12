#include "stdafx.h"

namespace AI_Saving {

	struct naviObstacle
	{
		vector<Fvector2>		border_cw;
	};
	typedef std::map<WORD,std::vector<WORD>> naviPath;
	struct naviPoint
	{
		Fvector		position;	// position in 3D space
		WORD		type;		// type of navigation point
		WORD		link1;		// every point is linked to another ONE.
		WORD		link2;		// ...and probably to yet another (bi-directional)
		naviPath	container;
	};
	
	struct naviPlane
	{
		Fvector p1,p2,p4;
		vector<naviObstacle>	obstacles;
		vector<WORD>			points;
	};

	WORD naviCost(vector<WORD> &way)
	{
		return 0;
	}

	void naviSave(
		const char* fname,
		vector<naviPoint> &points, 
		vector<naviPlane> &planes)
	{
		CFS_File	VM(fname);

		// header
		naviHeader	H;
		ZeroMemory	(&H,sizeof(H));
		H.version	= 1;
		VM.write_chunk(AIFF_Header, &H, sizeof(H));

		// planes
		CFS_Memory	fsPlanes;
		for (int i=0; i<planes.size(); i++)
		{
			naviPlane& P = planes[i];

			// definition
			fsPlanes.open_chunk(C_NaviPlane_DEF);
			fsPlanes.Wvector(&P.p1);
			fsPlanes.Wvector(&P.p2);
			fsPlanes.Wvector(&P.p3);
			fsPlanes.close_chunk();

			// obstacles
			fsPlanes.open_chunk(C_NaviPlane_OBSTACLES);
			vector<naviObstacle> &O = P.obstacles;
			for (int j=0; j<O.size(); j++)
			{
				fsPlanes.write_chunk(
					j,
					O[j].border_cw.begin(),
					O[j].border_cw.size()*sizeof(float)*2
					);
			}
			fsPlanes.close_chunk();

			// points
			fsPlanes.write_chunk(
				C_NaviPlane_POINTS,
				P.points.begin(),
				P.points.size()*sizeof(WORD));
		}
		VM.write_chunk_compressed(
			C_NaviPlanes,
			fsPlanes.pointer(),
			fsPlanes.size()
			)

		// points & pathes
		CFS_Memory	fsPoints;
		CFS_Memory	fsReachable;
		CFS_Memory	fsWayData;
		for (i=0; i<points.size(); i++)
		{
			naviPoint& P = points[i];
			
			// save base information
			fsPoints.Wvector(P.position);
			fsPoints.Wword	(P.type);
			fsPoints.Wword	(P.link1);
			fsPoints.Wword	(P.link2);
			fsPoints.Wword	(P.container.size());
			fsPoints.Wdword	(fsReachable.tell());

			// save 'reachable' & 'waydata' table
			for (naviPath::iterator N=P.container.begin(); N!=P.container.end(); N++)
			{
				// 'reachable'
				fsReachable.Wdword(N->first);
				fsReachable.Wdword(fsWaydata.tell());

				// waydata
				vector<WORD> &data = N->second;
				fsWayData.Wword(naviCost(data));
				fsWayData.Wword(data.size());
				fsWayData.write(data.begin(),data.size()*sizeof(WORD));
			}
		}
		VM.write_chunk_aligned(C_NaviPoints,	fsPoints.pointer(),		fsPoints.size());
		VM.write_chunk_aligned(C_NaviPath,		fsReachable.pointer(),	fsReachable.size());
		VM.write_chunk_aligned(C_NaviWaydata,	fsWayData.pointer(),	fsWayData.size());
	};
};
