//--------------------------------------------------------------------------------------
// File: EmptyProject11.cpp
//
// Empty starting point for new Direct3D 11 Win32 desktop applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include <Core/DXUT.h>
#include <Optional/DXUTgui.h>
#include <Optional/DXUTcamera.h>
#include <Optional/DXUTsettingsdlg.h>
#include <Optional/SDKmisc.h>
#include "RenderObjMesh.h"
#include "RenderObjShader.h"
#include "RenderTestMesh.h"
#include "RenderFrameShader.h"
#include <MeshLoadObj.h>
#pragma warning( disable : 4100 )

using namespace DirectX;
CDXUTDialogResourceManager			g_DialogResourceManager;
CModelViewerCamera                  g_Camera;
CDXUTDialog							g_HUD;
CD3DSettingsDlg                     g_D3DSettingsDlg;

RenderObjMesh						g_Mesh;
RenderObjShader						g_Sharder;
RenderFrameShader					g_FrameSharder;
MeshLoadObj							g_Load;

RenderTestMesh						g_Test;


void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{
	HRESULT hr = S_OK;

	static bool bFirstOnCreateDevice = true;

	// Warn the user that in order to support CS4x, a non-hardware device has been created, continue or quit?
	if (DXUTGetDeviceSettings().d3d11.DriverType != D3D_DRIVER_TYPE_HARDWARE && bFirstOnCreateDevice)
	{
		if (MessageBox(0, L"CS4x capability is missing. "\
			L"In order to continue, a non-hardware device has been created, "\
			L"it will be very slow, continue?", L"Warning", MB_ICONEXCLAMATION | MB_YESNO) != IDYES)
			return E_FAIL;
	}

	auto pd3dImmediateContext = DXUTGetD3D11DeviceContext();
	V_RETURN(g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));
	V_RETURN(g_D3DSettingsDlg.OnD3D11CreateDevice(pd3dDevice));

