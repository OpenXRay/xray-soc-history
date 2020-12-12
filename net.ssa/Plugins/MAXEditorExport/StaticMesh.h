//----------------------------------------------------
// file: StaticMesh.h
//----------------------------------------------------
#ifndef _INCDEF_StaticMesh_H_
#define _INCDEF_StaticMesh_H_

//----------------------------------------------------

// refs
struct SPickInfo;
struct FSChunkDef;
class ETexture;

using namespace std;

typedef unsigned __int64	CLASS_ID;
#define SCRIPT_MAX_SIZE 255

#pragma pack(push,1)
enum EShaderOP{
    sopBlend=0,
    sopMul,
    sopAdd,
    sop_forcedword = (-1)
};

struct SPoint {
	Fvector m_Point;
	Fvector m_Normal;
};

struct SFace {
	union{
		WORD p[3];
		struct { WORD p0,p1,p2; };
	};
};

struct SFaceTie{// -1 - face not found
	union{
		int t[3];
		struct { int t0,t1,t2; };
	};
};

struct SUV{
    float tu, tv;
};

struct SUV_tri{
    SUV uv[3];
    SUV_tri(){
        ZeroMemory(uv,sizeof(SUV)*3);
        uv[1].tv=1.f; uv[2].tu=1.f;
    }
    SUV_tri(SUV t[3]){
        CopyMemory(uv,t,sizeof(SUV)*3);
    }
};

struct STFace :public SFace{
    int GFaceID;
};
#pragma pack(pop)

typedef vector<DWORD>  DWORDList;
typedef DWORDList::iterator DWORDIt;

typedef vector<SUV_tri> UVFaceList;
typedef UVFaceList::iterator UVFaceIt;

#define MAX_LAYER_NAME 32

class ExpMesh;

class CTextureLayer{
    friend class ExpMesh;

	ExpMesh*			m_Parent;
    char                m_LayerName	[MAX_LAYER_NAME];
	char				m_TexName	[MAX_PATH];
    char				m_Script	[SCRIPT_MAX_SIZE];
    unsigned __int64    m_CLS;      // texture class
    Fmaterial           m_Mat;      // d3d material
    EShaderOP           m_SOP;		// shader OP

// layer texture geometry
    UVFaceList          m_UVMaps;
    DWORDList           m_GFaceIDs;
public:
				CTextureLayer	(ExpMesh* parent);
    virtual		~CTextureLayer	();
    bool		AddFace			(DWORD face_id, SUV_tri* uv=NULL);
	bool		AddFaceUnique	(DWORD face_id, SUV_tri* uv=NULL);
	void		Copy			(CTextureLayer* source, DWORD face_offs=0, bool bCopyData=true);
	void		FlipFaces		();
};

typedef vector<CTextureLayer*>::iterator TLayerIt;
typedef vector<SPoint>::iterator PointIt;
typedef vector<SFace>::iterator FaceIt;

class ExpMesh;
class INode;
class TriObject;
class Texmap;
class StdMat;
class Mesh;

#define MX 25
#define MY 25
#define MZ 25

typedef vector<SFaceTie> FTieList;
typedef FTieList::iterator FTieIt;

class ExpMesh {
	char fullname[MAX_PATH];
protected:
	friend class SceneBuilder;
    friend class TfrmPropertiesObject;
    friend class TfrmPropertiesSubObject;
    friend class CTextureLayer;

    vector<SPoint>			m_Vertices;
    vector<SFace>			m_GFaces;
    vector<CTextureLayer*>  m_Layers;
    FTieList	m_FTies;

	Fvector		VMmin, VMscale;
	DWORDList	VM[MX+1][MY+1][MZ+1];
	Fvector		VMeps;
protected:
    bool		UpdateTies();
	TriObject*	ExtractTriObject( INode *node, int &deleteIt );
	bool		ExtractMaterial( CTextureLayer *dest, StdMat *smtl );
	bool		ExtractTexName( char *dest, Texmap *map );
	bool		ComputeVertexNormals(Mesh *mesh);
public:
				ExpMesh		();
	virtual		~ExpMesh	();

	void		Save		( char *filename );
	void		SaveMesh	( int handle, bool bMeshChunk );

    CTextureLayer* AddTextureLayer();
	void		Append		( ExpMesh *source );
	void		Transform	( Fmatrix& parent );

	void		Renormalize	();
	void		FlipFaces	();
	void		FlipNormals	();
	void		CreateFace	(SPoint *points,bool bTestNormal);
	void		Duplicate2Sided(CTextureLayer* l, const char* mtl_name);
	
	bool		Convert		(INode *node);
};
//----------------------------------------------------
#endif /*_INCDEF_StaticMesh_H_*/

