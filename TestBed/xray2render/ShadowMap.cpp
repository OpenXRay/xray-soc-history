#include "stdafx.h"

#include "r_shader.h"
#include "r_constants_cache.h"


enum COMBINE_MODE
{
	CM_NORMAL,
	CM_DBG_NORMALS,
	CM_DBG_ACCUMULATOR
};
//-----------------------------------------------------------------------------
// Globals variables and definitions
//-----------------------------------------------------------------------------

#define SHADOW_MAP_FORMAT	D3DFMT_R32F
#define SHADOW_MAP_SIZE		256
#define OVERLAY_SIZE		128
#define DEPTH_RANGE			4.0f

// 2D
struct TVERTEX
{
    D3DXVECTOR4 p;
    FLOAT       tu, tv;
};
#define TVERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_TEX1)
D3DVERTEXELEMENT9 decl_vert2D[] =
{
	{ 0, 0,		D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT,	0 },
	{ 0, 16,	D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
	D3DDECL_END()
};

// 3D
struct VERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    FLOAT       tu, tv;
};
D3DVERTEXELEMENT9 decl_vert[] =
{
	{ 0, 0,		D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,	0 },
	{ 0, 12,	D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,		0 },
	{ 0, 24,	D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,	0 },
	D3DDECL_END()
};

//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: Main class to run this application. Most functionality is inherited
//       from the CD3DApplication base class.
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	//  ************************
	//	**** Generic stuff  ****
	//  ************************

    // Font for drawing text
    CD3DFont*						m_pFont;
    CD3DArcBall						m_ArcBall;

	// Model
	LPDIRECT3DVERTEXBUFFER9			m_pModelVB;
	LPDIRECT3DINDEXBUFFER9			m_pModelIB;
	DWORD							m_dwModelNumVerts;
	DWORD							m_dwModelNumFaces;
	FLOAT							m_fModelSize;

	LPDIRECT3DVERTEXBUFFER9			m_pFloorVB;
	LPDIRECT3DVERTEXBUFFER9			m_pQuadVB;

	LPDIRECT3DVERTEXDECLARATION9	m_pDeclVert;
	LPDIRECT3DVERTEXDECLARATION9	m_pDeclVert2D;

	// xr2
	R_constants						cc;

	//  ************************
	//	**** Deferred shading **
	//  ************************

	// Surfaces
	LPDIRECT3DTEXTURE9				d_Position;		// 64bit, fat	(x,y,z,0)				(eye-space)
	LPDIRECT3DSURFACE9				d_Position_S;
	LPDIRECT3DTEXTURE9				d_Normal;		// 64bit, fat	(x,y,z,0)				(eye-space)
	LPDIRECT3DSURFACE9				d_Normal_S;
	LPDIRECT3DTEXTURE9				d_Color;		// 64bit, fat	(r,g,b,specular-gloss)
	LPDIRECT3DSURFACE9				d_Color_S;
	LPDIRECT3DTEXTURE9				d_Accumulator;	// 32bit		(r,g,b,specular)
	LPDIRECT3DSURFACE9				d_Accumulator_S;

	// Special textures
	LPDIRECT3DTEXTURE9				t_SpecularPower_32;

	// Matrices
	D3DXMATRIX						dm_model2world;
	D3DXMATRIX						dm_model2world2view;
	D3DXMATRIX						dm_model2world2view2projection;

	D3DXMATRIX						dm_2world;
	D3DXMATRIX						dm_2view;
	D3DXMATRIX						dm_2projection;

	D3DXMATRIX						dm_world2view2projection;

	// Shaders
	R_shader						s_Scene2fat;
	R_shader						s_CombineDBG_Normals;
	R_shader						s_CombineDBG_Accumulator;
	R_shader						s_Light_Direct;

	//  ************************
	//	**** Shadow mapping ****
	//  ************************

    // Transform matrices
    D3DXMATRIX						m_matModelMVP;
    D3DXMATRIX						m_matShadowModelMVP;
    D3DXMATRIX						m_matShadowModelTex;

    D3DXMATRIX						m_matFloorMVP;
    D3DXMATRIX						m_matShadowFloorMVP;
    D3DXMATRIX						m_matShadowFloorTex;

	D3DXVECTOR4						m_vecLightDirModel;
	D3DXVECTOR4						m_vecLightDirFloor;

	// Shadow map
	LPDIRECT3DTEXTURE9				m_pShadowMap;
	LPDIRECT3DSURFACE9				m_pShadowMapSurf;
	LPDIRECT3DSURFACE9				m_pShadowMapZ;
    LPDIRECT3DVERTEXBUFFER9			m_pOverlayVB;

	// Shaders
	LPDIRECT3DVERTEXSHADER9			m_pSceneVS;
	LPDIRECT3DPIXELSHADER9			m_pScenePS;
	LPDIRECT3DVERTEXSHADER9			m_pShadowMapVS;
	LPDIRECT3DPIXELSHADER9			m_pShadowMapPS;
	LPDIRECT3DPIXELSHADER9			m_pShowMapPS;

