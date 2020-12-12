// file: MeshExpMesh.h
#ifndef __MeshExpMesh__H__INCLUDED__
#define __MeshExpMesh__H__INCLUDED__

#include "StaticMeshChunks.h"

using namespace std;


#pragma pack(push,1)

class MMaterial {
public:

	long m_FaceStart;
	long m_FaceCount;

	char m_TexName[MAX_PATH];

	Fcolor m_Ambient;
	Fcolor m_Diffuse;
	Fcolor m_Specular;
	Fcolor m_Emission;
	float m_Power;

public:
	MMaterial();
	MMaterial( MMaterial *source );
	~MMaterial();
};

class MPoint {
public:
	Fvector m_Point;
	Fvector m_Normal;
	Fvector m_TexCoord;
};

class MFace {
public:
	union{
		WORD p[3];
		struct { WORD p0,p1,p2; };
	};
};

#pragma pack(pop)

class ExpMesh {
public:
	// internal structure
	vector<MMaterial> m_Materials;
	vector<MPoint> m_Points;
	vector<MFace> m_Faces;
public:
	bool Verify();
	void Append( ExpMesh *source );
	void Copy( ExpMesh *source );
	void GetMaterial( ExpMesh *dest, MMaterial *material );
	void Transform( Fmatrix& parent );
	void Renormalize();
	void FlipFaces();
	void FlipNormals();

	int CreateUniqueFace(MPoint *points);
	void Save( char *filename );


	ExpMesh();
	~ExpMesh();
};




#endif /*__MeshExpMesh__H__INCLUDED__*/

