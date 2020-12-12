// file: MeshExpConvert.cpp

#include "Pch.h"
#pragma hdrstop

#include "MeshExpUtility.h"
#include "FileSystem.h"

//-------------------------------------------------------------------

TriObject *MeshExpUtility::ExtractTriObject( INode *node, int &deleteIt )
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(0).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) { 
		TriObject *tri = (TriObject *) obj->ConvertToType(0, 
			Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri) deleteIt = TRUE;
		return tri;
	}
	else {
		return NULL;
	}

//	ObjectState state = node->EvalWorldState(0,FALSE);
//	if( state.obj->ClassID() == Class_ID(EDITTRIOBJ_CLASS_ID,0) )
//		return (TriObject *)state.obj;
//	return 0;
}

bool MeshExpUtility::ConvertMaxMesh( ExpMesh *dest, INode *node ){

	// prepares & checks
	BOOL bDeleteObj;
	TriObject *obj = ExtractTriObject( node, bDeleteObj );

	if( !obj ){
		NConsole.print( "%s -> Can't convert to TriObject", node->GetName() );
		return false; }

	if( obj->mesh.getNumFaces() <=0 ){
		NConsole.print( "%s -> There are no faces ?", node->GetName() );
		if (bDeleteObj) SAFE_DELETE(obj);
		return false; }

	MPoint facepoints[3];
	vector<int> facepermut;
	facepermut.resize( obj->mesh.getNumFaces() );

	// create lighting/geometry model
	//  take into account SM groups & UV channels !

	obj->mesh.buildRenderNormals();
/*	if( !obj->mesh.normalsBuilt ){
		NConsole.print( "%s -> Can't build normals", node->GetName() );
		return false; }
*/
	for( int i=0; i<obj->mesh.getNumFaces(); i++){

		TVFace *_tvF = 0;
		if( obj->mesh.mapFaces(1) )
			_tvF = obj->mesh.mapFaces(1) + i;

		Face* _F = obj->mesh.faces + i;
		int smGroup = _F->getSmGroup();

		// position
		for( int k=0; k<3; k++){
			Point3 *_Pt = obj->mesh.verts + _F->v[k];
			facepoints[k].m_Point.set( _Pt->x, _Pt->y, _Pt->z );
		}

		for( k=0; k<3; k++){
			// texture coords
			if( _tvF ){
				UVVert *_uv = obj->mesh.mapVerts(1) + _tvF->t[k];
				facepoints[k].m_TexCoord.x = _uv->x;
				facepoints[k].m_TexCoord.y = -_uv->y;
				facepoints[k].m_TexCoord.z = _uv->z;
			} else {
				facepoints[k].m_TexCoord.set(0,0,0);
			}

			// normals
			if( smGroup ){
				RVertex * _RVtx = obj->mesh.getRVertPtr(_F->v[k]);
				int nbNormals = _RVtx->rFlags & NORCT_MASK;
				if (nbNormals == 1){
					Point3& _N = _RVtx->rn.getNormal();
					facepoints[k].m_Normal.set( _N.x, _N.y, _N.z );
				} else {
					for (int j = 0; j < nbNormals; j++) {
						RNormal* rn = _RVtx->ern + j;
						if (rn->getSmGroup() == smGroup) {
							Point3& _N = rn->getNormal();
							facepoints[k].m_Normal.set( _N.x, _N.y, _N.z );
							break;
						}
					}
				}
			} else {
				facepoints[k].m_Normal.mknormal(
					facepoints[0].m_Point,
					facepoints[1].m_Point,
					facepoints[2].m_Point );
			}
		}

		facepermut[dest->CreateUniqueFace( facepoints )] = i;
	}

	for (i=0;i<facepermut.size();i++){
		int y = facepermut[i];
		int j = i;
	}
	NConsole.print( "'%s' -> model: %d points, %d faces",
		node->GetName(), dest->m_Points.size(), dest->m_Faces.size());

	// resort faces (bubbles:))
	//  use matids for sorting keys

	for(i=0;i<dest->m_Faces.size();i++){
		int mid0 = obj->mesh.getFaceMtlIndex(facepermut[i]);
		for(int j=i+2;j<dest->m_Faces.size();j++)
			if( obj->mesh.getFaceMtlIndex(facepermut[j]) == mid0 ){
				std::swap( dest->m_Faces[i+1], dest->m_Faces[j] );
				std::swap( facepermut[i+1], facepermut[j] );
			}
	}

	// create 'surfaceted' subdivision
	//  unused materials excluded by this algo

	dest->m_Materials.push_back( MMaterial() );
	dest->m_Materials.back().m_FaceStart = 0;
	dest->m_Materials.back().m_FaceCount = 1;

	int prevMatId = obj->mesh.getFaceMtlIndex(facepermut[0]);
	for(i=1;i<dest->m_Faces.size();i++){
		if( prevMatId != obj->mesh.getFaceMtlIndex(facepermut[i]) ){
			prevMatId = obj->mesh.getFaceMtlIndex(facepermut[i]);
			dest->m_Materials.push_back( MMaterial() );
			dest->m_Materials.back().m_FaceStart = i;
			dest->m_Materials.back().m_FaceCount = 1;
		}
		else
			dest->m_Materials.back().m_FaceCount++;
	}

	// update materials
	//  (add textures & color info)

	Mtl *mtl = node->GetMtl();
	if( mtl ){
		
		if( mtl->ClassID() == Class_ID(MULTI_CLASS_ID,0) ){
			NConsole.print( "'%s' -> multi material '%s' ...", node->GetName(), mtl->GetName() );

			MultiMtl *mmtl = (MultiMtl*)mtl;
			
			MMaterial *material = dest->m_Materials.begin();
			for(;material!=dest->m_Materials.end();material++){
				
				int mtlindex = obj->mesh.getFaceMtlIndex(facepermut[material->m_FaceStart]);
				
				if( mmtl->GetSubMtl(mtlindex)->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
					StdMat *smtl = (StdMat*)mmtl->GetSubMtl(mtlindex);
					ExtractStdMaterial( material, smtl );

				} else {
					NConsole.print( "'%s' -> warning: bad submaterial '%s'", 
						node->GetName(), mmtl->GetSubMtl(mtlindex)->GetName() );
				}
			}
		
		} else if( mtl->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
			NConsole.print( "'%s' -> std material '%s' ...", node->GetName(), mtl->GetName() );
			
			StdMat *smtl = (StdMat*)mtl;

			MMaterial *material = dest->m_Materials.begin();
			for(;material!=dest->m_Materials.end();material++)
				ExtractStdMaterial( material, smtl );

		} else {
			NConsole.print( "'%s' -> unknown material class, (using defaults) ...", node->GetName() );
		}

	} else {
		NConsole.print( "'%s' -> warning: no material", node->GetName() );
	}

	if (bDeleteObj) SAFE_DELETE(obj);

	return true;
}

