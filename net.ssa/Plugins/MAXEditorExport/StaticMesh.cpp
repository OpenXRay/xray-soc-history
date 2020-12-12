//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "StaticMesh.h"
#include "FileSystem.h"
#include "StaticMeshChunks.h"
#include "MeshExpUtility.h"
//----------------------------------------------------
#define MK_CLSID(a,b,c,d,e,f,g,h) \
	CLASS_ID(	((CLASS_ID(a)<<24)|(CLASS_ID(b)<<16)|(CLASS_ID(c)<<8)|(CLASS_ID(d)))<<32 | \
((CLASS_ID(e)<<24)|(CLASS_ID(f)<<16)|(CLASS_ID(g)<<8)|(CLASS_ID(h))) )
//----------------------------------------------------
static Fmaterial static_mat={{1.f,1.f,1.f,1.f},
{0,0,0,0},
{0,0,0,0},
{0,0,0,0},
0};
void CLSID2TEXT(CLASS_ID id, char *text) {
	text[8]=0;
	for (int i=7; i>=0; i--) { text[i]=char(id&0xff); id>>=8; }
}
CLASS_ID TEXT2CLSID(char *text) {
	char buf[10];
	strncpy(buf,text,9);
	int need = 8-strlen(buf);
	while (need) { buf[8-need]=' '; need--; }
	return MK_CLSID(buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
}
//----------------------------------------------------------------------------

CTextureLayer::CTextureLayer(ExpMesh* parent){
	m_Parent		= parent;
    sprintf			(m_LayerName,"Layer %d",parent->m_Layers.size());
	m_TexName[0]	= 0;
    m_Script[0]		= 0;
    m_SOP			= sopBlend;
    m_CLS			= TEXT2CLSID("STD");
    m_Mat.diffuse.set(1,1,1,1);
    m_Mat.ambient.set(0,0,0,0);
    m_Mat.specular.set(0,0,0,0);
    m_Mat.emissive.set(0,0,0,0);
    m_Mat.power		= 0;
}
//----------------------------------------------------------------------------

CTextureLayer::~CTextureLayer(){
}
//----------------------------------------------------------------------------

bool CTextureLayer::AddFace(DWORD face_id, SUV_tri* uv)
{
    VERIFY(face_id<m_Parent->m_GFaces.size());
    if (uv) m_UVMaps.push_back(SUV_tri((SUV*)uv));
    else    m_UVMaps.push_back(SUV_tri());
    m_GFaceIDs.push_back(face_id);
	
    return true;
}
//----------------------------------------------------------------------------

bool CTextureLayer::AddFaceUnique(DWORD face_id, SUV_tri* uv)
{
    VERIFY(face_id<m_Parent->m_GFaces.size());
    if (m_GFaceIDs.end() != find(m_GFaceIDs.begin(),m_GFaceIDs.end(),face_id)) return false;
	AddFace(face_id,uv);
    return true;
}
//----------------------------------------------------------------------------

void CTextureLayer::Copy( CTextureLayer* source, DWORD face_offs, bool bCopyData )
{
    strcpy(m_LayerName, source->m_LayerName);
	strcpy(m_TexName, source->m_TexName);
	strcpy(m_Script, source->m_Script);
    m_CLS		= source->m_CLS;
    m_Mat.set	(source->m_Mat);
    m_SOP		= source->m_SOP;
	
	if (bCopyData){
		m_UVMaps.insert		(m_UVMaps.end(),	source->m_UVMaps.begin(),	source->m_UVMaps.end());
		m_GFaceIDs.insert	(m_GFaceIDs.end(),	source->m_GFaceIDs.begin(),	source->m_GFaceIDs.end());
		if (face_offs)
			for (DWORDIt id=m_GFaceIDs.begin(); id!=m_GFaceIDs.end(); id++)	(*id) += face_offs;
	}
}
//----------------------------------------------------------------------------
void CTextureLayer::FlipFaces()
{
	for(UVFaceIt uv = m_UVMaps.begin(); uv!=m_UVMaps.end(); uv++)
		swap((*uv).uv[0], (*uv).uv[2]);
}







//----------------------------------------------------------------------------
// Static Mesh
//----------------------------------------------------------------------------
ExpMesh::ExpMesh()
{
	VMmin.set(0,0,0);
	VMscale.set(0,0,0);
}
//----------------------------------------------------------------------------

ExpMesh::~ExpMesh(){
	m_Vertices.clear();
	m_GFaces.clear();
	
    for(TLayerIt l=m_Layers.begin(); l!=m_Layers.end();l++) SAFE_DELETE((*l));
    m_Layers.clear();
}
//----------------------------------------------------------------------------

bool ExpMesh::UpdateTies(){
	bool bOk=true;
    // create m_FTies
    vector< vector<DWORD> > VF;
    VF.resize(m_Vertices.size());
	int f_id=0;
    for (FaceIt it=m_GFaces.begin(); it!=m_GFaces.end(); it++,f_id++ ){
		VF[it->p0].push_back(f_id);
		VF[it->p1].push_back(f_id);
		VF[it->p2].push_back(f_id);
    }
	
    m_FTies.resize(m_GFaces.size());
    FTieIt ft_it=m_FTies.begin();
   	int *isect, *isect_end;
	isect=new int[m_GFaces.size()];
	f_id=0;
    for (FaceIt f_it=m_GFaces.begin(); f_it!=m_GFaces.end(); f_it++,ft_it++,f_id++ ){
		for(int i1=0,i2,itm; i1<3; i1++){
			i2=(i1+1)%3;
            isect_end = set_intersection(	VF[f_it->p[i1]].begin(), VF[f_it->p[i1]].end(),
											VF[f_it->p[i2]].begin(), VF[f_it->p[i2]].end(),
											isect);
            itm=isect_end-isect;
			if (itm>2){
				NConsole.print( "Face '%d' has '%d' ties.", f_id, itm );
				bOk=false;
			}
			// если элементов <=2 и не равен сам себе
			ft_it->t[i1]=(itm==2)?((isect[1]==f_id)?isect[0]:isect[1]):-1;
			VERIFY((*ft_it).t[i1]!=f_id);
        }
    }
	delete []isect;
	return bOk;
}
//----------------------------------------------------

CTextureLayer* ExpMesh::AddTextureLayer()
{
    CTextureLayer* l = new CTextureLayer(this);  VERIFY(l);
    m_Layers.push_back(l);
    return l;
}
//----------------------------------------------------------------------------

void ExpMesh::Renormalize(){
	for(PointIt v = m_Vertices.begin(); v!=m_Vertices.end(); v++)
		(*v).m_Normal.normalize_safe();
}
//----------------------------------------------------------------------------

void ExpMesh::FlipFaces(){
	for(FaceIt f = m_GFaces.begin(); f!=m_GFaces.end(); f++)
		swap((*f).p0, (*f).p2);
    for(TLayerIt l=m_Layers.begin(); l!=m_Layers.end();l++)
		(*l)->FlipFaces();
}
//----------------------------------------------------------------------------

void ExpMesh::FlipNormals(){
	for(PointIt v = m_Vertices.begin(); v!=m_Vertices.end(); v++)
		(*v).m_Normal.mul(-1.f);
}
//----------------------------------------------------------------------------

void ExpMesh::CreateFace( SPoint *points, bool bTestNormal ){
	SFace mface;
	mface.p0 = 0xffff;
	mface.p1 = 0xffff;
	mface.p2 = 0xffff;
	
	int k;
	
	// get similar vert idx list
	for (k=0; k<3; k++){
		DWORDList* vl; 
		int ix,iy,iz;
		ix = floorf(float(points[k].m_Point.x-VMmin.x)/VMscale.x*MX);
		iy = floorf(float(points[k].m_Point.y-VMmin.y)/VMscale.y*MY);
		iz = floorf(float(points[k].m_Point.z-VMmin.z)/VMscale.z*MZ);
		vl = &(VM[ix][iy][iz]);
		for(DWORDIt it=vl->begin();it!=vl->end(); it++){ 
			PointIt v = m_Vertices.begin()+(*it);
			if (bTestNormal){
				if( v->m_Point.similar(points[k].m_Point,EPS)&&v->m_Normal.similar(points[k].m_Normal,EPS))
					mface.p[k] = *it;
			}else{
				if( v->m_Point.similar(points[k].m_Point,EPS) )
					mface.p[k] = *it;
			}
		}
	}
	//&& (*v).m_Normal.similar(points[k].m_Normal
	/*
	for(PointIt v = m_Vertices.begin(); v!=m_Vertices.end(); v++){
	if( (*v).m_Point.similar(points[0].m_Point) && (*v).m_Normal.similar(points[0].m_Normal) ){ 
	mface.p[0] = v - m_Vertices.begin();
	if(	(mface.p1 != 0xffff) && (mface.p2 != 0xffff) ) break;
	}
	if( (*v).m_Point.similar(points[1].m_Point) && (*v).m_Normal.similar(points[1].m_Normal) ){ 
	mface.p[1] = v - m_Vertices.begin();
	if(	(mface.p0 != 0xffff) && (mface.p2 != 0xffff) ) break;
	}
	if( (*v).m_Point.similar(points[2].m_Point) && (*v).m_Normal.similar(points[2].m_Normal) ){	
	mface.p[2] = v - m_Vertices.begin();
	if(	(mface.p0 != 0xffff) && (mface.p1 != 0xffff) ) break;
	}
	}
	*/
	for(k=0; k<3; k++ ){
		if( mface.p[k] == 0xffff ){
			mface.p[k] = m_Vertices.size();
			m_Vertices.push_back( points[k] );
			int ix,iy,iz;
			ix = floorf(float(points[k].m_Point.x-VMmin.x)/VMscale.x*MX);
			iy = floorf(float(points[k].m_Point.y-VMmin.y)/VMscale.y*MY);
			iz = floorf(float(points[k].m_Point.z-VMmin.z)/VMscale.z*MZ);
			VM[ix][iy][iz].push_back(mface.p[k]);
			int ixE,iyE,izE;
			ixE = floorf(float(points[k].m_Point.x+VMeps.x-VMmin.x)/VMscale.x*MX);
			iyE = floorf(float(points[k].m_Point.y+VMeps.y-VMmin.y)/VMscale.y*MY);
			izE = floorf(float(points[k].m_Point.z+VMeps.z-VMmin.z)/VMscale.z*MZ);
			if (ixE!=ix)				
				VM[ixE][iy][iz].push_back(mface.p[k]);
			if (iyE!=iy)				
				VM[ix][iyE][iz].push_back(mface.p[k]);
			if (izE!=iz)				
				VM[ix][iy][izE].push_back(mface.p[k]);
			if ((ixE!=ix)&&(iyE!=iy))	
				VM[ixE][iyE][iz].push_back(mface.p[k]);
			if ((ixE!=ix)&&(izE!=iz))	
				VM[ixE][iy][izE].push_back(mface.p[k]);
			if ((iyE!=iy)&&(izE!=iz))	
				VM[ix][iyE][izE].push_back(mface.p[k]);
			if ((ixE!=ix)&&(iyE!=iy)&&(izE!=iz))
				VM[ixE][iyE][izE].push_back(mface.p[k]);
		}
	}
	
	if ((mface.p0==mface.p1)||(mface.p1==mface.p2)||(mface.p0==mface.p2)){
		NConsole.print( "Face missing." );
	}else{
		m_GFaces.push_back( mface );
	}
}
//----------------------------------------------------------------------------

TriObject *ExpMesh::ExtractTriObject( INode *node, int &deleteIt )
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
	}else{
		return NULL;
	}
}
//----------------------------------------------------------------------------

