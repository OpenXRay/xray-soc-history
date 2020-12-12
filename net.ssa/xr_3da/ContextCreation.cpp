#include "stdafx.h"

extern DWORD psSelectedDevice;
HRESULT _CreateContext2(DWORD dwBPP, BOOL bFS)
{

                // Confirm the device/format for HW vertex processing
                if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT )
                {
                    if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_PUREDEVICE )
                    {
                        dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING |
                                        D3DCREATE_PUREDEVICE;

                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                      formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }

                    if ( FALSE == bFormatConfirmed[f] )
                    {
                        dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING;

                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                      formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }

                    if ( FALSE == bFormatConfirmed[f] )
                    {
                        dwBehavior[f] = D3DCREATE_MIXED_VERTEXPROCESSING;

                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                      formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }
                }

                // Confirm the device/format for SW vertex processing
                if( FALSE == bFormatConfirmed[f] )
                {
                    dwBehavior[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

                    if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                  formats[f] ) ) )
                        bFormatConfirmed[f] = TRUE;
                }

	DWORD dwFlags			= (bFS ? D3DX_CONTEXT_FULLSCREEN:0);
	DWORD dwColorBits		= (bFS ? dwBPP : D3DX_DEFAULT);
	DWORD dwDepthBits		= D3DX_DEFAULT;
	DWORD dwStencilBits		= ((psDeviceFlags&rsShadows)?D3DX_DEFAULT:0);
	DWORD dwNumBB			= ((psDeviceFlags&rsTriplebuffer)?2:1);
	if (!bFS)			{
		// Windowed mode

		// No TripleBuffering
		dwNumBB=1;
		psDeviceFlags &= ~rsTriplebuffer;

		// Colorbits are equal to desktop depth
		// ??
	}
	if (Device.Caps.bForceAltStencil) {
		// Alternate ZBuffer & Stencil creation method
		// Instruct D3DX not to create it for us
		dwDepthBits = dwStencilBits = 0;
	}

    HRESULT hr =  D3DXCreateContextEx(
								psSelectedDevice,	// D3DX handle
                                dwFlags,			// flags
                                Device.m_hwnd,
                                NULL,				// focusWnd
                                dwColorBits,		// colorbits
                                D3DX_DEFAULT,		// alphabits
                                dwDepthBits,		// numdepthbits
                                dwStencilBits,		// numstencilbits
                                dwNumBB,			// numbackbuffers
                                Device.dwWidth,		// width
                                Device.dwHeight,	// height
                                D3DX_DEFAULT,		// refresh rate
                                &HW.pContext		// returned D3DX interface
                       );
	if (FAILED(hr)) return hr;

	// Capture all interfaces
	VERIFY(HW.pContext);
	HW.Capture();
	HW.Validate();

	// If alt. zbuf creation alg. used - create & attach it.
	// After that - rebuild internal D3D structures
	if (Device.Caps.bForceAltStencil) {
		hr = _CreateStencil();
		if (FAILED(hr)) {
			HW.Release();
			return hr;
		}
	}

	// Query support for all caps
	Device.Caps.Update();

	return hr;
}

void CRenderDevice::CreateContext(void)
{
    // ******************************* D3DX Initialization
	// Normal
	HRESULT hr = _CreateContext2(dwBPP, bFS );
	if (!FAILED(hr)) goto _DeviceOK;

	// Try 16 bit
	if ((dwBPP==32)||(dwBPP==24)) {
		hr = _CreateContext2(16, bFS );
		if (!FAILED(hr)) {
			Log("! Error: Failed to initialize 24/32 bit display mode.\n! Rolling back to 16 bit.");
			psCurrentBPP=dwBPP=16;
			goto _DeviceOK;
		}
	}

	// Try Windowed
	if (bFS) {
		hr = _CreateContext2(dwBPP, FALSE );
		if (!FAILED(hr)) {
			Log("! Error: Failed to initialize fullscreen mode.\n! Rolling back to windowed.");
			bFS = FALSE;
			psDeviceFlags&=~rsFullscreen;
			goto _DeviceOK;
		}
	}
	// Try Fullscreen
	if (!bFS) {
		hr = _CreateContext2(dwBPP, TRUE );
		if (!FAILED(hr)) {
			Log("! Error: Failed to initialize windowed mode.\n! Switching to fullscreen.");
			bFS = TRUE;
			psDeviceFlags|=rsFullscreen;
			goto _DeviceOK;
		}
	}
	// Initialization failed :(
	InterpretError(hr,__FILE__,__LINE__);

_DeviceOK:
	if ((psDeviceFlags&rsShadows)&& !Caps.bStencil) {
		Log("! Error: Engine request for stencil buffer ignored by hardware");
		Log("! Shadows disabled. Try switching to 32bit video mode");
		Log("! Another work-around: try start engine with '-alt_stencil' parameter");
		Log("! Recommendation: restart the video subsystem");
		psDeviceFlags &= ~rsShadows;
	}
}

void CRenderDevice::DestroyContext()
{
	if (Caps.bForceAltStencil) {
		_RELEASE(pZBuffer);
	}
	HW.Release();
}

// ddsdZB.ddpfPixelFormat.dwStencilBitDepth
