// PrimitivesR.cpp: implementation of the CPrimitivesR class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

// Device dependance
const DWORD Lines_BatchSize = 64;
void CDraw::OnDeviceCreate()
{
	vsTL = Device.Streams.Create(FVF::F_TL,Lines_BatchSize*4);
}
void CDraw::OnDeviceDestroy()
{
}

// Lines
void CDraw::Lines_Begin	(int count)
{
	pCurrent = pStart = (FVF::TL*)vsTL->Lock(count*4,dwLN_Offset);
}
void CDraw::Lines_Draw	(Fvector& P1, Fvector& P2, float width, DWORD C)
{
	FVF::TL			s1,s2;
	s1.transform	(P1,Device.mFullTransform); float l1 = width/sqrtf(s1.p.w);
	s2.transform	(P2,Device.mFullTransform);	float l2 = width/sqrtf(s2.p.w);
	Fvector2		dir,R;
	dir.set			(s2.p.x-s1.p.x,s2.p.y-s1.p.y);
	dir.norm		();
	R.cross			(dir);

	pCurrent->set	(Device._x2real(s1.p.x+R.x*l1), Device._y2real(s1.p.y+R.y*l1), s1.p.z, s1.p.w, C, 0, 1); pCurrent++;
	pCurrent->set	(Device._x2real(s2.p.x+R.x*l2), Device._y2real(s2.p.y+R.y*l2), s2.p.z, s1.p.w, C, 0, 0); pCurrent++;
	pCurrent->set	(Device._x2real(s1.p.x-R.x*l1), Device._y2real(s1.p.y-R.y*l1), s1.p.z, s1.p.w, C, 1, 1); pCurrent++;
	pCurrent->set	(Device._x2real(s2.p.x-R.x*l2), Device._y2real(s2.p.y-R.y*l2), s2.p.z, s1.p.w, C, 1, 0); pCurrent++;
}

void CDraw::Lines_End	()
{
	DWORD			C = pCurrent-pStart;
	vsTL->Unlock	(C);
	if (C)			Draw(vsTL,C,C/2,dwLN_Offset,Device.Streams_QuadIB);
}

void CDraw::Draw(CPrimitive& P, DWORD dwNumVerts, DWORD dwNumPrimitives)
{
	setVertices	(P.vShader,P.vSize,P.pVertices);
	setIndices	(P.dwBaseVertex,P.pIndices);
	DWORD dwRequired	= Device.Shader.dwPassesRequired;
	for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++)
	{
		Device.Shader.SetupPass(dwPass);
		Render		(D3DPT_TRIANGLELIST,0,dwNumVerts,0,dwNumPrimitives);
	}
	UPDATEC(dwNumVerts,dwNumPrimitives,dwRequired);
}
void CDraw::Draw(CVertexStream* S,	DWORD dwNumVerts, DWORD dwNumPrimitives, DWORD dwBase, IDirect3DIndexBuffer8* IB)
{
	setVertices	(S->mFVF,S->mStride,S->pVB);
	CHK_DX(HW.pDevice->SetIndices(pCurIB=IB,dwBase));
	DWORD dwRequired	= Device.Shader.dwPassesRequired;
	for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++)
	{
		Device.Shader.SetupPass(dwPass);
		Render		(D3DPT_TRIANGLELIST,0,dwNumVerts,0,dwNumPrimitives);
	}
	UPDATEC(dwNumVerts,dwNumPrimitives,dwRequired);
}
void CDraw::Draw(CVertexStream* S,	DWORD dwNumPrimitives, DWORD dwBase)
{
	setVertices	(S->mFVF,S->mStride,S->pVB);
	setIndices	(0,0);
	DWORD dwRequired	= Device.Shader.dwPassesRequired;
	for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++)
	{
		Device.Shader.SetupPass(dwPass);
		Render		(D3DPT_TRIANGLELIST,dwBase,dwNumPrimitives);
	}
	UPDATEC(dwNumPrimitives*3,dwNumPrimitives,dwRequired);
}
void CDraw::DrawNI_SP(CPrimitive& P, DWORD dwStartVert, DWORD dwNumPrimitives)
{
	setVertices	(P.vShader,P.vSize,P.pVertices);
	setIndices	(0,0);
	Device.Shader.SetupPass(0);
	Render		(D3DPT_TRIANGLELIST,dwStartVert,dwNumPrimitives);
	UPDATEC(dwNumPrimitives,dwNumPrimitives,1);
}
void CDraw::DrawSubset(CPrimitive& P, DWORD dwStartVertex, DWORD dwNumVerts, DWORD dwStartIndex, DWORD dwNumPrimitives)
{
	setVertices	(P.vShader,P.vSize,P.pVertices);
	setIndices	(P.dwBaseVertex,P.pIndices);
	DWORD dwRequired	= Device.Shader.dwPassesRequired;
	for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++)
	{
		Device.Shader.SetupPass(dwPass);
		Render		(D3DPT_TRIANGLELIST,dwStartVertex,dwNumVerts,dwStartIndex,dwNumPrimitives);
	}
	UPDATEC(dwNumVerts,dwNumPrimitives,dwRequired);
}
void CDraw::Reset()
{
	vCurShader = 0;
	CHK_DX(HW.pDevice->SetStreamSource(0,pCurVB=0,0));
	CHK_DX(HW.pDevice->SetIndices(pCurIB=0,0));
}
void CDraw::dbg_Draw(D3DPRIMITIVETYPE T, FVF::L* pVerts, int vcnt, WORD* pIdx, int pcnt)
{
	Reset					();
	Device.Shader.SetNULL	();
	Device.Shader.SetupPass	(0);

	CHK_DX(HW.pDevice->SetVertexShader(vCurShader=FVF::F_L));
	CHK_DX(HW.pDevice->DrawIndexedPrimitiveUP(T, 0, vcnt, pcnt, 
		pIdx, D3DFMT_INDEX16,
		pVerts, sizeof(FVF::L)
		));
}
void CDraw::dbg_Draw(D3DPRIMITIVETYPE T, FVF::L* pVerts, int pcnt)
{
	Reset					();
	Device.Shader.SetNULL	();
	Device.Shader.SetupPass	(0);

	CHK_DX(HW.pDevice->SetVertexShader(vCurShader=FVF::F_L));
	CHK_DX(HW.pDevice->DrawPrimitiveUP(T, pcnt, pVerts, sizeof(FVF::L)	));
}