bool ExpMesh::ExtractMaterial( CTextureLayer *dest, StdMat *smtl ){
	// ---- color info
	Color ambient		= smtl->GetAmbient(0);
	Color diffuse		= smtl->GetDiffuse(0);
	Color specular		= smtl->GetSpecular(0);
	Color emission		= smtl->GetSelfIllumColor(0);
	float st			= smtl->GetShinStr(0);
	float power			= smtl->GetShininess(0)*100.f;
	
	dest->m_Mat.ambient.set( ambient.r, ambient.g, ambient.b, 0 );
	dest->m_Mat.diffuse.set( diffuse.r, diffuse.g, diffuse.b, 1 );
	dest->m_Mat.specular.set( st*specular.r, st*specular.g, st*specular.b, 1 );
	dest->m_Mat.emissive.set( emission.r, emission.g, emission.b, 1 );
	dest->m_Mat.power = power;
	
	// ------- texture (if exist)
	if( smtl->MapEnabled( ID_AM ) ){
		if( smtl->GetSubTexmap( ID_AM ) ){
			if (!ExtractTexName( dest->m_TexName, smtl->GetSubTexmap( ID_AM ) )) return false;
		}else{
			return false;
		}
	}else 
		if( smtl->MapEnabled( ID_DI ) ){
			if( smtl->GetSubTexmap( ID_DI ) ){
				if (!ExtractTexName( dest->m_TexName, smtl->GetSubTexmap( ID_DI ) )) return false;
			}else{
				return false;
			}
		}else{
			return false;
		}
		if( smtl->GetTwoSided() ){
			if (theMeshExpUtility.m_Duplicate2Sided) 
				Duplicate2Sided(dest, smtl->GetName());
			else{ 
				NConsole.print( "Object contains 2-side material '%s' ...", smtl->GetName() );
				return false;
			}
		}
		return true;
}
//----------------------------------------------------------------------------