bool MeshExpUtility::ConvertMaxMesh( LayerMesh *dest, INode *node ){

	// prepares & checks
	BOOL bDeleteObj;
	TriObject *obj = ExtractTriObject( node, bDeleteObj );

	if( !obj ){
		NConsole.print( "%s -> Can't convert to TriObject", node->GetName() );
		return false; }

	if( obj->mesh.getNumFaces() <=0 ){
		NConsole.print( "%s -> There are no faces ?", node->GetName() );
		if (bDeleteObj) SAFE_DELETE(obj);
		return false; }

	SPoint facepoints[3];
	SUV_tri uv_tri;
	// create lighting/geometry model
	//  take into account SM groups & UV channels !
	obj->mesh.buildRenderNormals();
/*	if( !obj->mesh.normalsBuilt ){
		NConsole.print( "%s -> Can't build normals", node->GetName() );
		return false; }
*/

	for( int i=0; i<obj->mesh.getNumFaces(); i++){
		TVFace *_tvF = 0;
		if( obj->mesh.mapFaces(1) )	_tvF = obj->mesh.mapFaces(1) + i;

		Face* _F = obj->mesh.faces + i;
		int smGroup = _F->getSmGroup();

		// position
		for( int k=0; k<3; k++){
			Point3 *_Pt = obj->mesh.verts + _F->v[k];
			facepoints[k].m_Point.set( _Pt->x, _Pt->y, _Pt->z );
		}

//		int mid0 = obj->mesh.getFaceMtlIndex(i);

		for( k=0; k<3; k++){
			// texture coords
			if( _tvF ){
				UVVert *_uv = obj->mesh.mapVerts(1) + _tvF->t[k];
				uv_tri.uv[k].tu = _uv->x;
				uv_tri.uv[k].tv = -_uv->y;
			} else {
				uv_tri.uv[k].tu = 0;
				uv_tri.uv[k].tv = 0;
			}

			// normals
			if( smGroup ){
				RVertex * _RVtx = obj->mesh.getRVertPtr(_F->v[k]);
				int nbNormals = _RVtx->rFlags & NORCT_MASK;
				if (nbNormals == 1){
					Point3& _N = _RVtx->rn.getNormal();
					facepoints[k].m_Normal.set( _N.x, _N.y, _N.z );
				} else {
					for (int j = 0; j < nbNormals; j++) {
						RNormal* rn = _RVtx->ern + j;
						if (rn->getSmGroup() == smGroup) {
							Point3& _N = rn->getNormal();
							facepoints[k].m_Normal.set( _N.x, _N.y, _N.z );
							break;
						}
					}
				}
			} else {
				facepoints[k].m_Normal.mknormal(
					facepoints[0].m_Point,
					facepoints[1].m_Point,
					facepoints[2].m_Point );
			}
		}

		dest->CreateUniqueFace( facepoints );
	}
/*
	NConsole.print( "'%s' -> model: %d points, %d faces",
		node->GetName(), dest->m_Points.size(), dest->m_Faces.size());

	// resort faces (bubbles:))
	//  use matids for sorting keys

	for(i=0;i<dest->m_Faces.size();i++){
		int mid0 = obj->mesh.getFaceMtlIndex(facepermut[i]);
		for(int j=i+2;j<dest->m_Faces.size();j++)
			if( obj->mesh.getFaceMtlIndex(facepermut[j]) == mid0 ){
				std::swap( dest->m_Faces[i+1], dest->m_Faces[j] );
				std::swap( facepermut[i+1], facepermut[j] );
			}
	}

	// create 'surfaceted' subdivision
	//  unused materials excluded by this algo

	dest->m_Materials.push_back( MMaterial() );
	dest->m_Materials.back().m_FaceStart = 0;
	dest->m_Materials.back().m_FaceCount = 1;

	int prevMatId = obj->mesh.getFaceMtlIndex(facepermut[0]);
	for(i=1;i<dest->m_Faces.size();i++){
		if( prevMatId != obj->mesh.getFaceMtlIndex(facepermut[i]) ){
			prevMatId = obj->mesh.getFaceMtlIndex(facepermut[i]);
			dest->m_Materials.push_back( MMaterial() );
			dest->m_Materials.back().m_FaceStart = i;
			dest->m_Materials.back().m_FaceCount = 1;
		}
		else
			dest->m_Materials.back().m_FaceCount++;
	}

	// update materials
	//  (add textures & color info)

	Mtl *mtl = node->GetMtl();
	if( mtl ){
		
		if( mtl->ClassID() == Class_ID(MULTI_CLASS_ID,0) ){
			NConsole.print( "'%s' -> multi material '%s' ...", node->GetName(), mtl->GetName() );

			MultiMtl *mmtl = (MultiMtl*)mtl;
			
			MMaterial *material = dest->m_Materials.begin();
			for(;material!=dest->m_Materials.end();material++){
				
				int mtlindex = obj->mesh.getFaceMtlIndex(facepermut[material->m_FaceStart]);
				
				if( mmtl->GetSubMtl(mtlindex)->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
					StdMat *smtl = (StdMat*)mmtl->GetSubMtl(mtlindex);
					ExtractStdMaterial( material, smtl );

				} else {
					NConsole.print( "'%s' -> warning: bad submaterial '%s'", 
						node->GetName(), mmtl->GetSubMtl(mtlindex)->GetName() );
				}
			}
		
		} else if( mtl->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
			NConsole.print( "'%s' -> std material '%s' ...", node->GetName(), mtl->GetName() );
			
			StdMat *smtl = (StdMat*)mtl;

			MMaterial *material = dest->m_Materials.begin();
			for(;material!=dest->m_Materials.end();material++)
				ExtractStdMaterial( material, smtl );

		} else {
			NConsole.print( "'%s' -> unknown material class, (using defaults) ...", node->GetName() );
		}

	} else {
		NConsole.print( "'%s' -> warning: no material", node->GetName() );
	}

*/
	if (bDeleteObj) SAFE_DELETE(obj);
	return true;
}
//-------------------------------------------------------------------

