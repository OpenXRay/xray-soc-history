//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMesh.h"
#include "MeshExpUtility.h"
//----------------------------------------------------
void CEditMesh::Transform(Fmatrix& parent){
	// transform position
	for(FvectorIt pt=m_Points.begin(); pt!=m_Points.end(); pt++)
		parent.transform_tiny(*pt);
    // update normals
	/*    for (FaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
	parent.transform_dir(f_it->norm);
	parent.transform_dir(f_it->pv[0].norm);
	parent.transform_dir(f_it->pv[1].norm);
	parent.transform_dir(f_it->pv[2].norm);
    }
	*/
}
//----------------------------------------------------


bool CEditMesh::UpdateAdjacency(){
	NConsole.print("..Update adjacency");
	NConsole.ProgressStart(m_Faces.size());
	if (m_Faces.empty()) return false;
    m_Adjs.clear();
    m_Adjs.resize(m_Points.size());
	for (FaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
		NConsole.ProgressUpdate(f_it-m_Faces.begin());
		for (int k=0; k<3; k++) 
			m_Adjs[f_it->pv[k].pindex].push_back(f_it-m_Faces.begin());
	}
	NConsole.ProgressEnd();
	return true;
}


#define MX 30
#define MY 30
#define MZ 30
Fvector		VMmin, VMscale;
DWORDList	VM[MX+1][MY+1][MZ+1];
Fvector		VMeps;

struct st_Vert{
	Fvector P;
	Fvector N;
};

DEFINE_VECTOR(st_Vert,VertList,VertIt);
VertList m_NewVerts;

bool CEditMesh::OptimizeFace(int face_id, bool bTestNormal){
	st_Face& face = m_Faces[face_id];

	st_Vert verts[3];
	int mface[3];
	int k;
	
	for (k=0; k<3; k++){
		verts[k].P.set(m_Points[face.pv[k].pindex]);
		if (bTestNormal) verts[k].N.set(m_PNormals[face_id*3+k]);
		mface[k] = -1;
    }
	
	// get similar vert idx list
	for (k=0; k<3; k++){
		DWORDList* vl; 
		int ix,iy,iz;
		ix = floorf(float(verts[k].P.x-VMmin.x)/VMscale.x*MX);
		iy = floorf(float(verts[k].P.y-VMmin.y)/VMscale.y*MY);
		iz = floorf(float(verts[k].P.z-VMmin.z)/VMscale.z*MZ);
		vl = &(VM[ix][iy][iz]);
		for(DWORDIt it=vl->begin();it!=vl->end(); it++){ 
			VertIt v = m_NewVerts.begin()+(*it);
			if (bTestNormal){
				if( v->P.similar(verts[k].P,EPS)&&v->N.similar(verts[k].N,EPS))
					mface[k] = *it;
			}else{
				if( v->P.similar(verts[k].P,EPS) )
					mface[k] = *it;
			}
		}

	}
	for(k=0; k<3; k++ ){
		if( mface[k] == -1 ){
			mface[k] = m_NewVerts.size();
			m_NewVerts.push_back( verts[k] );
			int ix,iy,iz;
			ix = floorf(float(verts[k].P.x-VMmin.x)/VMscale.x*MX);
			iy = floorf(float(verts[k].P.y-VMmin.y)/VMscale.y*MY);
			iz = floorf(float(verts[k].P.z-VMmin.z)/VMscale.z*MZ);
			VM[ix][iy][iz].push_back(mface[k]);
			int ixE,iyE,izE;
			ixE = floorf(float(verts[k].P.x+VMeps.x-VMmin.x)/VMscale.x*MX);
			iyE = floorf(float(verts[k].P.y+VMeps.y-VMmin.y)/VMscale.y*MY);
			izE = floorf(float(verts[k].P.z+VMeps.z-VMmin.z)/VMscale.z*MZ);
			if (ixE!=ix)				
				VM[ixE][iy][iz].push_back(mface[k]);
			if (iyE!=iy)				
				VM[ix][iyE][iz].push_back(mface[k]);
			if (izE!=iz)				
				VM[ix][iy][izE].push_back(mface[k]);
			if ((ixE!=ix)&&(iyE!=iy))	
				VM[ixE][iyE][iz].push_back(mface[k]);
			if ((ixE!=ix)&&(izE!=iz))	
				VM[ixE][iy][izE].push_back(mface[k]);
			if ((iyE!=iy)&&(izE!=iz))	
				VM[ix][iyE][izE].push_back(mface[k]);
			if ((ixE!=ix)&&(iyE!=iy)&&(izE!=iz))
				VM[ixE][iyE][izE].push_back(mface[k]);
		}
	}
	
	if ((mface[0]==mface[1])||(mface[1]==mface[2])||(mface[0]==mface[2])){
		NConsole.print( "* Optimize: Mesh has degenerate face!");
        return false;
	}else{
		face.pv[0].pindex = mface[0];
		face.pv[1].pindex = mface[1];
		face.pv[2].pindex = mface[2];
        return true;
	}
}