bool ExpMesh::ExtractTexName( char *dest, Texmap *map ){
	if( map->ClassID() != Class_ID(BMTEX_CLASS_ID,0) )
		return false;
	BitmapTex *bmap = (BitmapTex*)map;
	_splitpath( bmap->GetMapName(), 0, 0, dest, 0 );
	_splitpath( bmap->GetMapName(), 0, 0, 0, dest + strlen(dest) );
	return true;
}
//----------------------------------------------------------------------------

// Linked list of vertex normals
class VNormal {
public:
	Point3 norm;
	DWORD smooth;
	VNormal *next;
	BOOL init;
	
	VNormal() {smooth=0;next=NULL;init=FALSE;norm=Point3(0,0,0);}
	VNormal(Point3 &n,DWORD s) {next=NULL;init=TRUE;norm=n;smooth=s;}
	~VNormal() {delete next;}
	void AddNormal(Point3 &n,DWORD s);
	Point3 &GetNormal(DWORD s);
	void Normalize();
};

// Add a normal to the list if the smoothing group bits overlap, 
// otherwise create a new vertex normal in the list
void VNormal::AddNormal(Point3 &n,DWORD s) {
	if (!(s&smooth) && init) {
		if (next) next->AddNormal(n,s);
		else{
			next = new VNormal(n,s);
		}
	} 
	else {
		norm   += n;
		smooth |= s;
		init    = TRUE;
	}
}