#define RGBA_GETALPHA(rgb)      ((rgb) >> 24)
void CDraw::dbg_DrawOBB	(Fmatrix& T, Fvector& half_dim, DWORD C)
{
	Fmatrix mL2W_Transform,mScaleTransform;

	mScaleTransform.scale(half_dim);
	mL2W_Transform.mul_43(T,mScaleTransform);

	static FVF::L  aabb[8];
	aabb[0].set( -1, -1, -1, C ); // 0
	aabb[1].set( -1, +1, -1, C ); // 1
	aabb[2].set( +1, +1, -1, C ); // 2
	aabb[3].set( +1, -1, -1, C ); // 3
	aabb[4].set( -1, -1, +1, C ); // 4
	aabb[5].set( -1, +1, +1, C ); // 5
	aabb[6].set( +1, +1, +1, C ); // 6
	aabb[7].set( +1, -1, +1, C ); // 7

	static WORD		aabb_id[12*2] = {
		0,1,  1,2,  2,3,  3,0,  4,5,  5,6,  6,7,  7,4,  1,5,  2,6,  3,7,  0,4
	};
	HW.pDevice->SetTransform(D3DTS_WORLD, mL2W_Transform.d3d());
	dbg_Draw(D3DPT_LINELIST,aabb,8,aabb_id,12);
}
void CDraw::dbg_DrawTRI	(Fmatrix& T, Fvector& p1, Fvector& p2, Fvector& p3, DWORD C)
{
	static FVF::L	tri[3];
	tri[0].p = p1; tri[0].color = C;
	tri[1].p = p2; tri[1].color = C;
	tri[2].p = p3; tri[2].color = C;

	HW.pDevice->SetTransform(D3DTS_WORLD, T.d3d());
	dbg_Draw(D3DPT_TRIANGLESTRIP,tri,1);
}
void CDraw::dbg_DrawLINE(Fmatrix& T, Fvector& p1, Fvector& p2, DWORD C)
{
	static FVF::L	line[2];
	line[0].p = p1; line[0].color = C;
	line[1].p = p2; line[1].color = C;

	HW.pDevice->SetTransform(D3DTS_WORLD, T.d3d());
	dbg_Draw(D3DPT_LINELIST,line,1);
}
void CDraw::dbg_DrawEllipse(Fmatrix& T, DWORD C)
{
	static float gVertices[] =
	{
		0.0000f,0.0000f,1.0000f,  0.0000f,0.3827f,0.9239f,  -0.1464f,0.3536f,0.9239f,
		-0.2706f,0.2706f,0.9239f,  -0.3536f,0.1464f,0.9239f,  -0.3827f,0.0000f,0.9239f,
		-0.3536f,-0.1464f,0.9239f,  -0.2706f,-0.2706f,0.9239f,  -0.1464f,-0.3536f,0.9239f,
		0.0000f,-0.3827f,0.9239f,  0.1464f,-0.3536f,0.9239f,  0.2706f,-0.2706f,0.9239f,
		0.3536f,-0.1464f,0.9239f,  0.3827f,0.0000f,0.9239f,  0.3536f,0.1464f,0.9239f,
		0.2706f,0.2706f,0.9239f,  0.1464f,0.3536f,0.9239f,  0.0000f,0.7071f,0.7071f,
		-0.2706f,0.6533f,0.7071f,  -0.5000f,0.5000f,0.7071f,  -0.6533f,0.2706f,0.7071f,
		-0.7071f,0.0000f,0.7071f,  -0.6533f,-0.2706f,0.7071f,  -0.5000f,-0.5000f,0.7071f,
		-0.2706f,-0.6533f,0.7071f,  0.0000f,-0.7071f,0.7071f,  0.2706f,-0.6533f,0.7071f,
		0.5000f,-0.5000f,0.7071f,  0.6533f,-0.2706f,0.7071f,  0.7071f,0.0000f,0.7071f,
		0.6533f,0.2706f,0.7071f,  0.5000f,0.5000f,0.7071f,  0.2706f,0.6533f,0.7071f,
		0.0000f,0.9239f,0.3827f,  -0.3536f,0.8536f,0.3827f,  -0.6533f,0.6533f,0.3827f,
		-0.8536f,0.3536f,0.3827f,  -0.9239f,0.0000f,0.3827f,  -0.8536f,-0.3536f,0.3827f,
		-0.6533f,-0.6533f,0.3827f,  -0.3536f,-0.8536f,0.3827f,  0.0000f,-0.9239f,0.3827f,
		0.3536f,-0.8536f,0.3827f,  0.6533f,-0.6533f,0.3827f,  0.8536f,-0.3536f,0.3827f,
		0.9239f,0.0000f,0.3827f,  0.8536f,0.3536f,0.3827f,  0.6533f,0.6533f,0.3827f,
		0.3536f,0.8536f,0.3827f,  0.0000f,1.0000f,0.0000f,  -0.3827f,0.9239f,0.0000f,
		-0.7071f,0.7071f,0.0000f,  -0.9239f,0.3827f,0.0000f,  -1.0000f,0.0000f,0.0000f,
		-0.9239f,-0.3827f,0.0000f,  -0.7071f,-0.7071f,0.0000f,  -0.3827f,-0.9239f,0.0000f,
		0.0000f,-1.0000f,0.0000f,  0.3827f,-0.9239f,0.0000f,  0.7071f,-0.7071f,0.0000f,
		0.9239f,-0.3827f,0.0000f,  1.0000f,0.0000f,0.0000f,  0.9239f,0.3827f,0.0000f,
		0.7071f,0.7071f,0.0000f,  0.3827f,0.9239f,0.0000f,  0.0000f,0.9239f,-0.3827f,
		-0.3536f,0.8536f,-0.3827f,  -0.6533f,0.6533f,-0.3827f,  -0.8536f,0.3536f,-0.3827f,
		-0.9239f,0.0000f,-0.3827f,  -0.8536f,-0.3536f,-0.3827f,  -0.6533f,-0.6533f,-0.3827f,
		-0.3536f,-0.8536f,-0.3827f,  0.0000f,-0.9239f,-0.3827f,  0.3536f,-0.8536f,-0.3827f,
		0.6533f,-0.6533f,-0.3827f,  0.8536f,-0.3536f,-0.3827f,  0.9239f,0.0000f,-0.3827f,
		0.8536f,0.3536f,-0.3827f,  0.6533f,0.6533f,-0.3827f,  0.3536f,0.8536f,-0.3827f,
		0.0000f,0.7071f,-0.7071f,  -0.2706f,0.6533f,-0.7071f,  -0.5000f,0.5000f,-0.7071f,
		-0.6533f,0.2706f,-0.7071f,  -0.7071f,0.0000f,-0.7071f,  -0.6533f,-0.2706f,-0.7071f,
		-0.5000f,-0.5000f,-0.7071f,  -0.2706f,-0.6533f,-0.7071f,  0.0000f,-0.7071f,-0.7071f,
		0.2706f,-0.6533f,-0.7071f,  0.5000f,-0.5000f,-0.7071f,  0.6533f,-0.2706f,-0.7071f,
		0.7071f,0.0000f,-0.7071f,  0.6533f,0.2706f,-0.7071f,  0.5000f,0.5000f,-0.7071f,
		0.2706f,0.6533f,-0.7071f,  0.0000f,0.3827f,-0.9239f,  -0.1464f,0.3536f,-0.9239f,
		-0.2706f,0.2706f,-0.9239f,  -0.3536f,0.1464f,-0.9239f,  -0.3827f,0.0000f,-0.9239f,
		-0.3536f,-0.1464f,-0.9239f,  -0.2706f,-0.2706f,-0.9239f,  -0.1464f,-0.3536f,-0.9239f,
		0.0000f,-0.3827f,-0.9239f,  0.1464f,-0.3536f,-0.9239f,  0.2706f,-0.2706f,-0.9239f,
		0.3536f,-0.1464f,-0.9239f,  0.3827f,0.0000f,-0.9239f,  0.3536f,0.1464f,-0.9239f,
		0.2706f,0.2706f,-0.9239f,  0.1464f,0.3536f,-0.9239f,  0.0000f,0.0000f,-1.0000f
	};
	static WORD gFaces[224*3] =
	{
		0,1,2, 0,2,3, 0,3,4, 0,4,5, 0,5,6, 0,6,7, 0,7,8, 0,8,9, 0,9,10,
		0,10,11, 0,11,12, 0,12,13, 0,13,14, 0,14,15, 0,15,16, 0,16,1, 1,17,18, 1,18,2,
		2,18,19, 2,19,3, 3,19,20, 3,20,4, 4,20,21, 4,21,5, 5,21,22, 5,22,6, 6,22,23,
		6,23,7, 7,23,24, 7,24,8, 8,24,25, 8,25,9, 9,25,26, 9,26,10, 10,26,27, 10,27,11,
		11,27,28, 11,28,12, 12,28,29, 12,29,13, 13,29,30, 13,30,14, 14,30,31, 14,31,15, 15,31,32,
		15,32,16, 16,32,17, 16,17,1, 17,33,34, 17,34,18, 18,34,35, 18,35,19, 19,35,36, 19,36,20,
		20,36,37, 20,37,21, 21,37,38, 21,38,22, 22,38,39, 22,39,23, 23,39,40, 23,40,24, 24,40,41,
		24,41,25, 25,41,42, 25,42,26, 26,42,43, 26,43,27, 27,43,44, 27,44,28, 28,44,45, 28,45,29,
		29,45,46, 29,46,30, 30,46,47, 30,47,31, 31,47,48, 31,48,32, 32,48,33, 32,33,17, 33,49,50,
		33,50,34, 34,50,51, 34,51,35, 35,51,52, 35,52,36, 36,52,53, 36,53,37, 37,53,54, 37,54,38,
		38,54,55, 38,55,39, 39,55,56, 39,56,40, 40,56,57, 40,57,41, 41,57,58, 41,58,42, 42,58,59,
		42,59,43, 43,59,60, 43,60,44, 44,60,61, 44,61,45, 45,61,62, 45,62,46, 46,62,63, 46,63,47,
		47,63,64, 47,64,48, 48,64,49, 48,49,33, 49,65,66, 49,66,50, 50,66,67, 50,67,51, 51,67,68,
		51,68,52, 52,68,69, 52,69,53, 53,69,70, 53,70,54, 54,70,71, 54,71,55, 55,71,72, 55,72,56,
		56,72,73, 56,73,57, 57,73,74, 57,74,58, 58,74,75, 58,75,59, 59,75,76, 59,76,60, 60,76,77,
		60,77,61, 61,77,78, 61,78,62, 62,78,79, 62,79,63, 63,79,80, 63,80,64, 64,80,65, 64,65,49,
		65,81,82, 65,82,66, 66,82,83, 66,83,67, 67,83,84, 67,84,68, 68,84,85, 68,85,69, 69,85,86,
		69,86,70, 70,86,87, 70,87,71, 71,87,88, 71,88,72, 72,88,89, 72,89,73, 73,89,90, 73,90,74,
		74,90,91, 74,91,75, 75,91,92, 75,92,76, 76,92,93, 76,93,77, 77,93,94, 77,94,78, 78,94,95,
		78,95,79, 79,95,96, 79,96,80, 80,96,81, 80,81,65, 81,97,98, 81,98,82, 82,98,99, 82,99,83,
		83,99,100, 83,100,84, 84,100,101, 84,101,85, 85,101,102, 85,102,86, 86,102,103, 86,103,87, 87,103,104,
		87,104,88, 88,104,105, 88,105,89, 89,105,106, 89,106,90, 90,106,107, 90,107,91, 91,107,108, 91,108,92,
		92,108,109, 92,109,93, 93,109,110, 93,110,94, 94,110,111, 94,111,95, 95,111,112, 95,112,96, 96,112,97,
		96,97,81, 113,98,97, 113,99,98, 113,100,99, 113,101,100, 113,102,101, 113,103,102, 113,104,103, 113,105,104,
		113,106,105, 113,107,106, 113,108,107, 113,109,108, 113,110,109, 113,111,110, 113,112,111, 113,97,112
	};

	const int vcnt = sizeof(gVertices)/(sizeof(float)*3);
	FVF::L  verts[vcnt];
	for (int i=0; i<vcnt; i++) {
		int k=i*3;
		verts[i].set(gVertices[k],gVertices[k+1],gVertices[k+2],C);
	}

	HW.pDevice->SetTransform(D3DTS_WORLD, T.d3d());
	HW.pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	dbg_Draw(D3DPT_TRIANGLELIST,verts,vcnt,gFaces,224);
	HW.pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}
