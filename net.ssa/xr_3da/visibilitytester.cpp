#include "stdafx.h"
#include "visibilitytester.h"
#include "HW.h"

const float fASPECT = 480.f/640.f;
void CFrustumClipper::Projection		(float FOV, float N, float F)
{
	fFarPlane	= F;
	float YFov	= deg2rad(FOV);
	float XFov	= deg2rad(FOV/fASPECT);

	// calc window extents in camera coords
	wR=tanf(XFov*0.5f);
	wL=-wR;
	wT=tanf(YFov*0.5f);
	wB=-wT;

	// Projection matrix
	Device.mProject.build_projection(YFov, fASPECT, N, F);
}

void CFrustumClipper::BuildFrustum(Fvector &P, Fvector &D, Fvector &U)
{
	// calc x-axis (viewhoriz) and store cop
	// here we are assuring that vectors are perpendicular & normalized
	Fvector			R;
	D.normalize		();
	R.crossproduct	(D,U);
	R.normalize		();
	U.crossproduct	(R,D);
	U.normalize		();
	COP.set			(P);

	// calculate the corner vertices of the window
	Fvector			sPts[4];  // silhouette points (corners of window)
	Fvector			Offset,T;
	Offset.add		(D,COP);

	sPts[0].mul(R,wR);	T.direct(Offset,U,wT);	sPts[0].add(T);
	sPts[1].mul(R,wL);	T.direct(Offset,U,wT);	sPts[1].add(T);
	sPts[2].mul(R,wL);	T.direct(Offset,U,wB);	sPts[2].add(T);
	sPts[3].mul(R,wR);	T.direct(Offset,U,wB);	sPts[3].add(T);

	// find projector direction vectors (from cop through silhouette pts)
	ProjDirs[0].sub(sPts[0],COP);
	ProjDirs[1].sub(sPts[1],COP);
	ProjDirs[2].sub(sPts[2],COP);
	ProjDirs[3].sub(sPts[3],COP);

	// calc frustum "side" plane eqs (near and FarP already defined)
	Planes[fcpTop].build	(COP, sPts[0], sPts[1]);  // top
	Planes[fcpLeft].build	(COP, sPts[1], sPts[2]);  // left
	Planes[fcpBottom].build	(COP, sPts[2], sPts[3]);  // bottom
	Planes[fcpRight].build	(COP, sPts[3], sPts[0]);  // right

	// Build far plane
	Fvector F[3];
	F[0].direct(COP,ProjDirs[0],fFarPlane);
	F[1].direct(COP,ProjDirs[1],fFarPlane);
	F[2].direct(COP,ProjDirs[2],fFarPlane);
	Planes[fcpFar].build	(F[2], F[1], F[0]);
}
/*
void beginPrimitives()
{
//	Fmaterial mmx;
//	mmx.Set(1,1,1,1);
//	CHK_DX(HW.pDevice->SetMaterial(mmx.d3d()));

	Device.SetMode(vAlpha|vZTest);
	Device.Texture.Set(0);
    CHK_DX(HW.pDevice->SetRenderState	(D3DRENDERSTATE_CULLMODE,	D3DCULL_NONE		));
	CHK_DX(HW.pDevice->SetRenderState	(D3DRENDERSTATE_AMBIENT,	0xffffffff			));
	CHK_DX(HW.pDevice->SetTransform		(D3DTRANSFORMSTATE_WORLD,	precalc_identity.d3d()));
}
void endPrimitives()
{
    CHK_DX(HW.pDevice->SetRenderState	(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW	));
}

void drawLine(Fvector &p1, Fvector &p2, DWORD clr=0xffffffff)
{
	FLITvertex	v[2];
	Fvector		n;
	FPcolor		c;

	c.set		(clr);
	n.set		(0,1,0);
	v[0].set	(p1,n,c,0,0);
	v[1].set	(p2,n,c,0,0);

	Device.Draw.PrimitiveSP(D3DPT_LINELIST,FLITVERTEX,v,2);
}
void drawTriangle(Fvector &p1, Fvector &p2, Fvector &p3, DWORD clr=RGBA_MAKE(255,255,255,128))
{
	FLITvertex	v[3];
	Fvector		n;
	FPcolor		c;

	c.set		(clr);
	n.set		(0,1,0);
	v[0].set	(p1,n,c,0,0);
	v[1].set	(p2,n,c,0,0);
	v[2].set	(p3,n,c,0,0);

	Device.Draw.PrimitiveSP(D3DPT_TRIANGLELIST,FLITVERTEX,v,3);
}
*/
void CFrustumClipper::DrawFrustum()
{
	/*
	beginPrimitives();
	Fvector F[4];
	F[0].direct(COP,ProjDirs[0],fFarPlane);
	F[1].direct(COP,ProjDirs[1],fFarPlane);
	F[2].direct(COP,ProjDirs[2],fFarPlane);
	F[3].direct(COP,ProjDirs[3],fFarPlane);

	DWORD C = RGBA_MAKE(255,0,0,255);
	drawTriangle(COP,F[0],F[1]);
	drawTriangle(COP,F[1],F[2]);
	drawTriangle(COP,F[2],F[3]);
	drawTriangle(COP,F[3],F[0]);
	drawLine	(COP,F[0],C);
	drawLine	(COP,F[1],C);
	drawLine	(COP,F[2],C);
	drawLine	(COP,F[3],C);

	endPrimitives();
	*/
}