// Retrieves a normal if the smoothing groups overlap or there is 
// only one in the list
Point3 &VNormal::GetNormal(DWORD s) {
	if (smooth&s || !next) return norm;
	else return next->GetNormal(s);	
}

// Normalize each normal in the list
void VNormal::Normalize() {
	VNormal *ptr = next, *prev = this;
	while (ptr){
		if (ptr->smooth&smooth){
			norm += ptr->norm;
			prev->next = ptr->next;
			delete ptr;
			ptr = prev->next;
		}else{
			prev = ptr;
			ptr  = ptr->next;
		}
	}
	norm = ::Normalize(norm);
	if (next) next->Normalize();
}

void DisplayVertexNormal(VNormal *vn, int i, int n) {
	DebugPrint("\nVertex %d Normal %d=(%.1f, %.1f, %.1f)", 
		i, n, vn->norm.x, vn->norm.y, vn->norm.z);
	if (vn->next) DisplayVertexNormal(vn->next, i, n+1);
}

// Compute the face and vertex normals
bool ExpMesh::ComputeVertexNormals(Mesh *mesh) {
	Face *face;	
	Point3 *verts;
	Point3 v0, v1, v2;
	Tab<VNormal> vnorms;
	Point3 fnorm;
	
	face = mesh->faces;	
	verts = mesh->verts;
	vnorms.SetCount(mesh->getNumVerts());
	
	// Compute face and vertex surface normals
	for (int i = 0; i < mesh->getNumVerts(); i++) {
		vnorms[i] = VNormal();
	}
	for (i = 0; i < mesh->getNumFaces(); i++, face++) {
		
		// Calculate the surface normal
		v0 = verts[face->v[0]];
		v1 = verts[face->v[1]];
		v2 = verts[face->v[2]];
		fnorm = (v1-v0)^(v2-v1);
		for (int j=0; j<3; j++) {		
			vnorms[face->v[j]].AddNormal(fnorm,face->smGroup);
		}
	}
	for (i=0; i < mesh->getNumVerts(); i++) {
		vnorms[i].Normalize();
	}
	// Display the normals in the debug window of the VC++ IDE
	DebugPrint("\n\nVertex Normals ---");
	for (i = 0; i < vnorms.Count(); i++) {
		DisplayVertexNormal(vnorms.Addr(i), i, 0);
	}
	DebugPrint("\n\n");
	return true;
}