public:
    CMyD3DApplication();

    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render			();
    HRESULT FrameMove		();
    HRESULT FinalCleanup	();
    HRESULT ConfirmDevice	(D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT Format);

	// Shadow mapping-base
	HRESULT RenderShadowMap	();
	HRESULT RenderScene		();
	HRESULT RenderOverlay	();

	HRESULT RenderFAT					();
	HRESULT RenderLight_Direct			();
	HRESULT RenderCombine				(COMBINE_MODE M);
	HRESULT RenderCombineDBG_Normals	();
	HRESULT RenderCombineDBG_Accumulator();

	HRESULT UpdateTransform	();

    LRESULT MsgProc			(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

CMyD3DApplication g_d3dApp;

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    InitCommonControls	();
	InitMath			();
    if (FAILED(g_d3dApp.Create(hInst)))
        return 0;

    return g_d3dApp.Run();
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Constructor
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    // Override base class members
	m_strWindowTitle					= _T("xray2 : render");
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
	m_dwCreationWidth					= 512;
	m_dwCreationHeight					= 512;

    m_pFont								= new CD3DFont(_T("Arial"), 12, D3DFONT_BOLD);

    m_pModelVB							= NULL;
    m_pModelIB							= NULL;
    m_pFloorVB							= NULL;

	m_pSceneVS							= NULL;
	m_pScenePS							= NULL;

	m_pShadowMap						= NULL;
	m_pShadowMapSurf					= NULL;
	m_pShadowMapZ						= NULL;

	m_pShadowMapVS						= NULL;
	m_pShadowMapPS						= NULL;
	m_pShowMapPS						= NULL;

    m_pOverlayVB						= NULL;

	// d-render
	d_Position							= NULL;
	d_Position_S						= NULL;
	d_Normal							= NULL;
	d_Normal_S							= NULL;
	d_Color								= NULL;
	d_Color_S							= NULL;
	d_Accumulator						= NULL;
	d_Accumulator_S						= NULL;
}