// 	const float                         g_fSpread = 400.0f;
// 	XMVECTOR vecEye = XMVectorSet(-g_fSpread * 2, g_fSpread * 4, -g_fSpread * 3, 0.f);
// 	g_Camera.SetViewParams(vecEye, g_XMZero);
	

	g_Sharder.Create(pd3dDevice, L"Media/Shader/ObjShader.fx");
	g_FrameSharder.Create(pd3dDevice, L"Media/Shader/FrameShader.fx");
	g_Load.Load("Media/Obj/Game2.unity_358.obj");
	g_Mesh.SetRenderObjShader(&g_Sharder);
	g_Mesh.Create(pd3dDevice, &g_Load);
	g_Test.SetRenderObjShader(&g_Sharder);
	g_Test.Create(pd3dDevice);

	static const XMVECTORF32 s_vEyeStart = {g_Load.Max().x, g_Load.Max().y , g_Load.Max().z } ;
	//static const XMVECTORF32 s_vEyeStart = { 0.0f, 1.0f, -5.0f, 0.0f };
	//static const XMVECTORF32 s_vAtStart = { 0.f, 0.f, 0.f, 0.f };
	static const XMVECTORF32 s_vAtStart = { g_Load.Center().x, g_Load.Center().y, g_Load.Center().z, 0.f };
	g_Camera.SetViewParams(s_vEyeStart, s_vAtStart);


    return hr;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	HRESULT hr;
	V_RETURN(g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
	V_RETURN(g_D3DSettingsDlg.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	// Setup the camera's projection parameters
	float fAspectRatio = pBackBufferSurfaceDesc->Width / (FLOAT)pBackBufferSurfaceDesc->Height;
	g_Camera.SetProjParams(XM_PI / 4, fAspectRatio, 10.0f, 500000.0f);
	g_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	g_Camera.SetButtonMasks(0, MOUSE_WHEEL, MOUSE_LEFT_BUTTON | MOUSE_MIDDLE_BUTTON | MOUSE_RIGHT_BUTTON);

	g_HUD.SetLocation(pBackBufferSurfaceDesc->Width - 170, 0);
	g_HUD.SetSize(170, 170);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	g_Camera.FrameMove(fElapsedTime);
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
                                  double fTime, float fElapsedTime, void* pUserContext )
{
	if (g_D3DSettingsDlg.IsActive())
	{
		g_D3DSettingsDlg.OnRender(fElapsedTime);
		return;
	}
    // Clear render target and the depth stencil 
    auto pRTV = DXUTGetD3D11RenderTargetView();
    pd3dImmediateContext->ClearRenderTargetView( pRTV, Colors::MidnightBlue );

    auto pDSV = DXUTGetD3D11DepthStencilView();
    pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

	// Get the projection & view matrix from the camera class
	XMMATRIX mProj = g_Camera.GetProjMatrix();
	XMMATRIX mView = g_Camera.GetViewMatrix();

	//
	g_Mesh.SetRenderObjShader(&g_Sharder);
	g_Mesh.OnRender(pd3dImmediateContext, &g_Camera);
	g_Test.SetRenderObjShader(&g_Sharder);
	g_Test.OnRender(pd3dImmediateContext, &g_Camera);
	//
	g_Mesh.SetRenderObjShader(&g_FrameSharder);
	g_Mesh.OnRender(pd3dImmediateContext, &g_Camera);
	g_Test.SetRenderObjShader(&g_FrameSharder);
	g_Test.OnRender(pd3dImmediateContext, &g_Camera);

	// Render the particles
	DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"HUD / Stats");
	g_HUD.OnRender(fElapsedTime);
	DXUT_EndPerfEvent();

}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
	g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
	g_DialogResourceManager.OnD3D11DestroyDevice();
	g_D3DSettingsDlg.OnD3D11DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();
	g_Mesh.Destory();
	g_Test.Destory();
	g_Sharder.Destory();
	g_FrameSharder.Destory();
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	// Pass messages to settings dialog if its active
	if (g_D3DSettingsDlg.IsActive())
	{
		g_D3DSettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}

	// Give the dialogs a chance to handle the message first
	*pbNoFurtherProcessing = g_HUD.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;
	// Pass all windows messages to camera so it can respond to user input
	g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Handle mouse button presses
//--------------------------------------------------------------------------------------
void CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                       bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                       int xPos, int yPos, void* pUserContext )
{
}


//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK OnDeviceRemoved( void* pUserContext )
{
    return true;
}
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4

void InitApp()
{
	g_D3DSettingsDlg.Init(&g_DialogResourceManager);
	g_HUD.Init(&g_DialogResourceManager);

	g_HUD.SetCallback(OnGUIEvent); int iY = 10;
	g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 23);
	g_HUD.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += 26, 170, 23, VK_F3);
	g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += 26, 170, 23, VK_F2);

}

void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	switch (nControlID)
	{
	case IDC_TOGGLEFULLSCREEN:
		DXUTToggleFullScreen(); 
		break;
	case IDC_TOGGLEREF:
		DXUTToggleREF(); 
		break;
	case IDC_CHANGEDEVICE:
		g_D3DSettingsDlg.SetActive(!g_D3DSettingsDlg.IsActive()); 
		break;
	}
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#ifdef _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device
    // that is available on the system depending on which D3D callbacks are set below

    // Set general DXUT callbacks
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackMouse( OnMouse );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackDeviceRemoved( OnDeviceRemoved );

    // Set the D3D11 DXUT callbacks. Remove these sets if the app doesn't need to support D3D11
    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );


	InitApp();

    // Perform any application-level initialization here

    DXUTInit( true, true, nullptr ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"EmptyProject11" );

    // Only require 10-level hardware or later
    DXUTCreateDevice( D3D_FEATURE_LEVEL_10_0, true, 800, 600 );
    DXUTMainLoop(); // Enter into the DXUT ren  der loop

    // Perform any application-level cleanup here

    return DXUTGetExitCode();
}