bool ExpMesh::Convert( INode *node ){
	theMeshExpUtility.ip->ProgressUpdate(0,TRUE,"Convertring mesh...");
	float pp, fp=0;
	
	// prepares & checks
	BOOL bDeleteObj;
	bool bResult = true;
	TriObject *obj = ExtractTriObject( node, bDeleteObj );
	
	if( !obj ){
		NConsole.print( "%s -> Can't convert to TriObject", node->GetName() );
		return false; }
	
	if( obj->mesh.getNumFaces() <=0 ){
		NConsole.print( "%s -> There are no faces ?", node->GetName() );
		if (bDeleteObj) SAFE_DELETE(obj);
		return false; }
	
	if( obj->mesh.getNumFaces()>0xFFFF ){
		NConsole.print( "%s -> More than 65535 faces per object isn't suported!", node->GetName() );
		if (bDeleteObj) SAFE_DELETE(obj);
		return false; }
	
	if( obj->mesh.getNumVerts()>0xFFFF ){
		NConsole.print( "%s -> More than 65535 vertices per object isn't suported!", node->GetName() );
		if (bDeleteObj) SAFE_DELETE(obj);
		return false; }
	
	Mtl *pMtlMain = node->GetMtl();
	DWORD cSubMaterials=0;
	
	SPoint facepoints[3];
	vector<DWORD> mtl_idx;
	CTextureLayer* layer= 0;
	
	obj->mesh.buildRenderNormals();
	
	if (pMtlMain){
		// There is at least one material. We're in case 1) or 2)
		cSubMaterials = pMtlMain->NumSubMtls();
		if (cSubMaterials < 1){
			// Count the material itself as a submaterial.
			cSubMaterials = 1;
		}
	}
	
	// reserve space
	m_GFaces.reserve(obj->mesh.getNumFaces());
	m_Vertices.reserve(m_GFaces.size()*3);
	
	//	Matrix3 mMatrix= node->GetNodeTM(0)*Inverse(node->GetParentNode()->GetNodeTM(0));
	obj->mesh.buildBoundingBox();
	Box3 bb = obj->mesh.getBoundingBox();//&mMatrix);
	VMscale.set(bb.pmax.x-bb.pmin.x, bb.pmax.y-bb.pmin.y, bb.pmax.z-bb.pmin.z);
	VMmin.set(bb.pmin.x, bb.pmin.y, bb.pmin.z);
	
	VMeps.set(VMscale.x/MX/2,VMscale.y/MY/2,VMscale.z/MZ/2);
	VMeps.x = (VMeps.x<EPS_L)?VMeps.x:EPS_L;
	VMeps.y = (VMeps.y<EPS_L)?VMeps.y:EPS_L;
	VMeps.z = (VMeps.z<EPS_L)?VMeps.z:EPS_L;
	
	//	ComputeVertexNormals(&obj->mesh);
	
	pp=100.f/float(obj->mesh.getNumFaces());
	for( int i=0; i<obj->mesh.getNumFaces(); i++){
		if (fp+pp>=ceilf(fp)) 
			theMeshExpUtility.ip->ProgressUpdate((int)fp,TRUE,"Converting face...");
		fp+=pp;
		
		TVFace *tvF = 0;
		if( obj->mesh.mapFaces(1) )	tvF = obj->mesh.mapFaces(1) + i;
		
		Face* _F = obj->mesh.faces + i;
		//		int smGroup = _F->getSmGroup();
		
		// position
		for( int k=0; k<3; k++){
			Point3 _Pt = obj->mesh.getVert(_F->v[k]);
			facepoints[k].m_Point.set( _Pt.x, _Pt.y, _Pt.z );
		}
		
		if (pMtlMain){
			int m_id = obj->mesh.getFaceMtlIndex(i);
			if (cSubMaterials == 1){
				m_id = 0;
			}else{
				// SDK recommends mod'ing the material ID by the valid # of materials, 
				// as sometimes a material number that's too high is returned.
				m_id %= cSubMaterials;
			}
			DWORDIt tl_id = find(mtl_idx.begin(),mtl_idx.end(),m_id);
			if (mtl_idx.end() == tl_id){
				layer = AddTextureLayer();
				mtl_idx.push_back(m_id);
			}else{
				layer = m_Layers[tl_id-mtl_idx.begin()];
			}
			VERIFY(layer);
		}
		
		SUV_tri uv_tri;
		for( k=0; k<3; k++){
			// texture coords
			if( tvF ){
				UVVert _uv = obj->mesh.getTVert(tvF->t[k]);
				uv_tri.uv[k].tu = _uv.x;
				uv_tri.uv[k].tv = -_uv.y;
			} else {
				uv_tri.uv[k].tu = 0;
				uv_tri.uv[k].tv = 0;
			}
			
			// normals
			/*			if( smGroup )
			{
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
			}
			else {
			facepoints[k].m_Normal.mknormal(
			facepoints[0].m_Point,
			facepoints[1].m_Point,
			facepoints[2].m_Point );
			}
			*/
			facepoints[k].m_Normal.set(0,0,0);
		}
		CreateFace( facepoints, false );
		if (layer) layer->AddFace( i, &uv_tri );
	}
	
	Fvector N;
	for (FaceIt f_it=m_GFaces.begin(); f_it!=m_GFaces.end(); f_it++){
		N.mknormal(	m_Vertices[f_it->p0].m_Point,
			m_Vertices[f_it->p1].m_Point,
			m_Vertices[f_it->p2].m_Point);
		m_Vertices[f_it->p0].m_Normal.add(N);
		m_Vertices[f_it->p1].m_Normal.add(N);
		m_Vertices[f_it->p2].m_Normal.add(N);
	}
	
	for (PointIt p_it=m_Vertices.begin(); p_it!=m_Vertices.end(); p_it++){
		if (p_it->m_Normal.square_magnitude()>EPS_S){
			p_it->m_Normal.normalize();
		}else{
			p_it->m_Normal.set(0,1,0);
//			NConsole.print( "Can't compute vertex normal! (Possible model has pseudo 2-sided faces.)");
//			bResult=false;
//			break;
		}
	}

	
	if (bResult ){
		NConsole.print( "Model '%s' contains: %d points, %d faces",
			node->GetName(), m_Vertices.size(), m_GFaces.size());
	}
	
	if( bResult ){
		if( pMtlMain ){
			if( pMtlMain->ClassID() == Class_ID(MULTI_CLASS_ID,0) ){
				NConsole.print( "'%s' -> multi material '%s' ...", node->GetName(), pMtlMain->GetName() );
				MultiMtl *mmtl = (MultiMtl*)pMtlMain;
				
				//			for (TLayerIt l=m_Layers.begin(); l!=m_Layers.end(); l++){
				//				int m_id = mtl_idx[l-m_Layers.begin()];
				DWORD cnt = m_Layers.size();
				for (DWORD i=0; i<cnt; i++){
					CTextureLayer* l = m_Layers[i];
					int m_id = mtl_idx[i];
					
					if( mmtl->GetSubMtl(m_id)->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
						StdMat *smtl = (StdMat*)mmtl->GetSubMtl(m_id);
						if (!ExtractMaterial(l, smtl)){
							NConsole.print( "'%s' -> can't extract material...<%s>", node->GetName(), pMtlMain->GetName() );
							bResult = false;
							break;
						}
					} else {
						NConsole.print( "'%s' -> warning: bad submaterial '%s'", 
							node->GetName(), mmtl->GetSubMtl(m_id)->GetName() );
						bResult = false;
						break;
					}
				}
				//			if (m_Layers.size()!=cnt)
				//				reverse(m_Layers.begin()+cnt,m_Layers.end());
			} else if( pMtlMain->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
				NConsole.print( "'%s' -> std material '%s' ...", node->GetName(), pMtlMain->GetName() );
				StdMat *smtl = (StdMat*)pMtlMain;
				DWORD cnt = m_Layers.size();
				//			for (TLayerIt l=m_Layers.begin(); l!=m_Layers.end(); l++)
				for (DWORD i=0; i<cnt; i++){
					CTextureLayer* l = m_Layers[i];
					if (!ExtractMaterial( l, smtl )){
						NConsole.print( "'%s' -> can't extract material...<%s>", node->GetName(), pMtlMain->GetName() );
						bResult = false;
						break;
					}
				}
			} else {
				NConsole.print( "'%s' -> unknown material class, (using defaults) ...", node->GetName() );
				bResult = false;
			}
		} else {
			NConsole.print( "'%s' -> warning: no material", node->GetName() );
			bResult = false;
		}
	}
	
	if (bResult){
		if (!UpdateTies( )){
			NConsole.print( "Model '%s' has non manifold face", node->GetName() );
			bResult=false;
		}
	}
	
	
	if (bResult){
		NConsole.print( "Model '%s' converted: %d points, %d faces",
			node->GetName(), m_Vertices.size(), m_GFaces.size());
	}
	
	if (bDeleteObj) SAFE_DELETE(obj);
	return bResult;
}
//----------------------------------------------------------------------------

