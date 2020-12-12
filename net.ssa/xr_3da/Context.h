//-----------------------------------------------------------------------------
// File: D3DApp.h
//
// Desc: Application class for the Direct3D samples framework library.
//
// Copyright (c) 1998-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef D3DAPP_H
#define D3DAPP_H

//-----------------------------------------------------------------------------
// Error codes
//-----------------------------------------------------------------------------
enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOREF };

#define D3DAPPERR_NODIRECT3D          0x82000001
#define D3DAPPERR_NOWINDOW            0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE    0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE    0x82000006
#define D3DAPPERR_NOWINDOWEDHAL       0x82000007
#define D3DAPPERR_NODESKTOPHAL        0x82000008
#define D3DAPPERR_NOHALTHISMODE       0x82000009
#define D3DAPPERR_NONZEROREFCOUNT     0x8200000a
#define D3DAPPERR_MEDIANOTFOUND       0x8200000b
#define D3DAPPERR_RESIZEFAILED        0x8200000c

//-----------------------------------------------------------------------------
// Name: struct D3DModeInfo
// Desc: Structure for holding information about a display mode
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Name: struct D3DDeviceInfo
// Desc: Structure for holding information about a Direct3D device, including
//       a list of modes compatible with this device
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Name: struct D3DAdapterInfo
// Desc: Structure for holding information about an adapter, including a list
//       of devices available on this adapter
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// Name: class Context
// Desc: A base class for creating D3D8 applications. To create a simple
//       Direct3D application, simply derive this class into a class (such as
//       class CMyD3DApplication) and override the following functions, as 
//       needed:
//          OneTimeSceneInit()    - To initialize app data (alloc mem, etc.)
//          InitDeviceObjects()   - To initialize the 3D scene objects
//          FrameMove()           - To animate the scene
//          Render()              - To render the scene
//          DeleteDeviceObjects() - To cleanup the 3D scene objects
//          FinalCleanup()        - To cleanup app data (for exitting the app)
//          MsgProc()             - To handle Windows messages
//-----------------------------------------------------------------------------
class CContext
{
	friend class	CRenderDevice;
protected:
    // Internal error handling function
    HRESULT DisplayErrorMsg( HRESULT hr, DWORD dwType );

    // Internal functions to manage and render the 3D scene
    HRESULT Initialize3DEnvironment	();
    HRESULT Resize3DEnvironment		();
    HRESULT ToggleFullscreen		();
    HRESULT ForceWindowed			();
    HRESULT UserSelectNewDevice		();
    VOID    Cleanup3DEnvironment	();
    HRESULT Render3DEnvironment		();
    virtual HRESULT AdjustWindowForChange();

protected:
    // Main objects used for creating and rendering the 3D scene
    HWND					m_hWnd;				// The main app window
    HWND					m_hWndFocus;		// The D3D focus window (usually same as m_hWnd)
    LPDIRECT3D8				m_pD3D;				// The main D3D object
    LPDIRECT3DDEVICE8		m_pd3dDevice;		// The D3D rendering device
    D3DCAPS8				m_d3dCaps;			// Caps for the device
    D3DSURFACE_DESC			m_d3dsdBackBuffer;  // Surface desc of the backbuffer
    DWORD					m_dwCreateFlags;	// Indicate sw or hw vertex processing
    DWORD					m_dwWindowStyle;	// Saved window style for mode switches
    RECT					m_rcWindowBounds;	// Saved window bounds for mode switches
    RECT					m_rcWindowClient;	// Saved client area size for mode switches
public:
    // Functions to create, run, pause, and clean up the application
    virtual HRESULT			Create	(HINSTANCE hInstance );
    virtual INT				Run		();
    virtual LRESULT			MsgProc	( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual VOID			Pause	( BOOL bPause );

    // Internal constructor
    CContext				();
};




#endif



