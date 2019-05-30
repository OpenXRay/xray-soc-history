//----------------------------------------------------
// file: SObject2.h
//----------------------------------------------------
#ifndef _INCDEF_SObject2_H_
#define _INCDEF_SObject2_H_

#include "SceneObject.h"
#include "StaticMesh.h"
//----------------------------------------------------
class CTextureLayer;

class SObject2Mesh {
public:
	char m_Name[32];
	char m_FileName[32];
	Fmatrix m_Position;
	ExpMesh *m_Mesh;
public:
	SObject2Mesh( ExpMesh *mesh = 0 ){
		m_Name[0]=0;
		m_FileName[0]=0;
		m_Position.identity();
		m_Mesh = mesh;
	};

	void CopyNames( const SObject2Mesh* source ){
        VERIFY(source);
		strcpy( m_Name, source->m_Name );
		strcpy( m_FileName, source->m_FileName );
	}

	~SObject2Mesh(){
		SAFE_DELETE( m_Mesh );
	}
	__inline char* GetName(){return m_Name; }
};

typedef list<SObject2Mesh*> SObjMeshList;
typedef SObjMeshList::iterator SObjMeshIt;

class SExpObject2 : public SceneObject {
	// build options
	int m_MakeUnique;
	int m_DynamicList;

	// object data
	Fmatrix m_Position;

    void ClearMeshes();
public:
	char m_ClassName[MAX_OBJCLS_NAME];
	char m_ClassScript[MAX_CLASS_SCRIPT];
	SObjMeshList m_Meshes;
public:
	SObject2Mesh* AddMesh( ExpMesh *mesh, char* name=0 ){
		m_Meshes.push_back( new SObject2Mesh() );
		m_Meshes.back()->m_Mesh = mesh;
		if (name){
			strcpy(m_Meshes.back()->m_Name,name);
			strcpy(m_Meshes.back()->m_FileName,name);
		}
		return m_Meshes.back();
	}
	void AppendMesh( ExpMesh *mesh, char* name=0 ){
		VERIFY(m_Meshes.back()->m_Mesh);
		m_Meshes.back()->m_Mesh->Append(mesh);
		if (name){
			strcpy(m_Meshes.back()->m_Name,name);
			strcpy(m_Meshes.back()->m_FileName,name);
		}
	}

	virtual void Save( int handle );

	SExpObject2( char *name );
	virtual ~SExpObject2();
	void Construct();
};
//----------------------------------------------------
#endif /*_INCDEF_SObject2_H_*/


