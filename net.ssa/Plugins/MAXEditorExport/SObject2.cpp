//----------------------------------------------------
// file: SExpObject2.cpp
//----------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "Log.h"
#include "SObject2.h"
#include "SceneClassList.h"
#include "FileSystem.h"

//----------------------------------------------------
#define SOBJ_CHUNK_CLASS_SCRIPT  0xA912
#define SOBJ_CHUNK_POSITION      0xA913
#define SOBJ_CHUNK_REFERENCE     0xA914
#define SOBJ_CHUNK_MESH          0xA915
#define   SOBJ_CHUNK_MESH_NAME   0xA921
#define   SOBJ_CHUNK_MESH_FILE   0xA922
#define   SOBJ_CHUNK_MESH_POS    0xA923
#define   SOBJ_CHUNK_MESH_CORE   0xA924
#define   SOBJ_CHUNK_MESH_OPS    0xA926
#define SOBJ_CHUNK_BOP           0xA917
//----------------------------------------------------

// mimimal bounding box size
float g_MinBoxSize = 0.05f;

//----------------------------------------------------
SExpObject2::SExpObject2( char *name ):SceneObject(){
	Construct();
	strcpy( m_Name, name );
}

void SExpObject2::Construct(){
	m_ClassID = OBJCLASS_SOBJECT2;

	strcpy(m_ClassName,"Static");
	m_ClassScript[0] = 0;
	m_Position.identity();

	m_MakeUnique = true;
	m_DynamicList = false;
}

void SExpObject2::ClearMeshes (){
// clear all previous created meshes
    for(SObjMeshIt m_src = m_Meshes.begin();m_src!=m_Meshes.end();m_src++) SAFE_DELETE(*m_src);
	m_Meshes.clear();
}

SExpObject2::~SExpObject2(){
    ClearMeshes();
}

void SExpObject2::Save( int handle ){

	FSChunkDef chunk_level0;
	FSChunkDef chunk_level1;
	FS.initchunk(&chunk_level0,handle);
	FS.initchunk(&chunk_level1,handle);

	FS.wopenchunk(&chunk_level0,SOBJ_CHUNK_CLASS_SCRIPT);
	FS.writestring(handle,m_ClassScript);
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,SOBJ_CHUNK_POSITION);
	FS.write(handle,&m_Position,sizeof(m_Position));
	FS.wclosechunk(&chunk_level0);

	FS.wopenchunk(&chunk_level0,SOBJ_CHUNK_BOP);
	FS.writeword(handle,m_MakeUnique);
	FS.writeword(handle,m_DynamicList);
	FS.wclosechunk(&chunk_level0);

	SObjMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++){
		FS.wopenchunk(&chunk_level0,SOBJ_CHUNK_MESH);

			FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_NAME);
			FS.writestring(handle,(*m)->m_Name);
			FS.wclosechunk(&chunk_level1);

			FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_FILE);
			FS.writestring(handle,(*m)->m_FileName);
			FS.wclosechunk(&chunk_level1);

			FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_POS);
			FS.write(handle,&(*m)->m_Position,sizeof((*m)->m_Position));
			FS.wclosechunk(&chunk_level1);

			FS.wopenchunk(&chunk_level1,SOBJ_CHUNK_MESH_CORE);
			(*m)->m_Mesh->SaveMesh( handle, false );
			FS.wclosechunk(&chunk_level1);

		FS.wclosechunk(&chunk_level0);
	}
}