HRESULT CMyD3DApplication::OneTimeSceneInit()	{ return S_OK; }

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove	()
{
	UpdateTransform	();
    return			S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render		()
{
    // Begin the scene
    if (SUCCEEDED(m_pd3dDevice->BeginScene()))
    {
		// RenderShadowMap	();
		// RenderScene			();
		// RenderOverlay		();

		RenderFAT					();
		RenderLight_Direct			();
		RenderCombine				(CM_DBG_ACCUMULATOR);

        // Output statistics
        m_pFont->DrawText			(OVERLAY_SIZE + 12,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats);
        m_pFont->DrawText			(OVERLAY_SIZE + 12, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats);

        // End the scene.
        m_pd3dDevice->EndScene		();
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize device-dependent objects. This is the place to create mesh
//       and texture objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    LPDIRECT3DVERTEXBUFFER9 pMeshSrcVB;
    LPDIRECT3DINDEXBUFFER9  pMeshSrcIB;
    VERTEX*					pSrc;
    VERTEX*					pDst;
    TVERTEX*				pDstT;
    CD3DMesh                Mesh;
	FLOAT					fModelRad;

    // Initialize the font's internal textures
    m_pFont->InitDeviceObjects(m_pd3dDevice);

	// Load model
	if (FAILED(Mesh.Create(m_pd3dDevice, _T("media\\star.x"))))			return D3DAPPERR_MEDIANOTFOUND;

	// Fix vertex contents
    Mesh.SetFVF			(m_pd3dDevice, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
	
	// Create model VB
    m_dwModelNumVerts	= Mesh.GetSysMemMesh()->GetNumVertices();
    m_pd3dDevice->CreateVertexBuffer(m_dwModelNumVerts * sizeof(VERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pModelVB, NULL);
    
	// Copy vertices and compute bounding sphere
    Mesh.GetSysMemMesh()->GetVertexBuffer(&pMeshSrcVB);
    pMeshSrcVB->Lock	(0, 0, (void**)&pSrc, 0);
    m_pModelVB->Lock	(0, 0, (void**)&pDst, 0);
    memcpy				(pDst, pSrc, m_dwModelNumVerts * sizeof(VERTEX));
	D3DXVECTOR3			vecModelCenter;
	D3DXComputeBoundingSphere(&pSrc->p, m_dwModelNumVerts, sizeof(VERTEX), &vecModelCenter, &fModelRad);
    m_pModelVB->Unlock	();
    pMeshSrcVB->Unlock	();
    pMeshSrcVB->Release	();

	m_fModelSize = fModelRad * 2.0f;
	
	// Create model IB
    m_dwModelNumFaces = Mesh.GetSysMemMesh()->GetNumFaces();
    m_pd3dDevice->CreateIndexBuffer(m_dwModelNumFaces * 3 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pModelIB, NULL);
    
	// Copy indices
    Mesh.GetSysMemMesh()->GetIndexBuffer(&pMeshSrcIB);
    pMeshSrcIB->Lock(0, 0, (void**)&pSrc, 0);
    m_pModelIB->Lock(0, 0, (void**)&pDst, 0);
    memcpy(pDst, pSrc, 3 * m_dwModelNumFaces * sizeof(WORD));
    m_pModelIB->Unlock();
    pMeshSrcIB->Unlock();
    pMeshSrcIB->Release();

	// Create floor VB
    m_pd3dDevice->CreateVertexBuffer(4 * sizeof(VERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pFloorVB, NULL);
	m_pFloorVB->Lock(0, 0, (void**)&pDst, 0);
	pDst[0].p = D3DXVECTOR3(-10.0f, 0.0f, 10.0f);
	pDst[0].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pDst[0].tu = 0.0f;
	pDst[0].tv = 1.0f;
	pDst[1].p = D3DXVECTOR3(10.0f, 0.0f, 10.0f);
	pDst[1].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pDst[1].tu = 1.0f;
	pDst[1].tv = 1.0f;
	pDst[2].p = D3DXVECTOR3(-10.0f, 0.0f, -10.0f);
	pDst[2].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pDst[2].tu = 0.0f;
	pDst[2].tv = 0.0f;
	pDst[3].p = D3DXVECTOR3(10.0f, 0.0f, -10.0f);
	pDst[3].n = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pDst[3].tu = 1.0f;
	pDst[3].tv = 0.0f;
	m_pFloorVB->Unlock();

	// Create overlay VB
    m_pd3dDevice->CreateVertexBuffer(4 * sizeof(TVERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pOverlayVB, NULL);
	m_pOverlayVB->Lock(0, 0, (void**)&pDstT, 0);
	pDstT[0].p = D3DXVECTOR4(4.5f, 4.5f + OVERLAY_SIZE, 0.0f, 1.0f);
	pDstT[0].tu = 0.0f;
	pDstT[0].tv = 1.0f;
	pDstT[1].p = D3DXVECTOR4(4.5f + OVERLAY_SIZE, 4.5f + OVERLAY_SIZE, 0.0f, 1.0f);
	pDstT[1].tu = 1.0f;
	pDstT[1].tv = 1.0f;
	pDstT[2].p = D3DXVECTOR4(4.5f, 4.5f, 0.0f, 1.0f);
	pDstT[2].tu = 0.0f;
	pDstT[2].tv = 0.0f;
	pDstT[3].p = D3DXVECTOR4(4.5f + OVERLAY_SIZE, 4.5f, 0.0f, 1.0f);
	pDstT[3].tu = 1.0f;
	pDstT[3].tv = 0.0f;
	m_pOverlayVB->Unlock();

	// Create overlay VB
	{
		const float	 w	= float(m_d3dsdBackBuffer.Width),	h = float(m_d3dsdBackBuffer.Height);
		const float _w	= w-1, _h = h-1;
		m_pd3dDevice->CreateVertexBuffer	(4 * sizeof(TVERTEX), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pQuadVB, NULL);
		m_pQuadVB->Lock						(0, 0, (void**)&pDstT, 0);
		pDstT[0].p	= D3DXVECTOR4(0, _h,	0.0f, 1.0f);
		pDstT[0].tu = 0.0f;
		pDstT[0].tv = 1.0f;
		pDstT[1].p	= D3DXVECTOR4(_w, _h,	0.0f, 1.0f);
		pDstT[1].tu = 1.0f;
		pDstT[1].tv = 1.0f;
		pDstT[2].p	= D3DXVECTOR4(0, 0,		0.0f, 1.0f);
		pDstT[2].tu = 0.0f;
		pDstT[2].tv = 0.0f;
		pDstT[3].p	= D3DXVECTOR4(_w, 0,	0.0f, 1.0f);
		pDstT[3].tu = 1.0f;
		pDstT[3].tv = 0.0f;
		m_pQuadVB->Unlock					();
	}

	// Create shader declaration(s)
	if (FAILED(m_pd3dDevice->CreateVertexDeclaration(decl_vert, &m_pDeclVert)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateVertexDeclaration(decl_vert2D, &m_pDeclVert2D)))
		return E_FAIL;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Restore device-memory objects and state after a device is created or
//       resized.
//-----------------------------------------------------------------------------

HRESULT CreateRT(IDirect3DDevice9* D, DWORD w, DWORD h, D3DFORMAT f, LPDIRECT3DTEXTURE9* pT, LPDIRECT3DSURFACE9* pS)
{
	if (FAILED(D->CreateTexture(w,h, 1, D3DUSAGE_RENDERTARGET, f, D3DPOOL_DEFAULT, pT, NULL)))	return E_FAIL;
	if (FAILED((*pT)->GetSurfaceLevel(0, pS)))														return E_FAIL;
	return S_OK;
}

HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    m_pFont->RestoreDeviceObjects();

	UpdateTransform		();

	// Create targets
	DWORD				w			= m_d3dsdBackBuffer.Width, h = m_d3dsdBackBuffer.Height;
	CreateRT						(m_pd3dDevice,w,h,D3DFMT_A16B16G16R16F,	&d_Position,&d_Position_S);
	CreateRT						(m_pd3dDevice,w,h,D3DFMT_A16B16G16R16F,	&d_Normal,&d_Normal_S);
	CreateRT						(m_pd3dDevice,w,h,D3DFMT_A16B16G16R16,	&d_Color,&d_Color_S);
	CreateRT						(m_pd3dDevice,w,h,D3DFMT_A8R8G8B8,		&d_Accumulator,&d_Accumulator_S);

	// Create shaders
	s_Scene2fat.compile				(m_pd3dDevice,"shaders\\D\\fat_base.s");
	s_CombineDBG_Normals.compile	(m_pd3dDevice,"shaders\\D\\cm_dbg_normals.s");
	s_CombineDBG_Accumulator.compile(m_pd3dDevice,"shaders\\D\\cm_dbg_accumulator.s");
	s_Light_Direct.compile			(m_pd3dDevice,"shaders\\D\\light_direct.s");

	// Create special textures
	{
		const DWORD	size				= 256;
		float		array[size];
		for (DWORD it=0; it<size; it++)
		{
			float	v		= float(it)/float(size);
			array	[it]	= powf		(v,32);
		}
		m_pd3dDevice->CreateTexture		(size,1, 1, 0, D3DFMT_R16F, D3DPOOL_MANAGED, &t_SpecularPower_32, NULL);
		D3DLOCKED_RECT					R;
		t_SpecularPower_32->LockRect	(0,&R,0, D3DLOCK_DISCARD);
		D3DXFloat32To16Array			((D3DXFLOAT16*)R.pBits,array,size);
		t_SpecularPower_32->UnlockRect	(0);
	}

	// Create shadow map texture and retrieve surface
	if (FAILED(m_pd3dDevice->CreateTexture(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1, 
		D3DUSAGE_RENDERTARGET, SHADOW_MAP_FORMAT, D3DPOOL_DEFAULT, &m_pShadowMap, NULL)))
		return E_FAIL;
	if (FAILED(m_pShadowMap->GetSurfaceLevel(0, &m_pShadowMapSurf)))
		return E_FAIL;
	// Create depth buffer for shadow map rendering
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pShadowMapZ, NULL)))
		return E_FAIL;

    m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Create model shaders
	SAFE_RELEASE(m_pSceneVS);
	LoadVertexShader(m_pd3dDevice, "shaders\\shadowscene.vsh", &m_pSceneVS);
	SAFE_RELEASE(m_pScenePS);
	LoadPixelShader(m_pd3dDevice, "shaders\\shadowscene.psh", &m_pScenePS);

	// Create shadow map shaders
	SAFE_RELEASE(m_pShadowMapVS);
	LoadVertexShader(m_pd3dDevice, "shaders\\shadowmap.vsh", &m_pShadowMapVS);
	SAFE_RELEASE(m_pShadowMapPS);
	LoadPixelShader(m_pd3dDevice, "shaders\\shadowmap.psh", &m_pShadowMapPS);
	SAFE_RELEASE(m_pShowMapPS);
	LoadPixelShader(m_pd3dDevice, "shaders\\showmap.psh", &m_pShowMapPS);

    m_ArcBall.SetWindow(m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 1.0f);
    m_ArcBall.SetRadius(3.0f);
	m_ArcBall.SetRightHanded(TRUE);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the device-dependent objects are about to be lost.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    m_pFont->InvalidateDeviceObjects();

	// Delete shaders
	SAFE_RELEASE(m_pSceneVS);
	SAFE_RELEASE(m_pScenePS);
	SAFE_RELEASE(m_pShadowMapVS);
	SAFE_RELEASE(m_pShadowMapPS);
	SAFE_RELEASE(m_pShowMapPS);

	// Delete surfaces
	SAFE_RELEASE(m_pShadowMapSurf);
	SAFE_RELEASE(m_pShadowMap);
	SAFE_RELEASE(m_pShadowMapZ);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    m_pFont->DeleteDeviceObjects();

    SAFE_RELEASE(m_pModelVB);
    SAFE_RELEASE(m_pModelIB);
    SAFE_RELEASE(m_pFloorVB);
    SAFE_RELEASE(m_pDeclVert);
	SAFE_RELEASE(m_pDeclVert2D);

    SAFE_RELEASE(m_pOverlayVB);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE(m_pFont);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
	BOOL	bDebug	= FALSE;

	if (bDebug)
	{
		if (dwBehavior & D3DCREATE_PUREDEVICE)					return E_FAIL;
		if (pCaps->DeviceType==D3DDEVTYPE_HAL)					return E_FAIL;
		// if ((dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0) return E_FAIL;
		return S_OK;
	} else {
		if (dwBehavior & D3DCREATE_PUREDEVICE)					return E_FAIL;
		if (pCaps->PixelShaderVersion < D3DPS_VERSION(2, 0))	return E_FAIL;
		if (pCaps->VertexShaderVersion < D3DVS_VERSION(2, 0))	return E_FAIL;
	}

	/*
	if ((dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0)
	{
		return E_FAIL;
	}

	// If device doesn't support 1.1 vertex shaders in HW, switch to SWVP.
    if (pCaps->VertexShaderVersion < D3DVS_VERSION(1, 1))
    {
        if ((dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) == 0)
        {
            return E_FAIL;
        }
    }
	*/

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderShadowMap()
// Desc: Called to render shadow map for the light. Outputs per-pixel distance
//       normalized to 0..1 range to the floating point buffer.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderShadowMap()
{
	LPDIRECT3DSURFACE9	pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9		oldViewport;

	// Save old render taget
	m_pd3dDevice->GetRenderTarget		(0, &pOldBackBuffer);
	m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
	// Save old viewport
	m_pd3dDevice->GetViewport			(&oldViewport);

	// Set new render target
	m_pd3dDevice->SetRenderTarget		(0, m_pShadowMapSurf);
	m_pd3dDevice->SetDepthStencilSurface(m_pShadowMapZ);

	// Setup shadow map viewport
	D3DVIEWPORT9	shadowViewport;
	shadowViewport.X = 0;
	shadowViewport.Y = 0;
	shadowViewport.Width  = SHADOW_MAP_SIZE;
	shadowViewport.Height = SHADOW_MAP_SIZE;
	shadowViewport.MinZ = 0.0f;
	shadowViewport.MaxZ = 1.0f;
	m_pd3dDevice->SetViewport(&shadowViewport);

	// Clear viewport
    m_pd3dDevice->Clear						(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0L);
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE, D3DCULL_CW);

	// Set shadow map shader
	m_pd3dDevice->SetPixelShader			(m_pShadowMapPS);
	m_pd3dDevice->SetVertexShader			(m_pShadowMapVS);
	m_pd3dDevice->SetVertexDeclaration		(m_pDeclVert);

	D3DXVECTOR4	vRange						= D3DXVECTOR4	(1.0f / DEPTH_RANGE, 0.0f, 0.0f, 0.0f);
	m_pd3dDevice->SetVertexShaderConstantF	(12, vRange, 1);

	// Render model
	m_pd3dDevice->SetVertexShaderConstantF	(0, m_matShadowModelMVP, 4);
	m_pd3dDevice->SetStreamSource			(0, m_pModelVB, 0, sizeof(VERTEX));
	m_pd3dDevice->SetIndices				(m_pModelIB);
	m_pd3dDevice->DrawIndexedPrimitive		(D3DPT_TRIANGLELIST, 0, 0, m_dwModelNumVerts, 0, m_dwModelNumFaces);

	//Render floor
	m_pd3dDevice->SetVertexShaderConstantF	(0, m_matShadowFloorMVP, 4);
	m_pd3dDevice->SetStreamSource			(0, m_pFloorVB, 0, sizeof(VERTEX));
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

	// Restore old render target
	m_pd3dDevice->SetRenderTarget			(0, pOldBackBuffer);
	m_pd3dDevice->SetDepthStencilSurface	(pOldZBuffer);
	pOldBackBuffer->Release					();
	pOldZBuffer->Release					();

	// Restore old viewport
	m_pd3dDevice->SetViewport				(&oldViewport);

	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderScene()
// Desc: Renders the scene objects while performing shadow mapping.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderScene	()
{
	m_pd3dDevice->Clear						(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00404080, 1.0f, 0L);

	D3DXVECTOR4 vDiffuseFloor				(0.75f, 0.75f, 0.75f, 1.0f);
	D3DXVECTOR4 vDiffuseModel				(1.0f, 1.0f, 1.0f, 1.0f);

	m_pd3dDevice->SetTexture				(0, m_pShadowMap);

	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	// Pixel shader and jittering
	// jittered sample offset table for 1k shadow maps
	m_pd3dDevice->SetPixelShader			(m_pScenePS);
	float	C								= float(1024)/float(SHADOW_MAP_SIZE);
	m_pd3dDevice->SetPixelShaderConstantF	(0,C*D3DXVECTOR4(-0.000692, -0.000868, -0.002347, 0.000450),1);
	m_pd3dDevice->SetPixelShaderConstantF	(1,C*D3DXVECTOR4(0.000773, -0.002042, -0.001592, 0.001880),1);
	m_pd3dDevice->SetPixelShaderConstantF	(2,C*D3DXVECTOR4(-0.001208, -0.001198, -0.000425, -0.000915),1);
	m_pd3dDevice->SetPixelShaderConstantF	(3,C*D3DXVECTOR4(-0.000050, 0.000105, -0.000753, 0.001719),1);
	m_pd3dDevice->SetPixelShaderConstantF	(4,C*D3DXVECTOR4(-0.001855, -0.000004, 0.001140, -0.001212),1);
	m_pd3dDevice->SetPixelShaderConstantF	(5,C*D3DXVECTOR4(0.000684, 0.000273, 0.000177, 0.000647),1);
	m_pd3dDevice->SetPixelShaderConstantF	(6,C*D3DXVECTOR4(-0.001448, 0.002095, 0.000811, 0.000421),1);
	m_pd3dDevice->SetPixelShaderConstantF	(7,C*D3DXVECTOR4(0.000542, 0.001491, 0.000537, 0.002367),1);

	// Vertex Shader
	m_pd3dDevice->SetVertexShader			(m_pSceneVS);
	m_pd3dDevice->SetVertexDeclaration		(m_pDeclVert);

	m_pd3dDevice->SetVertexShaderConstantF	(31, vDiffuseModel, 1);
	D3DXVECTOR4	vZBias = D3DXVECTOR4(0.1f, -0.01f, 1.0f, 0.0f);
	m_pd3dDevice->SetVertexShaderConstantF	(32, vZBias, 1);
	D3DXVECTOR4	vRange = D3DXVECTOR4(1.0f / DEPTH_RANGE, 0.0f, 0.0f, 0.0f);
	m_pd3dDevice->SetVertexShaderConstantF	(12, vRange, 1);

	// Render floor
	m_pd3dDevice->SetVertexShaderConstantF	(0, m_matFloorMVP, 4);
	m_pd3dDevice->SetVertexShaderConstantF	(4, m_matShadowFloorMVP, 4);
	m_pd3dDevice->SetVertexShaderConstantF	(8, m_matShadowFloorTex, 4);
	m_pd3dDevice->SetVertexShaderConstantF	(30, m_vecLightDirFloor, 1);
	m_pd3dDevice->SetVertexShaderConstantF	(31, vDiffuseFloor, 1);
	m_pd3dDevice->SetStreamSource			(0, m_pFloorVB, 0, sizeof(VERTEX));
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

	// Render model
	m_pd3dDevice->SetVertexShaderConstantF	(0, m_matModelMVP, 4);
	m_pd3dDevice->SetVertexShaderConstantF	(4, m_matShadowModelMVP, 4);
	m_pd3dDevice->SetVertexShaderConstantF	(8, m_matShadowModelTex, 4);
	m_pd3dDevice->SetVertexShaderConstantF	(30, m_vecLightDirModel, 1);
	m_pd3dDevice->SetVertexShaderConstantF	(31, vDiffuseModel, 1);
	m_pd3dDevice->SetStreamSource			(0, m_pModelVB, 0, sizeof(VERTEX));
	m_pd3dDevice->SetIndices				(m_pModelIB);
	m_pd3dDevice->DrawIndexedPrimitive		(D3DPT_TRIANGLELIST, 0, 0, m_dwModelNumVerts, 0, m_dwModelNumFaces);

	m_pd3dDevice->SetTexture(0, NULL);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderFAT()
// Desc: Renders fat-buffer
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderFAT	()
{
	LPDIRECT3DSURFACE9						pBaseTarget;

	// Set new render targets
	m_pd3dDevice->GetRenderTarget			(0, &pBaseTarget	);
	m_pd3dDevice->SetRenderTarget			(0, d_Position_S	);
	m_pd3dDevice->SetRenderTarget			(1, d_Normal_S		);
	m_pd3dDevice->SetRenderTarget			(2, d_Color_S		);
	m_pd3dDevice->Clear						(0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00, 1.0f, 0L);

	// samplers and texture
	m_pd3dDevice->SetTexture				(0, 0);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MIPFILTER,	D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR);

	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE, D3DCULL_CCW);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_Scene2fat.ps);
	m_pd3dDevice->SetVertexShader			(s_Scene2fat.vs);
	m_pd3dDevice->SetVertexDeclaration		(m_pDeclVert);
	cc.set									(s_Scene2fat.constants.get("m_model2view"),				*((Fmatrix*)&dm_model2world2view));
	cc.set									(s_Scene2fat.constants.get("m_model2view2projection"),	*((Fmatrix*)&dm_model2world2view2projection));
	cc.flush								(m_pd3dDevice);

	// Render model
	m_pd3dDevice->SetStreamSource			(0, m_pModelVB, 0, sizeof(VERTEX));
	m_pd3dDevice->SetIndices				(m_pModelIB);
	m_pd3dDevice->DrawIndexedPrimitive		(D3DPT_TRIANGLELIST, 0, 0, m_dwModelNumVerts, 0, m_dwModelNumFaces);

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);
	m_pd3dDevice->SetRenderTarget			(0, pBaseTarget	);
	m_pd3dDevice->SetRenderTarget			(1, 0	);
	m_pd3dDevice->SetRenderTarget			(2, 0	);

	return S_OK;
}

HRESULT CMyD3DApplication::RenderCombine	(COMBINE_MODE M)
{
	if (M==CM_DBG_NORMALS)					return RenderCombineDBG_Normals();
	if (M==CM_DBG_ACCUMULATOR)				return RenderCombineDBG_Accumulator();

	return E_FAIL;
}

//-----------------------------------------------------------------------------
// Name: RenderLight_Direct				()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderLight_Direct	()
{
	LPDIRECT3DSURFACE9						pBaseTarget;

	// Set new render targets
	m_pd3dDevice->GetRenderTarget			(0, &pBaseTarget	);
	m_pd3dDevice->SetRenderTarget			(0, d_Accumulator_S	);
	m_pd3dDevice->Clear						(0L, NULL, D3DCLEAR_TARGET, 0x00, 1.0f, 0L);

	// samplers and texture (POS)
	m_pd3dDevice->SetTexture				(0, d_Position);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// samplers and texture (NORM)
	m_pd3dDevice->SetTexture				(1, d_Normal);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(1, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// samplers and texture (Power32)
	m_pd3dDevice->SetTexture				(2, t_SpecularPower_32);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR);
	m_pd3dDevice->SetSamplerState			(2, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_Light_Direct.ps);
	m_pd3dDevice->SetVertexShader			(s_Light_Direct.vs);
	m_pd3dDevice->SetFVF					(TVERTEX_FVF);
	D3DXVECTOR3 vLightDir					= D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	D3DXMATRIX	mInvView;
	D3DXVec3Normalize						(&vLightDir, &vLightDir);
	D3DXMatrixInverse						(&mInvView,0,&dm_2view);
	D3DXVec3TransformNormal					(&vLightDir, &vLightDir,&mInvView);
	D3DXVec3Normalize						(&vLightDir, &vLightDir);
	cc.set									(s_Light_Direct.constants.get("light_direction"),	vLightDir.x,vLightDir.y,vLightDir.z,0	);
	cc.set									(s_Light_Direct.constants.get("light_color"),		.3f,		.3f,		1.,			.7	);
	cc.flush								(m_pd3dDevice);

	// Blend mode - directional light comes first - means no blending
	m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, FALSE);

	// Render Quad
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE,	D3DCULL_NONE);
	m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

	// Second light
	m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pd3dDevice->SetRenderState			(D3DRS_SRCBLEND,	D3DBLEND_ONE);
	m_pd3dDevice->SetRenderState			(D3DRS_DESTBLEND,	D3DBLEND_ONE);
	cc.set									(s_Light_Direct.constants.get("light_direction"),	-vLightDir.x,-vLightDir.y,-vLightDir.z,0	);
	cc.set									(s_Light_Direct.constants.get("light_color"),		.9f,		.3f,		.0,			.7		);
	cc.flush								(m_pd3dDevice);
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);
	m_pd3dDevice->SetRenderState			(D3DRS_ALPHABLENDENABLE, FALSE);

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);
	m_pd3dDevice->SetTexture				(1, NULL);
	m_pd3dDevice->SetRenderTarget			(0, pBaseTarget	);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderCombineDBG_Normals			()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderCombineDBG_Normals	()
{
	// samplers and texture
	m_pd3dDevice->SetTexture				(0, d_Normal);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_CombineDBG_Normals.ps);
	m_pd3dDevice->SetVertexShader			(s_CombineDBG_Normals.vs);
	m_pd3dDevice->SetFVF					(TVERTEX_FVF);
	cc.flush								(m_pd3dDevice);

	// Render Quad
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE,	D3DCULL_NONE);
	m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderCombineDBG_Accumulator			()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderCombineDBG_Accumulator	()
{
	// samplers and texture
	m_pd3dDevice->SetTexture				(0, d_Accumulator);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MINFILTER,	D3DTEXF_POINT);
	m_pd3dDevice->SetSamplerState			(0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT);

	// Shader and params
	m_pd3dDevice->SetPixelShader			(s_CombineDBG_Accumulator.ps);
	m_pd3dDevice->SetVertexShader			(s_CombineDBG_Accumulator.vs);
	m_pd3dDevice->SetFVF					(TVERTEX_FVF);
	cc.flush								(m_pd3dDevice);

	// Render Quad
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE,	D3DCULL_NONE);
	m_pd3dDevice->SetStreamSource			(0, m_pQuadVB, 0, sizeof(TVERTEX));
	m_pd3dDevice->DrawPrimitive				(D3DPT_TRIANGLESTRIP, 0, 2);

	// Cleanup
	m_pd3dDevice->SetTexture				(0, NULL);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderOverlay()
