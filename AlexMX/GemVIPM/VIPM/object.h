/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */
#ifndef objectH
#define objectH

//#include "quad.h"

// Incremented by the draw routs. Display + zero whenever you want.
extern int g_iMaxNumTrisDrawn;
extern long g_bShowVIPMInfo;
extern long g_bUseFastButBadOptimise;


// The data that gets stored inside mesh.h's tris, pts and edges.
class MeshPt;
class MeshEdge;
class MeshTri;

#include "D3dx8core.h"

struct MyPt
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNorm;
	float fU, fV;

	DWORD dwIndex;

	// Temporary data.
	MeshPt *pTempPt;	// Temporary data.
};

struct MyEdge
{
	// Temporary data.
};

struct MyTri
{
	// Temporary data.
	int iSlidingWindowLevel;			// Which sliding window level this tri belongs to.

	DWORD dwIndex;
	MeshTri *pOriginalTri;
};


// The data that gets stored inside mesh.h's tris, pts and edges.
#define MESHTRI_APP_DEFINED		MyTri	mytri;
#define MESHEDGE_APP_DEFINED	MyEdge	myedge;
#define MESHPT_APP_DEFINED		MyPt	mypt;


#include "xrCore.h"

#include "mesh.h"
#include "MxBlock.h"
#include "MxQMetric.h"

struct GeneralTriInfo
{
	MeshPt		*ppt[3];
};

struct GeneralCollapseInfo
{
	DlinkDefine(GeneralCollapseInfo,List);
	
	ArbitraryList<GeneralTriInfo>		TriOriginal;
	ArbitraryList<GeneralTriInfo>		TriCollapsed;

	int			iSlidingWindowLevel;					// Which sliding window level the binned tris will belong to.
	ArbitraryList<GeneralTriInfo>		TriNextLevel;	// On collapses that change levels, lists the tris that were on the next level.

	MeshPt		*pptBin;
	MeshPt		*pptKeep;

	float		fError;					// Error of this collapse.
	int			iNumTris;				// Number of tris after this collapse has been made.

	DlinkMethods(GeneralCollapseInfo,List);

	GeneralCollapseInfo()
	{
		ListInit();
	}

	GeneralCollapseInfo ( GeneralCollapseInfo *pPrev )
	{
		ListInit();
		ListAddAfter ( pPrev );
	}

	~GeneralCollapseInfo()
	{
		ListDel();
	}
};

struct Object
{
	struct edge_info
	{
		float				err;
		int					v1, v2;
		MxVector			target;
							edge_info			(){};
	};
	xr_vector<edge_info*>	edge_list;

	xr_vector<MxQuadric*>	__quadrics;			// 1 per vertex

	void					collect_quadrics	();
	unsigned int			quadric_count		() const { return __quadrics.size(); }
	void					compute_face_quadric(MeshTri* tri, MxQuadric& Q);
	MxQuadric&				quadric				(unsigned int i)       { return *(__quadrics[i]); }
	const MxQuadric&		quadric				(unsigned int i) const { return *(__quadrics[i]); }

	// The permanent shape.
	MeshPt		PermPtRoot;
	MeshTri		PermTriRoot;
	MeshEdge	PermEdgeRoot;

	// The collapse list is ordered backwards,
	// so ptr->ListNext() is the _previous_ collapse to ptr.
	GeneralCollapseInfo		CollapseRoot;

	// The current shape.
	MeshPt		CurPtRoot;
	MeshTri		CurTriRoot;
	MeshEdge	CurEdgeRoot;

	// pNextCollapse points to the _next_ collapse to do.
	// pNextCollapse->ListNext() is the collapse that's just been done.
	// &CollapseRoot = no more collapses to do.
	GeneralCollapseInfo		*pNextCollapse;

	int			iFullNumTris;		// How many tris with no collapses.
	int			iFullNumPts;		// How many pts with no collapses.
	int			iNumCollapses;		// Total number of collapses.


	int			iCurSlidingWindowLevel;

public:	
				Object						();

				~Object						();

	// Check that this is sensible.
	void		CheckObject					( void );

	// Bins all the current data.
	void		BinCurrentObject			( void );

	// Creates the current data from the permanent data.
	void		MakeCurrentObjectFromPerm	( void );

	// Creates and performs a collapse of pptBinned to pptKept.
	// Make sure they actually share an edge!
	// Make sure the object is fully collapsed already.
	void		CreateEdgeCollapse			( MeshPt *pptBinned, MeshPt *pptKept );

	// Bin the last collapse.
	// Returns TRUE if these was a last collapse to do.
	long		BinEdgeCollapse				( void );

	// Returns TRUE if a collapse was undone.
	long		UndoCollapse				( void );

	// Returns TRUE if a collapse was done.
	long		DoCollapse					( void );
	
	void		SetNewLevel					( int iLevel );

	long		CollapseAllowedForLevel		( MeshPt *pptBinned, int iLevel );

	// Return the error from this edge collapse.
	// Set bTryToCacheResult=TRUE if you can pass pptBinned in multiple times.
	// Make sure you call this with bTryToCacheResult=FALSE if any data changes,
	//	or you'll confuse the poor thing.
	float		FindCollapseError			( MeshPt *pptBinned, MeshEdge *pedgeCollapse, long bTryToCacheResult = FALSE );
};

#endif // objectH