void CEditMesh::Optimize(BOOL NOOPT)
{
	if (!NOOPT) {
		NConsole.print(".Optimizing mesh...");
		// clear static data
		for (int x=0; x<MX+1; x++)
			for (int y=0; y<MY+1; y++)
				for (int z=0; z<MZ+1; z++)
					VM[x][y][z].clear();

		VMscale.set(m_Box.max.x-m_Box.min.x, m_Box.max.y-m_Box.min.y, m_Box.max.z-m_Box.min.z);
		VMmin.set(m_Box.min.x, m_Box.min.y, m_Box.min.z);
		
		VMeps.set(VMscale.x/MX/2,VMscale.y/MY/2,VMscale.z/MZ/2);
		VMeps.x = (VMeps.x<EPS_L)?VMeps.x:EPS_L;
		VMeps.y = (VMeps.y<EPS_L)?VMeps.y:EPS_L;
		VMeps.z = (VMeps.z<EPS_L)?VMeps.z:EPS_L;
		
		m_NewVerts.clear();
		m_NewVerts.reserve(m_Points.size()*3);
		
		INTList mark_for_del;
		mark_for_del.clear();
		
		NConsole.ProgressStart(m_Faces.size());
		
		NConsole.print("..Check faces");
		
		for (int k=0; k<m_Faces.size(); k++){
			NConsole.ProgressUpdate(k);
			if (!OptimizeFace(k,!U.m_ObjectSuppressSmoothGroup))
				mark_for_del.push_back(k);
		}
		NConsole.ProgressEnd();
		
		NConsole.print("..Update points");
		m_Points.clear();
		m_PNormals.clear();

		m_Points.resize(m_NewVerts.size());
		FvectorIt p_it = m_Points.begin();
		for (VertIt v_it=m_NewVerts.begin(); v_it!=m_NewVerts.end(); v_it++,p_it++) p_it->set(v_it->P);
		if (mark_for_del.size()>0){
			NConsole.print("..Delete degenerate faces");
			std::sort	(mark_for_del.begin(),mark_for_del.end());
			std::reverse(mark_for_del.begin(),mark_for_del.end());
			// delete degenerate faces
			for (INTIt i_it=mark_for_del.begin(); i_it!=mark_for_del.end(); i_it++)
				m_Faces.erase(m_Faces.begin()+(*i_it));
			// delete degenerate faces refs
			for (INTIt m_d=mark_for_del.begin(); m_d!=mark_for_del.end(); m_d++){
				for (SurfFacesPairIt plp_it=m_SurfFaces.begin(); plp_it!=m_SurfFaces.end(); plp_it++){
					INTList& 	pol_lst = plp_it->second;
					for (int k=0; k<pol_lst.size(); k++){
						int& f = pol_lst[k];
						if (f>*m_d){ f--;
						}else if (f==*m_d){
							pol_lst.erase(pol_lst.begin()+k);
							k--;
						}
					}
				}
			}
/*			for (SurfFacesPairIt plp_it=m_SurfFaces.begin(); plp_it!=m_SurfFaces.end(); plp_it++){
				INTList& 	pol_lst = plp_it->second;
				for (int k=0; k<pol_lst.size(); k++){
					NConsole.print("#%d %d",k,pol_lst[k]);
				}
			}
*/		}
	}
	
    UpdateAdjacency();
}