// Desc: Displays the content of the shadow map.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderOverlay()
{
    m_pd3dDevice->SetSamplerState		(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    m_pd3dDevice->SetSamplerState		(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pd3dDevice->SetRenderState		(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pd3dDevice->SetFVF				(TVERTEX_FVF);
	m_pd3dDevice->SetStreamSource		(0, m_pOverlayVB, 0, sizeof(TVERTEX));
	m_pd3dDevice->SetPixelShader		(m_pShowMapPS);
	m_pd3dDevice->SetTexture			(0, m_pShadowMap);
	m_pd3dDevice->SetTextureStageState	(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pd3dDevice->SetTextureStageState	(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	m_pd3dDevice->DrawPrimitive			(D3DPT_TRIANGLESTRIP, 0, 2);
	m_pd3dDevice->SetTexture			(0, NULL);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: UpdateTransform()
// Desc: Re-computes all transform parameters for shadow mapping.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::UpdateTransform()
{
	// Model offset
	D3DXVECTOR3 vModelOffs		= D3DXVECTOR3(0.0f, 2.0f, 0.0f);

    // Set the transform matrices
    D3DXVECTOR3 vEyePt			= D3DXVECTOR3(0.0f, 3.0f, -4.0f);
    D3DXVECTOR3 vLookatPt		= D3DXVECTOR3(0.0f, 2.0f,  0.0f);
    D3DXVECTOR3 vUpVec			= D3DXVECTOR3(0.0f, 1.0f,  0.0f);
    FLOAT       fAspect			= (FLOAT)m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;

	D3DXMATRIX	matWorldModel, matWorldFloor, matView, matProj, mat;
	D3DXMATRIX	matShadowView, matShadowProj, mat2;

    D3DXMatrixLookAtLH			(&matView, &vEyePt, &vLookatPt, &vUpVec);
    D3DXMatrixPerspectiveFovLH	(&matProj, D3DX_PI / 3, fAspect, 1.0f, 100.0f);
	D3DXMatrixMultiply			(&mat, &matView, &matProj);

	dm_2view					= matView;
	dm_2projection				= matProj;
	dm_world2view2projection	= mat;

	D3DXMATRIX matTranslate;
	D3DXMatrixTranslation		(&matTranslate, vModelOffs.x, vModelOffs.y, vModelOffs.z);
	D3DXMatrixMultiply			(&matWorldModel, m_ArcBall.GetRotationMatrix(), &matTranslate);
	dm_model2world				= matWorldModel;
	D3DXMatrixMultiply			(&dm_model2world2view,				&dm_model2world, &dm_2view);
	D3DXMatrixMultiply			(&dm_model2world2view2projection,	&dm_model2world, &dm_world2view2projection);
	D3DXMatrixMultiplyTranspose	(&m_matModelMVP, &matWorldModel,	&mat);

	D3DXMatrixIdentity			(&matWorldFloor);
	D3DXMatrixMultiplyTranspose	(&m_matFloorMVP, &matWorldFloor,	&mat);

	// Light direction
	D3DXVECTOR3 vLightDir		= D3DXVECTOR3(2.0f, 1.0f, -1.0f);
	D3DXVec3Normalize			(&vLightDir, &vLightDir);

	// Setup shadow map transform
	vLookatPt					= vModelOffs;
	vEyePt						= vLookatPt + vLightDir * (m_fModelSize / 2.0f);
    D3DXMatrixLookAtLH			(&matView, &vEyePt, &vLookatPt, &vUpVec);

	// Projection for directional light
    D3DXMatrixOrthoLH			(&matProj, 5.0f, 5.0f, 0.0f, DEPTH_RANGE);

	D3DXMATRIX matShadowModelMVP, matShadowFloorMVP;
	D3DXMatrixMultiply			(&mat, &matView, &matProj);
	D3DXMatrixMultiply			(&matShadowModelMVP, &matWorldModel, &mat);
	D3DXMatrixTranspose			(&m_matShadowModelMVP, &matShadowModelMVP);
	D3DXMatrixMultiply			(&matShadowFloorMVP, &matWorldFloor, &mat);
	D3DXMatrixTranspose			(&m_matShadowFloorMVP, &matShadowFloorMVP);

	// Texture adjustment matrix
	FLOAT fTexelOffs			= (.5f / SHADOW_MAP_SIZE);
	D3DXMATRIX matTexAdj	(	0.5f,      0.0f,        0.0f,        0.0f,
								0.0f,     -0.5f,        0.0f,        0.0f,
								0.0f,      0.0f,        0.0f,        0.0f,
								0.5f + fTexelOffs,	0.5f + fTexelOffs,	1.0f, 1.0f);

	D3DXMatrixMultiplyTranspose(&m_matShadowModelTex, &matShadowModelMVP, &matTexAdj);
	D3DXMatrixMultiplyTranspose(&m_matShadowFloorTex, &matShadowFloorMVP, &matTexAdj);

	// Setup lighting -- transform into model space
	D3DXVECTOR3 v;
	D3DXMatrixInverse			(&mat, NULL, &matWorldModel);
	D3DXVec3TransformNormal		(&v, &vLightDir, &mat);
	D3DXVec3Normalize			(&v, &v);
	m_vecLightDirModel			= D3DXVECTOR4(v.x, v.y, v.z, 0.0f);

	D3DXMatrixInverse			(&mat, NULL, &matWorldFloor);
	D3DXVec3TransformNormal		(&v, &vLightDir, &mat);
	D3DXVec3Normalize			(&v, &v);
	m_vecLightDirFloor			= D3DXVECTOR4(v.x, v.y, v.z, 0.0f);

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_ArcBall.HandleMouseMessages(hWnd, uMsg, wParam, lParam);

    return CD3DApplication::MsgProc(hWnd, uMsg, wParam, lParam);
}