bool MeshExpUtility::ExtractStdMaterial( MMaterial *dest, StdMat *smtl ){

	// ---- color info

	Color ambient = smtl->GetAmbient(0);
	Color diffuse = smtl->GetDiffuse(0);
	Color specular = smtl->GetSpecular(0);
	Color emission = smtl->GetSelfIllumColor(0);
	float st = smtl->GetShinStr(0);
	float power = smtl->GetShininess(0)*100.f;

	dest->m_Ambient.set( ambient.r, ambient.g, ambient.b, 0 );
	dest->m_Diffuse.set( diffuse.r, diffuse.g, diffuse.b, 1 );
	dest->m_Specular.set( st*specular.r, st*specular.g, st*specular.b, 1 );
	dest->m_Emission.set( emission.r, emission.g, emission.b, 1 );
	dest->m_Power = power;

	// ------- texture (if exist)

	if( smtl->MapEnabled( ID_AM ) ){
		if( smtl->GetSubTexmap( ID_AM ) )
			ExtractTexName( dest->m_TexName, smtl->GetSubTexmap( ID_AM ) );

	} else if( smtl->MapEnabled( ID_DI ) ){
		if( smtl->GetSubTexmap( ID_DI ) )
			ExtractTexName( dest->m_TexName, smtl->GetSubTexmap( ID_DI ) );
	}

	return true;
}