void ExpMesh::Duplicate2Sided(CTextureLayer* src, const char* mtl_name){
	NConsole.print( "  Duplicating faces in 2-Sided material '%s'", mtl_name );
	
	CTextureLayer* layer = AddTextureLayer();
	layer->Copy(src,0,false);
	
	SPoint facepoints[3];
	int cnt = src->m_GFaceIDs.size();
	for (DWORD i=0; i<cnt; i++){
		DWORD face_id = src->m_GFaceIDs[i];
		int offs = m_Vertices.size();
		facepoints[0].m_Point.set(m_Vertices[m_GFaces[face_id].p2].m_Point);
		facepoints[1].m_Point.set(m_Vertices[m_GFaces[face_id].p1].m_Point);
		facepoints[2].m_Point.set(m_Vertices[m_GFaces[face_id].p0].m_Point);
		facepoints[0].m_Normal.mul(m_Vertices[m_GFaces[face_id].p2].m_Normal,-1);
		facepoints[1].m_Normal.mul(m_Vertices[m_GFaces[face_id].p1].m_Normal,-1);
		facepoints[2].m_Normal.mul(m_Vertices[m_GFaces[face_id].p0].m_Normal,-1);
		
		CreateFace( facepoints, true );
		
		SUV_tri uv;
		uv.uv[0].tu = src->m_UVMaps[i].uv[2].tu; uv.uv[0].tv = src->m_UVMaps[i].uv[2].tv;
		uv.uv[1].tu = src->m_UVMaps[i].uv[1].tu; uv.uv[1].tv = src->m_UVMaps[i].uv[1].tv;
		uv.uv[2].tu = src->m_UVMaps[i].uv[0].tu; uv.uv[2].tv = src->m_UVMaps[i].uv[0].tv;
		
		layer->AddFace(m_GFaces.size()-1,&uv);
	}
}

