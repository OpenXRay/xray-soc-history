//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMesh.h"
#include "EditObject.h"
#include "FileSystem.h"

#define EMESH_CURRENT_VERSION	      	0x0011
//----------------------------------------------------
#define EMESH_CHUNK_VERSION	        	0x1000
#define EMESH_CHUNK_MESHNAME        	0x1001
#define EMESH_CHUNK_VISIBLE	        	0x1002
#define EMESH_CHUNK_LOCKED	        	0x1003
#define EMESH_CHUNK_BBOX	        	0x1004
#define EMESH_CHUNK_VERTS	        	0x1005
#define EMESH_CHUNK_FACES	        	0x1006
#define EMESH_CHUNK_VMAPS	        	0x1007
#define EMESH_CHUNK_VMREFS	        	0x1008
#define EMESH_CHUNK_SFACE				0x1009
#define EMESH_CHUNK_BOP					0x1010

void CEditMesh::SaveMesh(CFS_Base& F){
//	DWORD TSZ = 0, SZ = 0;

	F.open_chunk	(EMESH_CHUNK_VERSION);
	F.Wword       	(EMESH_CURRENT_VERSION); 								//SZ=2;
	F.close_chunk  	();
//    Log->Msg(mtInformation,"HDR: %d",SZ); TSZ+=SZ;
    
	F.open_chunk	(EMESH_CHUNK_MESHNAME);
    F.WstringZ		(m_Name);				    							//SZ=strlen(m_Name);
	F.close_chunk   ();
//    Log->Msg(mtInformation,"Name: %d",SZ); TSZ+=SZ;

	F.write_chunk	(EMESH_CHUNK_BBOX,&m_Box, sizeof(m_Box));
	F.write_chunk	(EMESH_CHUNK_VISIBLE,&m_Visible,1);						//SZ=1;
	F.write_chunk	(EMESH_CHUNK_LOCKED,&m_Locked,1);						//SZ+=1;
//	F.write_chunk	(EMESH_CHUNK_BOP,&m_Ops, sizeof(m_Ops));
//    Log->Msg(mtInformation,"HDR2: %d",SZ); TSZ+=SZ;

	F.open_chunk	(EMESH_CHUNK_VERTS);
	F.Wdword		(m_Points.size());										//SZ=m_Points.size()*4;
    F.write			(m_Points.begin(), m_Points.size()*sizeof(Fvector));	//SZ+=m_Points.size()*sizeof(Fvector);
    for (AdjIt a_it=m_Adjs.begin(); a_it!=m_Adjs.end(); a_it++){
    	int sz 		= a_it->size(); VERIFY(sz<=255);						
		F.Wbyte		(sz);                                                   //SZ+=1;
        F.write		(a_it->begin(), sizeof(int)*sz);						//SZ+=sz*4;
    }
	F.close_chunk     ();
//    Log->Msg(mtInformation,"Verts: %d",SZ); TSZ+=SZ;

	F.open_chunk	(EMESH_CHUNK_FACES);
	F.Wdword		(m_Faces.size()); 		/* polygon count */				//SZ=4;
    F.write			(m_Faces.begin(), m_Faces.size()*sizeof(st_Face));		//SZ+=m_Faces.size()*sizeof(st_Face);
	F.close_chunk  	();
//    Log->Msg(mtInformation,"Faces: %d",SZ); TSZ+=SZ;
    
	F.open_chunk	(EMESH_CHUNK_VMREFS);
	F.Wdword		(m_VMRefs.size());                                      //SZ=4;
    for (VMRefsIt r_it=m_VMRefs.begin(); r_it!=m_VMRefs.end(); r_it++){
    	int sz 		= r_it->size(); VERIFY(sz<=255);
		F.Wbyte		(sz);													//SZ+=1;
        F.write		(r_it->begin(), sizeof(st_VMapPt)*sz);					//SZ+=sz*sizeof(st_VMapPt);
    }
	F.close_chunk	();
//    Log->Msg(mtInformation,"VMRefs: %d",SZ); TSZ+=SZ;
    
	F.open_chunk	(EMESH_CHUNK_SFACE);
	F.Wword			(m_SurfFaces.size()); 	/* surface polygon count*/		//SZ=2;
	for (SurfFacesPairIt plp_it=m_SurfFaces.begin(); plp_it!=m_SurfFaces.end(); plp_it++){
    	F.WstringZ	(plp_it->first->name); 	/* surface name*/				//SZ+=strlen(plp_it->first->name);
    	INTList& 	pol_lst = plp_it->second;
        F.Wdword	(pol_lst.size());		/* surface-polygon indices*/	//SZ+=4;
        F.write		(pol_lst.begin(), sizeof(int)*pol_lst.size());			//SZ+=4*pol_lst.size();
    }
	F.close_chunk     ();
//    Log->Msg(mtInformation,"SFace: %d",SZ); TSZ+=SZ;

	F.open_chunk	(EMESH_CHUNK_VMAPS);
	F.Wdword		(m_VMaps.size());										//SZ=4;
	for (VMapIt vm_it=m_VMaps.begin(); vm_it!=m_VMaps.end(); vm_it++){
	    F.WstringZ	(vm_it->name);											//SZ+=strlen(vm_it->name);
        F.Wdword	(vm_it->vm.size());										//SZ+=4;
        F.write		(vm_it->vm.begin(), sizeof(Fvector2)*vm_it->vm.size() );//SZ+=vm_it->vm.size()*sizeof(Fvector2);
    }
	F.close_chunk	();
//    Log->Msg(mtInformation,"VMaps: %d",SZ); TSZ+=SZ;
//    Log->Msg(mtInformation,"TSZ: %d",TSZ);
}
