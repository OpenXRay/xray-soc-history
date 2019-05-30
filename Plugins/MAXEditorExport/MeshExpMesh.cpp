// file: MeshExpMesh.cpp

#include "Pch.h"
#pragma hdrstop

#include "MeshExpMesh.h"
#include "FileSystem.h"

//-------------------------------------------------------------------

MMaterial::MMaterial(){
	m_FaceStart = 0;
	m_FaceCount = 0;
	m_TexName[0] = 0;
	m_Ambient.set(0,0,0,0);
	m_Diffuse.set(1,1,1,1);
	m_Specular.set(0,0,0,0);
	m_Emission.set(0,0,0,0);
	m_Power = 0;
}

MMaterial::MMaterial( MMaterial *source ){
	m_FaceStart = source->m_FaceStart;
	m_FaceCount = source->m_FaceCount;
	strcpy(m_TexName,source->m_TexName);
	m_Ambient.set(source->m_Ambient);
	m_Diffuse.set(source->m_Diffuse);
	m_Specular.set(source->m_Specular);
	m_Emission.set(source->m_Emission);
	m_Power = source->m_Power;
}

MMaterial::~MMaterial(){
}

//-------------------------------------------------------------------


ExpMesh::ExpMesh(){
}

ExpMesh::~ExpMesh(){
	m_Materials.clear();
	m_Points.clear();
	m_Faces.clear();
}

//-------------------------------------------------------------------

bool ExpMesh::Verify(){
	return true;
}

//-------------------------------------------------------------------

void ExpMesh::Append( ExpMesh *source ){

	_ASSERTE( source );

	vector<MMaterial> amaterials;
	vector<MPoint> apoints;
	vector<MFace> afaces;

	amaterials.insert(amaterials.end(),source->m_Materials.begin(),source->m_Materials.end());
	apoints.insert(apoints.end(),source->m_Points.begin(),source->m_Points.end() );
	afaces.insert(afaces.end(),source->m_Faces.begin(),source->m_Faces.end() );

	for( int i=0; i<amaterials.size(); i++)
		amaterials[i].m_FaceStart += m_Faces.size();

	for(i=0; i<afaces.size(); i++)
		for(int k=0;k<3;k++)
			afaces[i].p[k] += m_Points.size();

	m_Materials.insert(m_Materials.end(),amaterials.begin(),amaterials.end());
	m_Points.insert(m_Points.end(),apoints.begin(),apoints.end());
	m_Faces.insert(m_Faces.end(),afaces.begin(),afaces.end());

	amaterials.clear();
	apoints.clear();
	afaces.clear();
}

//----------------------------------------------------

void ExpMesh::Copy( ExpMesh *source ){
	m_Materials.insert(m_Materials.end(),source->m_Materials.begin(),source->m_Materials.end());
	m_Points.insert(m_Points.end(),source->m_Points.begin(),source->m_Points.end() );
	m_Faces.insert(m_Faces.end(),source->m_Faces.begin(),source->m_Faces.end() );
}

//----------------------------------------------------

void ExpMesh::GetMaterial( ExpMesh *dest, MMaterial *material ){
	_ASSERTE( dest );

	int fstface = 0;
	vector<int> pointidx;

	pointidx.resize( m_Points.size() );
	for(int i=0;i<pointidx.size();i++)
		pointidx[i] = 0;

	for( i=0; i<material->m_FaceCount; i++)
		for(int k=0;k<3;k++)
			pointidx[m_Faces[material->m_FaceStart+i].p[k]]=1;

	for( i=0;i<m_Points.size();i++){
		if( pointidx[i] ){
			pointidx[i] = dest->m_Points.size();
			dest->m_Points.push_back( m_Points[i] );
		} else {
			pointidx[i] = -1;
		}
	}

	fstface = dest->m_Faces.size();
	for( i=0; i<material->m_FaceCount;i++){
		dest->m_Faces.push_back( m_Faces[material->m_FaceStart+i] );
		for(int k=0;k<3;k++)
			dest->m_Faces.back().p[k] = pointidx[dest->m_Faces.back().p[k]];
	}

	dest->m_Materials.push_back( (*material) );
	dest->m_Materials.back().m_FaceStart += fstface;

	pointidx.clear();
}

//-------------------------------------------------------------------

void ExpMesh::Transform( Fmatrix& parent ){
	for( int i=0; i<m_Points.size(); i++){
		parent.transform(m_Points[i].m_Point);
		parent.shorttransform(m_Points[i].m_Normal);
	}
}