bool MeshExpUtility::ExtractStdMaterial( CTextureLayer *dest, StdMat *smtl ){

	// ---- color info
/*
	Color ambient = smtl->GetAmbient(0);
	Color diffuse = smtl->GetDiffuse(0);
	Color specular = smtl->GetSpecular(0);
	Color emission = smtl->GetSelfIllumColor(0);
	float st = smtl->GetShinStr(0);
	float power = smtl->GetShininess(0)*100.f;

	dest->m_Ambient.set( ambient.r, ambient.g, ambient.b, 0 );
	dest->m_Diffuse.set( diffuse.r, diffuse.g, diffuse.b, 1 );
	dest->m_Specular.set( st*specular.r, st*specular.g, st*specular.b, 1 );
	dest->m_Emission.set( emission.r, emission.g, emission.b, 1 );
	dest->m_Power = power;

	// ------- texture (if exist)

	if( smtl->MapEnabled( ID_AM ) ){
		if( smtl->GetSubTexmap( ID_AM ) )
			ExtractTexName( dest->m_TexName, smtl->GetSubTexmap( ID_AM ) );

	} else if( smtl->MapEnabled( ID_DI ) ){
		if( smtl->GetSubTexmap( ID_DI ) )
			ExtractTexName( dest->m_TexName, smtl->GetSubTexmap( ID_DI ) );
	}
*/
	return true;
}
//-------------------------------------------------------------------

bool MeshExpUtility::ExtractTexName( char *dest, Texmap *map ){
	if( map->ClassID() != Class_ID(BMTEX_CLASS_ID,0) )
		return false;
	BitmapTex *bmap = (BitmapTex*)map;
	_splitpath( bmap->GetMapName(), 0, 0, dest, 0 );
	_splitpath( bmap->GetMapName(), 0, 0, 0, dest + strlen(dest) );
	return true;
}

//-------------------------------------------------------------------