void ExpMesh::Append( ExpMesh *source ){
	_ASSERTE( source );
	
	theMeshExpUtility.ip->ProgressUpdate(0,TRUE,"Append mesh...");
	
	vector<SPoint>	apoints;
	vector<SFace>	afaces;
	
	apoints.insert(apoints.end(),source->m_Vertices.begin(),source->m_Vertices.end() );
	afaces.insert(afaces.end(),source->m_GFaces.begin(),source->m_GFaces.end() );
	
	for(int i=0; i<afaces.size(); i++)
		for(int k=0;k<3;k++) afaces[i].p[k] += m_Vertices.size();
		
		CTextureLayer* layer;
		for(TLayerIt l=source->m_Layers.begin(); l!=source->m_Layers.end();l++){
			layer = AddTextureLayer();
			layer->Copy(*l, m_GFaces.size());
		}
		
		m_Vertices.insert(m_Vertices.end(),apoints.begin(),apoints.end());
		m_GFaces.insert(m_GFaces.end(),afaces.begin(),afaces.end());
		
		apoints.clear();
		afaces.clear();
		
		theMeshExpUtility.ip->ProgressUpdate(100,TRUE,"Append mesh...");
}
//----------------------------------------------------------------------------

void ExpMesh::Transform( Fmatrix& parent ){
	for(PointIt v = m_Vertices.begin(); v!=m_Vertices.end(); v++){
		parent.transform((*v).m_Point);
		parent.shorttransform((*v).m_Normal);
	}
}
//----------------------------------------------------------------------------

void ExpMesh::SaveMesh( int handle, bool bMeshChunk ){
	FSChunkDef layerchunk;
	FSChunkDef current;
	FSChunkDef meshchunk;
	
	FS.initchunk            ( &meshchunk, handle );
	FS.initchunk            ( &current, handle );
	FS.initchunk            ( &layerchunk, handle );
	
	if (bMeshChunk) FS.wopenchunk( &meshchunk, STM_CHUNK_MESH );
	
	{
		FS.wopenchunk           ( &current, STM_CHUNK_VERSION );
		FS.writedword           ( handle, STM_CURRENT_MESH_VERSION );
		FS.wclosechunk          ( &current );
		
		FS.wopenchunk           ( &current, STM_CHUNK_HEADER );
		FS.wclosechunk          ( &current );
		
		FS.wopenchunk           ( &current, STM_CHUNK_POINTLIST );
		FS.write                ( handle, m_Vertices.begin(), m_Vertices.size()*sizeof(SPoint));
		FS.wclosechunk          ( &current );
		
		FS.wopenchunk           ( &current, STM_CHUNK_TIELIST );
		FS.write                ( handle, m_FTies.begin(), m_FTies.size()*sizeof(SFaceTie));
		FS.wclosechunk          ( &current );
		
		FS.wopenchunk           ( &current, STM_CHUNK_FACELIST );
		FS.write                ( handle, m_GFaces.begin(), m_GFaces.size()*sizeof(SFace));
		FS.wclosechunk          ( &current );
		
		// Layers
		for(TLayerIt l=m_Layers.begin(); l!=m_Layers.end();l++){
			FS.wopenchunk       ( &current, STM_CHUNK_LAYER );
			// layer name
			FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_NAME );
			FS.writestring      ( handle, (*l)->m_LayerName );
			FS.wclosechunk      ( &layerchunk );
			// texture name
			FS.wopenchunk		( &layerchunk, STM_CHUNK_LAYER_TNAME );
			FS.writestring		( handle, (*l)->m_TexName );
			FS.wclosechunk		( &layerchunk );
			// texture class
			FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_TCLASS );
			FS.write            ( handle, &((*l)->m_CLS), sizeof((*l)->m_CLS) );
			FS.wclosechunk      ( &layerchunk );
			// script id
			FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_SCRIPT );
			FS.writestring		( handle, (*l)->m_Script );
			FS.wclosechunk      ( &layerchunk );
			// material
			FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_MATERIAL );
			FS.write            ( handle, &((*l)->m_Mat), sizeof((*l)->m_Mat) );
			FS.wclosechunk      ( &layerchunk );
			// shader op
			FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_SHADER_OP );
			FS.write            ( handle, &((*l)->m_SOP), sizeof((*l)->m_SOP) );
			FS.wclosechunk      ( &layerchunk );
			// layer uv-maps
			FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_UVMAPS );
			FS.write            ( handle, (*l)->m_UVMaps.begin(), (*l)->m_UVMaps.size()*sizeof(SUV_tri));
			FS.wclosechunk      ( &layerchunk );
			// layer geom face refs
			FS.wopenchunk       ( &layerchunk, STM_CHUNK_LAYER_FACE_REFS );
			FS.write            ( handle, (*l)->m_GFaceIDs.begin(), (*l)->m_GFaceIDs.size()*sizeof(DWORD));
			FS.wclosechunk      ( &layerchunk );
			
			FS.wclosechunk( &current );
		}
	}
	if (bMeshChunk) FS.wclosechunk          ( &meshchunk );
}
//----------------------------------------------------------------------------

