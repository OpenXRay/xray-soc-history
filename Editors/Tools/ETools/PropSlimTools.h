#ifndef PropSlimToolsH
#define PropSlimToolsH

#ifdef ETOOLS_EXPORTS
#define ETOOLS_API __declspec( dllexport )
#else
#define ETOOLS_API __declspec( dllimport )
#endif

#include "ArbitraryList.h"

struct VIPM_SWR
{
	u32		offset;					// Offset of the first index in the index buffer to start at (note! no retrictions. Can easily be >64k)
	u16		num_tris;				// Number of tris to render (most cards can't do more than 65536)
	u16		num_verts;				// Number of vertices to render with (using WORD indices)
};
struct VIPM_Result
{
	ArbitraryList<u16>		permute_verts;
	ArbitraryList<VIPM_SWR>	swr_records;// The records of the collapses.
	ArbitraryList<u16>		indices;
	~VIPM_Result()
	{
		permute_verts.resize(0);
		swr_records.resize	(0);
		indices.resize		(0);
	}
};

extern "C" {
	ETOOLS_API void			VIPM_Init			();
	ETOOLS_API void			VIPM_AppendVertex	(const Fvector3& pt, const Fvector2& uv);
	ETOOLS_API void			VIPM_AppendFace		(u16 v0, u16 v1, u16 v2);
	ETOOLS_API VIPM_Result*	VIPM_Convert		(u32 max_sliding_window=u32(-1), float error_tolerance=0.1f, BOOL optimize_vertex_order=TRUE);
	ETOOLS_API void			VIPM_Destroy		();
};

#endif // PropSlimToolsH