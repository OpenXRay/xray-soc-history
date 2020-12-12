//----------------------------------------------------
// file: StaticMesh.h
//----------------------------------------------------
#ifndef _INCDEF_StaticCEditMesh_H_
#define _INCDEF_StaticCEditMesh_H_

//----------------------------------------------------
// refs
struct st_Surface;
struct SPickInfo;
struct FSChunkDef;

// refs
class CFS_Base;
class CStream;

#pragma pack( push,1 )
struct st_VMap{
	char			name[MAX_OBJ_NAME];	// vertex map name
    Fvector2List    vm;			// u,v - координаты
    st_VMap()		{name[0]=0;}
};
struct st_VMapPt{
	int				vmap_index;	// ссылка на мапу
	int				index;		// индекс в мапе на uv
};
DEFINE_SVECTOR(st_VMapPt,8,VMapPtList,VMapPtIt);
DEFINE_VECTOR(VMapPtList,VMRefsList,VMRefsIt);
struct st_FaceVert{
	int 			pindex;		// point index in PointList
    int				vmref;		// vm index
};
struct st_Face{
    st_FaceVert		pv[3];		// face vertices (P->P...)
};
struct st_MeshOptions{
	int		 		m_Collision;
    int				m_Reserved0;
    st_MeshOptions(){m_Collision=1;}
};
struct st_FaceVert10{
	int 			pindex;		// point index in PointList
    Fvector			norm;
    int				vmref;		// vm index
};
struct st_Face10{
    Fvector			norm;
    st_FaceVert10	pv[3];		// face vertices (P->P...)
};
#pragma pack( pop )

DEFINE_VECTOR(INTList,AdjList,AdjIt);
DEFINE_VECTOR(st_VMap,VMapList,VMapIt);
DEFINE_VECTOR(st_Face,FaceList,FaceIt);
DEFINE_MAP(st_Surface*,INTList,SurfFaces,SurfFacesPairIt);

//refs
struct st_RenderBuffer;
struct st_Surface;
class INode;

class CEditMesh {
	friend class CEditObject;
    friend class CSectorItem;
    friend class CSector;
    friend class CPortalUtils;
    friend class SceneBuilder;
    
	char m_Name[MAX_OBJ_NAME];

    CEditObject*	m_Parent;

    // internal variables
    int				m_Visible;
    int				m_Locked;
protected:
	Fbox			m_Box;
    FvectorList		m_Points;	// |
    AdjList			m_Adjs;     // + some array size!!!
    SurfFaces		m_SurfFaces;
    FvectorList		m_PNormals;	// |*3
    FvectorList		m_FNormals;	// |
    FaceList		m_Faces;    // + some array size!!!
    VMapList		m_VMaps;
    VMRefsList		m_VMRefs;

    void 			PrepareMesh				();

	void 			RecurseTri				(int id);

	// mesh optimize routine
	bool 			OptimizeFace			(int face_id, bool bTestNormal);
	bool 			UpdateAdjacency			();
public:
	                CEditMesh				(CEditObject* parent){m_Parent=parent;Construct();}
//	                CEditMesh				(CEditMesh* source,CEditObject* parent){m_Parent=parent;Construct();CloneFrom(source);}
	virtual         ~CEditMesh				();
	void			Construct				(){m_Box.set(0,0,0,0,0,0);m_Visible=1;m_Locked=0;m_Name[0]=0;}
    void			Clear					();

    // mesh modify routine
//	void            CloneFrom				(CEditMesh *source);
	void            Transform				(Fmatrix& parent);


	IC char*		GetName					(){return m_Name;}
	void            GetBox					(Fbox& box){box.set(m_Box);}

    // statistics methods
    int 			GetFaceCount			(){return m_Faces.size();}
	int 			GetVertexCount			(){return m_Points.size();}

    // IO - routine
    void			SaveMesh				(CFS_Base&);
	bool 			LoadMesh				(CStream&);

	// convert
	void 			Optimize				(BOOL NoOpt);
    void			RecomputeBBox			();
	void			FlipFaces				();
	TriObject*		ExtractTriObject		(INode *node, int &deleteIt);
	bool			ExtractTexName			(char *dest, Texmap *map);
	bool			ExtractMaterial			(st_Surface *surf, StdMat *smtl);
	bool			Convert					(INode *node);
};
//----------------------------------------------------
#endif /*_INCDEF_StaticMesh_H_*/

