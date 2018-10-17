// xrSKIN_BUILD.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "fmesh.h"
#include "progmesh.h"
#include "xrSKIN_BUILD.h"
#include "Exporter.h"
#include "ftimer.h"
#undef DEG_TO_RAD
#undef RAD_TO_DEG
#include "MgcCont3DMinBox.h"
#include "MeshExpUtility.rh"
#include "MeshExpUtility.h"
#include "BlenderListLoader.h"

#include "EditObject.h"
#include "EditMesh.h"
#include "Bone.h"
#include "Motion.h"
#include "Envelope.h"

#pragma comment(lib, "x:\\xrProgressive.lib")

// shaders 
//----------------------------------------------------------------------------------------------
AStringVec Shaders;

HWND m_hSHWindow=0;
static string m_SelShName="models\\model";
static LPCSTR m_SelTexName;

BOOL CALLBACK ShaderDialogProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp){
	switch( msg ){

	case WM_INITDIALOG:{
		SetWindowPos(hw,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		m_hSHWindow = hw;

		for (AStringIt it=Shaders.begin(); it!=Shaders.end(); it++)
			SendDlgItemMessage(hw, IDC_SHADERLIST, LB_ADDSTRING, 0, (LPARAM)it->c_str());

		int k = SendDlgItemMessage(hw, IDC_SHADERLIST, LB_FINDSTRING, -1, (LPARAM)m_SelShName.c_str());
		SendDlgItemMessage(hw, IDC_SHADERLIST, LB_SETCURSEL, k, 0);

		SendDlgItemMessage(hw, IDC_TEXTURE, LB_SETCURSEL, k, 0);
		HWND wnd = GetDlgItem(hw,IDC_TEXTURE);
		SetWindowText(wnd,m_SelTexName);
		}break;
	case WM_COMMAND:
		switch( LOWORD(wp) ){
		case ID_SHOK:{
			char buf[255];
			int k = SendDlgItemMessage(hw, IDC_SHADERLIST, LB_GETCURSEL, 0, 0);
			SendDlgItemMessage(hw, IDC_SHADERLIST, LB_GETTEXT, k, (LPARAM)buf);
			if (k>-1) m_SelShName = buf;
			SendMessage(hw,WM_CLOSE,0,0);
			}break;
		case ID_SHCANCEL:
			SendMessage(hw,WM_CLOSE,0,0);
			break;
		}
		break;
	case WM_CLOSE:{
		int res=0;
		EndDialog(hw,res);
		m_hSHWindow=0;
		}break;
	default:
		return FALSE;
	}
	return TRUE;
}

//----------------------------------------------------------------------------------------------

void MakeMotion(CExporter* E)
{
	int iFPS			= GetFrameRate();
	int iTPF			= GetTicksPerFrame();
	int iStartTick		= U.ip->GetAnimRange().Start();
	int iEndTick		= U.ip->GetAnimRange().End();

	// build motion
	CSMotion* MOT		= new CSMotion();
	MOT->SetParam		(iStartTick/iTPF,iEndTick/iTPF,(float)iFPS);
	MOT->SetName		("xyz");

	BoneMotionVec& BMVec= MOT->BoneMotions();
	BMVec.reserve		(E->m_Bones.size());

	Fmatrix tmBone;
	for(int boneId = 0; boneId < int(E->m_Bones.size()); boneId++)
	{
		CBoneDef* bone	= E->m_Bones[boneId];

		// only export keyframes for the selected bone candidates
		{
			BMVec.push_back(st_BoneMotion());
			st_BoneMotion& BM = BMVec.back();

			BM.envs[ctPositionX] = new CEnvelope();
			BM.envs[ctPositionY] = new CEnvelope();
			BM.envs[ctPositionZ] = new CEnvelope();
			BM.envs[ctRotationH] = new CEnvelope();
			BM.envs[ctRotationP] = new CEnvelope();
			BM.envs[ctRotationB] = new CEnvelope();

			BM.envs[ctPositionX]->behavior[0]=1; BM.envs[ctPositionX]->behavior[1]=1;
			BM.envs[ctPositionY]->behavior[0]=1; BM.envs[ctPositionY]->behavior[1]=1;
			BM.envs[ctPositionZ]->behavior[0]=1; BM.envs[ctPositionZ]->behavior[1]=1;
			BM.envs[ctRotationH]->behavior[0]=1; BM.envs[ctRotationH]->behavior[1]=1;
			BM.envs[ctRotationP]->behavior[0]=1; BM.envs[ctRotationP]->behavior[1]=1;
			BM.envs[ctRotationB]->behavior[0]=1; BM.envs[ctRotationB]->behavior[1]=1;
			// get the bone transformation matrix
			//tmBone = skeletonCandidate.GetBoneTM(boneCandidateId, (startFrame + frameDisplacement % numFrames)/(float)fps);
			// if it's the root bone, set the default bone pose to identity

			int tick;
			st_Key *X,*Y,*Z,*H,*P,*B;
			for(tick = iStartTick; tick < iEndTick; tick+=iTPF)
			{
				X = new st_Key();	Y = new st_Key();	Z = new st_Key();
				H = new st_Key();	P = new st_Key();	B = new st_Key();
				BM.envs[ctPositionX]->keys.push_back(X);	BM.envs[ctPositionY]->keys.push_back(Y);	BM.envs[ctPositionZ]->keys.push_back(Z);
				BM.envs[ctRotationH]->keys.push_back(H);	BM.envs[ctRotationP]->keys.push_back(P);	BM.envs[ctRotationB]->keys.push_back(B);
				// get the bone transformation matrix
				bone->CalculateLocalMatrix(tick,tmBone);

				float displacedTime = (float)tick/(float)(iTPF*iFPS);

				float h,p,b, x,y,z;
				tmBone.getHPB(h,p,b);
				x=tmBone.c.x; y=tmBone.c.y; z=tmBone.c.z;
				X->time = displacedTime;	Y->time = displacedTime;	Z->time = displacedTime;
				H->time = displacedTime;	P->time = displacedTime;	B->time = displacedTime;
				X->shape = 3;	Y->shape = 3;	Z->shape = 3;
				H->shape = 3;	P->shape = 3;	B->shape = 3;
				X->value = x;	Y->value = y;	Z->value = z;
				H->value =-h;	P->value =-p;	B->value =-b;
				//X->value = 0;	Y->value = sinf(displacedTime);	Z->value = 0;
				//H->value = 0;	P->value = 0;	B->value = 0;
			}
		} 
	}
	MOT->SaveMotion("x:\\import\\t.skl");
}
