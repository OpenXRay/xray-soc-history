#include "stdafx.h"
#include "compiler.h"

IC BYTE	compress(float c, int max_value)
{
	int	cover = iFloor(c*float(max_value)+.5f);
	clamp(cover,0,max_value);
	return BYTE(cover);
}

struct CNodeCompressed {
	IC	void	compress_node(NodeCompressed& Dest, vertex& Src);
};

IC void	CNodeCompressed::compress_node(NodeCompressed& Dest, vertex& Src)
{
	Dest.light	(compress(Src.LightLevel,15));
	for	(u32 L=0; L<4; ++L)
		Dest.link(L,Src.n[L]);
//	for	(u32 L=0; L<4; ++L)
//		if ((Src.n[L] < g_nodes.size()) && (Dest.link(L) != Src.n[L])) {
//			Dest.link(L,Src.n[L]);
//			Dest.link(L);
//		}
}

void	Compress	(NodeCompressed& Dest, vertex& Src, hdrNODES& H)
{
	// Compress plane (normal)
	Dest.plane	= pvCompress	(Src.Plane.n);
	
	// Compress position
	CNodePositionCompressor(Dest.p,Src.Pos,H);
//	CompressPos	(Dest.p1,Src.P1,H);
	
	// Sector
	// R_ASSERT(Src.sector<=255);
	// Dest.sector = BYTE(Src.sector);

	// Light & Cover
	CNodeCompressed().compress_node(Dest,Src);
//	Dest.cover[0]	= CompressCover(Src.cover[0]);
//	Dest.cover[1]	= CompressCover(Src.cover[1]);
//	Dest.cover[2]	= CompressCover(Src.cover[2]);
//	Dest.cover[3]	= CompressCover(Src.cover[3]);
	Dest.cover0		= compress(Src.cover[0],15);
	Dest.cover1		= compress(Src.cover[1],15);
	Dest.cover2		= compress(Src.cover[2],15);
	Dest.cover3		= compress(Src.cover[3],15);
//	Msg				("[%.3f -> %d][%.3f -> %d][%.3f -> %d][%.3f -> %d]",
//		Src.cover[0],Dest.cover0,
//		Src.cover[1],Dest.cover1,
//		Src.cover[2],Dest.cover2,
//		Src.cover[3],Dest.cover3
//		);

	// Compress links
//	R_ASSERT	(Src.neighbours.size()<64);
//	Dest.links	= BYTE(Src.neighbours.size());
}

float	CalculateHeight(Fbox& BB)
{
	// All nodes
	BB.invalidate();

	for (u32 i=0; i<g_nodes.size(); i++)
	{
		vertex&	N	= g_nodes[i];
		BB.modify	(N.Pos);
	}
	return BB.max.y-BB.min.y+EPS_L;
}

xr_vector<NodeCompressed>	compressed_nodes;

class CNodeRenumberer {
	struct SSortNodesPredicate {

		IC	bool	operator()			(const NodeCompressed &vertex0, const NodeCompressed &vertex1) const
		{
			return		(vertex0.p.xz() < vertex1.p.xz());
		}


		IC	bool	operator()			(u32 vertex_id0, u32 vertex_id1) const
		{
			return		(compressed_nodes[vertex_id0].p.xz() < compressed_nodes[vertex_id1].p.xz());
		}
	};

	xr_vector<NodeCompressed>	&m_nodes;
	xr_vector<u32>				m_sorted;

public:
					CNodeRenumberer(xr_vector<NodeCompressed> &nodes) :
						m_nodes(nodes)
	{
		for (u32 i=0; i<(int)m_nodes.size(); ++i)
			m_sorted.push_back(i);

		std::sort	(m_sorted.begin(),m_sorted.end(),SSortNodesPredicate());

		for (u32 i=0; i<(int)m_nodes.size(); ++i) {
			for (u32 j=0; j<4; ++j) {
				xr_vector<u32>::iterator I = std::lower_bound(m_sorted.begin(),m_sorted.end(),m_nodes[i].link(j));
				if (m_sorted.end() != I)
					m_nodes[i].link(j,u32(I - m_sorted.begin()));
			}
		}

		std::sort	(m_nodes.begin(),m_nodes.end(),SSortNodesPredicate());
	}
};

void xrSaveNodes(LPCSTR N)
{
	Msg				("NS: %d, CNS: %d, ratio: %f%%",sizeof(vertex),sizeof(CLevelGraph::CVertex),100*float(sizeof(CLevelGraph::CVertex))/float(sizeof(vertex)));

	Msg				("Renumbering nodes...");

	string256		fName; 
	strconcat		(fName,N,"level.ai");

	IWriter			*fs = FS.w_open(fName);

	// Header
	Status			("Saving header...");
	hdrNODES		H;
	H.version		= XRAI_CURRENT_VERSION;
	H.count			= g_nodes.size();
	H.size			= g_params.fPatchSize;
	H.size_y		= CalculateHeight(H.aabb);
	fs->w			(&H,sizeof(H));
	
//	fs->w_u32		(g_covers_palette.size());
//	for (u32 j=0; j<g_covers_palette.size(); ++j)
//		fs->w		(&g_covers_palette[j],sizeof(g_covers_palette[j]));

	// All nodes
	Status			("Saving nodes...");
	for (u32 i=0; i<g_nodes.size(); ++i) {
		vertex			&N	= g_nodes[i];
		NodeCompressed	NC;
		Compress		(NC,N,H);
		compressed_nodes.push_back(NC);
	}

	CNodeRenumberer	A(compressed_nodes);

	for (u32 i=0; i<g_nodes.size(); ++i) {
		fs->w			(&compressed_nodes[i],sizeof(NodeCompressed));
		Progress		(float(i)/float(g_nodes.size()));
	}
	// Stats
	u32	SizeTotal	= fs->tell();
	Msg				("%dK saved",SizeTotal/1024);

	FS.w_close		(fs);
}
