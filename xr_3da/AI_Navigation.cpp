// AI_Navigation.cpp: implementation of the CAI_Navigation class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AI_Navigation.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_Navigation::CAI_Navigation()
{

}

CAI_Navigation::~CAI_Navigation()
{
	for (int i=0; i<naviPlanes.size(); i++)
		delete naviPlanes[i];
}

void CAI_Navigation::Load(CVirtualFileStream&	fs)
{
	// header
	naviHeader	H;
	fs.ReadChunk(AIFF::Header,&H);
	R_ASSERT(H.version==1);

	// planes
	CStream*	pFS = fs.OpenChunk(AIFF::NaviPlanes);
	for (int id=0; ; id++)
	{
		CStream*		tmpFS = pFS->OpenChunk(id);
		if (0==tmpFS)	break;

		naviPlanes.push_back(new CAI_NaviPlane());
		naviPlanes.back()->Load(tmpFS);

		delete tmpFS;
	}
	delete pFS;

	// ok, now setup pointers to points/pathfinding/waydata
	DWORD S;

	S = fs.FindChunk(AIFF:NaviPoints);	R_ASSERT(S);
	naviPoints	= (CAI_NaviPoint*) fs.Pointer();

	S = fs.FindChunk(AIFF:NaviPath);	R_ASSERT(S);
	naviPath	= (BYTE*) fs.Pointer();

	S = fs.FindChunk(AIFF:NaviWaydata); R_ASSERT(S);
	naviWaydata = (BYTE*) fs.Pointer();
}

BOOL CAI_Navigation::BuildPath(CAI_Mover* pMover, Fvector& vTo, float maxLength)
{
	DWORD srcPlaneID	= pMover->planeID;
	DWORD destPlaneID	= classifyPoint(vTo);
	if (destPlaneID != srcPlaneID) {
		VERIFY(srcPlaneID<naviPlanes.size());
		VERIFY(destPlaneID<naviPlanes.size());

		// let's search path between planes
		WORD*	srcBegin	= naviPlanes[srcPlaneID]->naviLinks.begin	();
		WORD*	srcEnd		= naviPlanes[srcPlaneID]->naviLinks.end		();

		WORD*	destBegin	= naviPlanes[destPlaneID]->naviLinks.begin	();
		WORD*	destEnd		= naviPlanes[destPlaneID]->naviLinks.end	();

		Fvector&	srcPoint	= pMover->Position3D();
		WORD*		bestPath	= 0;
		float		bestCost	= flt_max;
		WORD		bestSource;
		WORD		bestDestination;

		for (WORD* src=srcBegin; src!=srcEnd; src++)
		{
			float srcCost = srcPoint.distance_to(naviPoints[*src].position);
			for (WORD* dest=destBegin; dest!=destEnd; dest++)
			{
				CAI_NaviPath* begin	= (CAI_NaviPath*)(naviPath+naviPoints[*src].path_offset);
				CAI_NaviPath* end	= begin+naviPoints[*src].path_count;

				CAI_NaviPath* found	= lower_bound(begin,end,DWORD(*dest));
				if (found->ID != DWORD(*dest)) continue;

				float	destCost	= vTo.distance_to(naviPoints[*dest].position);
				
				WORD*	pData		= (WORD*) (naviWaydata+found->offset);
				WORD	wCost		= *pData++;

				float	fCost		= float(wCost)/130.f + srcCost + destCost;
				if (fCost<bestCost) {
					bestPath		= pData;
					bestCost		= fCost;
					bestSource		= *src;
					bestDestination	= *dest;
				}
			}
		}

		// here we end-up with bestPath selected
		if (0==bestPath) return FALSE;

		// **** build real path
		vector<Fvector>		PATH;
		float				PATH_LEN	= 0;
		pMover->path_clear	();
		#pragma todo("pMover->path_clear")

		// 1st - from 'source' to point #1
		if (naviPlanes[srcPlaneID]->Navigate(
			pMover->Position3D(),
			naviPoints[bestDestination].position,
			PATH,
			&PATH_LEN
			))
		{
			#pragma todo("pMover->path_add")
			pMover->path_add	(PATH);
			PATH.clear			();
			if (PATH_LEN >= maxLength) {
				return TRUE;
			}
		} else {
			return FALSE;
		}

		// 2nd - from point #1 to point #last
		int	current_plane = 
		while (PATH_LEN<=maxLength)
		{
		}

		// 3rd - from poiny #last to 'destination'
	}
}