void ExpMesh::Save( char *filename ){
	_ASSERTE( filename );
	int handle = FS.create	( filename );
	SaveMesh				( handle, true );
	FS.close				( handle );
}
//----------------------------------------------------------------------------









//----------------------------------------------------
/*void ExpMesh::Convert(ExpMesh* mesh)
{
//    vector<SScript>    scripts;
// vertices
vector<MPoint>::iterator Vsrc;
for(Vsrc=mesh->m_Points.begin(); Vsrc!=mesh->m_Points.end();Vsrc++){
m_Vertices.push_back(SPoint());
m_Vertices.back().m_Point.set(Vsrc->m_Point);
m_Vertices.back().m_Normal.set(Vsrc->m_Normal);
}
// faces
vector<MFace>::iterator Fsrc;
for(Fsrc=mesh->m_Faces.begin(); Fsrc!=mesh->m_Faces.end();Fsrc++){
m_GFaces.push_back(SFace());
CopyMemory(m_GFaces.back().p,Fsrc->p,sizeof(MFace));
}
// materials
vector<MMaterial>::iterator m;
for(m=mesh->m_Materials.begin(); m!=mesh->m_Materials.end();m++){
CTextureLayer* l = AddTextureLayer(m->m_TexName);
l->m_Mat.diffuse.set(m->m_Diffuse.r, m->m_Diffuse.g, m->m_Diffuse.b, m->m_Diffuse.a);
l->m_Mat.ambient.set(m->m_Ambient.r, m->m_Ambient.g, m->m_Ambient.b, m->m_Ambient.a);
l->m_Mat.specular.set(m->m_Specular.r, m->m_Specular.g, m->m_Specular.b, m->m_Specular.a);
l->m_Mat.emissive.set(m->m_Emission.r, m->m_Emission.g, m->m_Emission.b, m->m_Emission.a);
l->m_Mat.power = m->m_Power;
// tfaces
int st = m->m_FaceStart;
for(DWORD ti=0; ti<m->m_FaceCount;ti++){
SUV_tri uv;
uv.uv[0].tu=mesh->m_Points[mesh->m_Faces[st+ti].p0].m_TexCoord.x;
uv.uv[0].tv=mesh->m_Points[mesh->m_Faces[st+ti].p0].m_TexCoord.y;
uv.uv[1].tu=mesh->m_Points[mesh->m_Faces[st+ti].p1].m_TexCoord.x;
uv.uv[1].tv=mesh->m_Points[mesh->m_Faces[st+ti].p1].m_TexCoord.y;
uv.uv[2].tu=mesh->m_Points[mesh->m_Faces[st+ti].p2].m_TexCoord.x;
uv.uv[2].tv=mesh->m_Points[mesh->m_Faces[st+ti].p2].m_TexCoord.y;
l->AddFace(st+ti, &uv);
}
}
}
*/
