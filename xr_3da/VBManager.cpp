// VBManager.cpp: implementation of the CVBManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VBManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVBManager::CVBManager()
{
	pTLB	= NULL;
}

CVBManager::~CVBManager()
{

}

IDirect3DVertexBuffer8* CVBManager::Create(
   DWORD dwVertType,
   DWORD dwVertCount,
   BOOL bSoftware)
{
	VERIFY		(dwVertCount);
	HW.Validate	();

	IDirect3DVertexBuffer8*	m_pVertexBuffer;

	// If the device does not support transform and lighting in hardware,
	// make sure the vertex buffers end up in system memory.
	if ((Device.Caps.hwLevel!=D3DX_HWLEVEL_TL) || bSoftware ) vbdesc.dwCaps |= D3DVBCAPS_SYSTEMMEMORY;

	CHK_DX(HW.pD3D->CreateVertexBuffer(&vbdesc, &m_pVertexBuffer, 0L ));

	VERIFY(m_pVertexBuffer);
	return m_pVertexBuffer;
}

#include "fmesh.h"
IDirect3DVertexBuffer8* CVBManager::CreateFromData(
	DWORD dwTypeDest,
	DWORD dwTypeSrc,
	DWORD dwVertCount,
	void *pSource,
	BOOL bSoftware)
{
	void *pDest;
	IDirect3DVertexBuffer8* pVertexBuffer =
		Create(dwTypeDest,dwVertCount,bSoftware);
	CHK_DX(pVertexBuffer->Lock(
		DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS,
		&pDest, NULL ));
	VERIFY(pDest);

	ConvertVertices(dwTypeDest, pDest, dwTypeSrc, pSource, dwVertCount);

    CHK_DX(pVertexBuffer->Unlock());
	return pVertexBuffer;
}

void CVBManager::OnDeviceCreate()
{
	pTLB = Create(FTLVERTEX,TLB_SIZE);
}

void CVBManager::OnDeviceDestroy()
{
	_RELEASE(pTLB);
}

FTLvertex*	CVBManager::TLB_Start()
{
	CHK_DX(pTLB->Lock(
		DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS,
		(void **)&pStart, NULL ));
	return pStart;
}

void CVBManager::TLB_End(FTLvertex *pEnd) {
	VERIFY((pEnd-pStart) < TLB_SIZE);
	VERIFY((pEnd-pStart) > 0);
	CHK_DX(pTLB->Unlock());
	CHK_DX(HW.pDevice->DrawPrimitiveVB(
		D3DPT_TRIANGLESTRIP,
		pTLB,
		0,
		pEnd-pStart,
		0));
	UPDATEC(pEnd-pStart,(pEnd-pStart)/2);
}

void CVBManager::TLB_EndIndexed(FTLvertex *pEnd, WORD *pIndices, DWORD Count) {
	VERIFY((pEnd-pStart) < TLB_SIZE);
	CHK_DX(pTLB->Unlock());
	if (pEnd-pStart) {
		CHK_DX(HW.pDevice->DrawIndexedPrimitiveVB(
			D3DPT_TRIANGLELIST,
			pTLB,
			0,
			pEnd-pStart,
			pIndices,
			Count,
			0));
		UPDATEC(pEnd-pStart,Count/3);
	}
}