void ExpMesh::Renormalize(){
	for( int i=0; i<m_Points.size(); i++)
		m_Points[i].m_Normal.normalize_safe();
}

void ExpMesh::FlipFaces(){
	for( int i=0; i<m_Faces.size(); i++)
		swap( m_Faces[i].p0, m_Faces[i].p2 );
}

void ExpMesh::FlipNormals(){
	for( int i=0; i<m_Points.size(); i++)
		m_Points[i].m_Normal.mul(-1.f);
}

//-------------------------------------------------------------------

int ExpMesh::CreateUniqueFace( MPoint *points ){

	MFace mface;
	mface.p0 = 0xffff;
	mface.p1 = 0xffff;
	mface.p2 = 0xffff;

	for(int i=0;i<m_Points.size();i++){
		for(int k=0; k<3; k++){
			if( m_Points[i].m_Point.similar(points[k].m_Point) && 
				m_Points[i].m_Normal.similar(points[k].m_Normal) && 
				m_Points[i].m_TexCoord.similar(points[k].m_TexCoord) )
			{
					mface.p[k] = i;
			}
		}

		if(	(mface.p0 != 0xffff) && 
			(mface.p1 != 0xffff) && 
			(mface.p2 != 0xffff) ) break;
	}

	for(int k=0; k<3; k++ ){
		if( mface.p[k] == 0xffff ){
			mface.p[k] = m_Points.size();
			m_Points.push_back( points[k] );
		}
	}

	m_Faces.push_back( mface );
	return (m_Faces.size()-1);
}

//-------------------------------------------------------------------

void ExpMesh::Save( char *filename ){

	_ASSERTE( filename );
	int handle = FS.create( filename );

	FSChunkDef mesh_chunk;
	FSChunkDef version_chunk;
	FSChunkDef header_chunk;
	FSChunkDef pointlist_chunk;
	FSChunkDef facelist_chunk;
	FSChunkDef material_chunk;
	FSChunkDef materialcolor_chunk;
	FSChunkDef materialtname_chunk;

	FS.initchunk( &mesh_chunk, handle );
	FS.initchunk( &version_chunk, handle );
	FS.initchunk( &header_chunk, handle );
	FS.initchunk( &pointlist_chunk, handle );
	FS.initchunk( &facelist_chunk, handle );
	FS.initchunk( &material_chunk, handle );
	FS.initchunk( &materialcolor_chunk, handle );
	FS.initchunk( &materialtname_chunk, handle );

	FS.wopenchunk( &mesh_chunk, STM_CHUNK_MESH );

		FS.wopenchunk( &version_chunk, STM_CHUNK_VERSION );
		FS.wclosechunk( &version_chunk );

		FS.wopenchunk( &header_chunk, STM_CHUNK_HEADER );
		FS.wclosechunk( &header_chunk );

		FS.wopenchunk( &pointlist_chunk, STM_CHUNK_POINTLIST );
		FS.write( handle, m_Points.begin(), sizeof(MPoint)*m_Points.size() );
		FS.wclosechunk( &pointlist_chunk );

		FS.wopenchunk( &facelist_chunk, STM_CHUNK_FACELIST );
		FS.write( handle, m_Faces.begin(), sizeof(MFace)*m_Faces.size() );
		FS.wclosechunk( &facelist_chunk );

		MMaterial *material = m_Materials.begin();
		for(;material!=m_Materials.end();material++){
			FS.wopenchunk( &material_chunk, STM_CHUNK_MATERIAL );
			
			FS.writedword( handle, material->m_FaceStart );
			FS.writedword( handle, material->m_FaceCount );

			FS.wopenchunk( &materialcolor_chunk, STM_CHUNK_MATERIAL_COLOR );
//			FS.writecolor4( handle, material->m_Ambient.val );
//			FS.writecolor4( handle, material->m_Diffuse.val );
//			FS.writecolor4( handle, material->m_Specular.val );
//			FS.writecolor4( handle, material->m_Emission.val );
			FS.writefloat( handle, material->m_Power );
			FS.wclosechunk( &materialcolor_chunk );

			if( material->m_TexName[0] ){
				FS.wopenchunk( &materialtname_chunk, STM_CHUNK_MATERIAL_TNAME );
				FS.writestring( handle, material->m_TexName );
				FS.wclosechunk( &materialtname_chunk );
			}

			FS.wclosechunk( &material_chunk );
		}

	FS.wclosechunk( &mesh_chunk );
	FS.close( handle );
}

//-------------------------------------------------------------------

