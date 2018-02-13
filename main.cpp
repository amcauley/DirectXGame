#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include "Engine/Util.h"
#include "Engine/Logger.h"
#include "Engine/GameMgr.h"
#include "Scenes/TestScene.h"

// Include the Direct3D Library file.
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#define S_WIDTH  960
#define S_HEIGHT 540

// Globals.
///TODO: Start moving these into appropriate classes.
IDXGISwapChain *swapchain = NULL;
ID3D11Device *dev = NULL;
ID3D11DeviceContext *devcon = NULL;
ID3D11RenderTargetView *backbuffer = NULL;

ID3D11Texture2D* pDepthStencilBuffer = NULL;
ID3D11DepthStencilState *pDepthStencilState = NULL;
ID3D11DepthStencilView *pDepthStencilView = NULL;

ID3D11RasterizerState* pRasterState = NULL;

GameMgr g_gameMgr;

// Prototypes
bool InitD3D(HWND hWnd);
bool InitGame(HINSTANCE hInstance, HWND hWnd);
bool RenderFrame(void);
bool CleanD3D(void);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// The entry point for any Windows program:
int WINAPI WinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
{
  gLogger.init();
  LOGI("Test message %d", 42);

  HWND hWnd;
  WNDCLASSEX wc;

  ZeroMemory(&wc, sizeof(WNDCLASSEX));

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = hInstance;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.lpszClassName = "WindowClass";

  RegisterClassEx(&wc);

  RECT wr = { 0, 0, S_WIDTH, S_HEIGHT };
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

  hWnd = CreateWindowEx(NULL,
    "WindowClass",
    "DirectXGame Window",
    WS_OVERLAPPEDWINDOW,
    200,
    200,
    wr.right - wr.left,
    wr.bottom - wr.top,
    NULL,
    NULL,
    hInstance,
    NULL);

  ShowWindow(hWnd, nCmdShow);

  bool bKeepRunning = true;

  // Set up and initialize Direct3D.
  if (!InitD3D(hWnd))
  {
    LOGE("InitD3D failure");
    bKeepRunning = false;
  }

  if (!InitGame(hInstance, hWnd))
  {
    LOGE("InitGame failed");
    return false;
  }

  // Enter the main loop:
  MSG msg;
  while (bKeepRunning)
  {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if (msg.message == WM_QUIT)
        break;
    }

    if (!RenderFrame())
    {
      LOGE("RenderFrame failure");
      bKeepRunning = false;
    }
  }

  // Clean up DirectX and COM.
  if (!CleanD3D())
  {
    LOGE("CleanD3D error, continuing");
  }

  gLogger.close();
  return msg.wParam;
}


// This is the main message handler for the program.
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_DESTROY:
    {
      PostQuitMessage(0);
      return 0;
    } break;
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}


// This function initializes and prepares Direct3D for use.
bool InitD3D(HWND hWnd)
{
  // Create a struct to hold information about the swap chain.
  DXGI_SWAP_CHAIN_DESC scd;

  // Clear out the struct for use.
  ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

  // Fill the swap chain description struct.
  scd.BufferCount = 1;                                   // one back buffer
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;    // use 32-bit color
  scd.BufferDesc.Width = S_WIDTH;                        // set the back buffer width
  scd.BufferDesc.Height = S_HEIGHT;                      // set the back buffer height
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;     // how swap chain is to be used
  scd.OutputWindow = hWnd;                               // the window to be used
  scd.SampleDesc.Count = 1;                              // how many multisamples (TODO: investigate why depth buffer failed when this was > 1)
  scd.Windowed = TRUE;                                   // windowed/full-screen mode
  scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

                                                         // create a device, device context and swap chain using the information in the scd struct
  if (HR_FAILED(D3D11CreateDeviceAndSwapChain(NULL,
    D3D_DRIVER_TYPE_HARDWARE,
    NULL,
    NULL,
    NULL,
    NULL,
    D3D11_SDK_VERSION,
    &scd,
    &swapchain,
    &dev,
    NULL,
    &devcon)))
  {
    return false;
  }

  // Get the address of the back buffer.
  ID3D11Texture2D *pBackBuffer;
  if (HR_FAILED(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
  {
    return false;
  }

  // Use the back buffer address to create the render target.
  if (HR_FAILED(dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer)))
  {
    return false;
  }

  RELEASE_NON_NULL(pBackBuffer);

  // Depth buffering: http://www.rastertek.com/dx11tut03.html
  // Initialize the description of the depth buffer.
  D3D11_TEXTURE2D_DESC depthBufferDesc;
  ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

  // Set up the description of the depth buffer.
  depthBufferDesc.Width = S_WIDTH;
  depthBufferDesc.Height = S_HEIGHT;
  depthBufferDesc.MipLevels = 1;
  depthBufferDesc.ArraySize = 1;
  depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthBufferDesc.SampleDesc.Count = 1;
  depthBufferDesc.SampleDesc.Quality = 0;
  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthBufferDesc.CPUAccessFlags = 0;
  depthBufferDesc.MiscFlags = 0;

  // Create the texture for the depth buffer using the filled out description.
  if (HR_FAILED(dev->CreateTexture2D(&depthBufferDesc, NULL, &pDepthStencilBuffer)))
  {
    return false;
  }

  // Initialize the description of the stencil state.
  D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
  ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

  // Set up the description of the stencil state.
  depthStencilDesc.DepthEnable = true;
  depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

  depthStencilDesc.StencilEnable = true;
  depthStencilDesc.StencilReadMask = 0xFF;
  depthStencilDesc.StencilWriteMask = 0xFF;

  // Stencil operations if pixel is front-facing.
  depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Stencil operations if pixel is back-facing.
  depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  // Create the depth stencil state.
  if(HR_FAILED(dev->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState)))
  {
    return false;
  }

  // Set the depth stencil state.
  devcon->OMSetDepthStencilState(pDepthStencilState, 1);

  // Initailze the depth stencil view.
  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

  // Set up the depth stencil view description.
  depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Texture2D.MipSlice = 0;

  // Create the depth stencil view.
  if(HR_FAILED(dev->CreateDepthStencilView(pDepthStencilBuffer, &depthStencilViewDesc, &pDepthStencilView)))
  {
    return false;
  }

  // Bind the render target view and depth stencil buffer to the output render pipeline.
  devcon->OMSetRenderTargets(1, &backbuffer, pDepthStencilView);

  // Setup the raster description which will determine how and what polygons will be drawn.
  D3D11_RASTERIZER_DESC rasterDesc;
  rasterDesc.AntialiasedLineEnable = false;
  rasterDesc.CullMode = D3D11_CULL_BACK;
  rasterDesc.DepthBias = 0;
  rasterDesc.DepthBiasClamp = 0.0f;
  rasterDesc.DepthClipEnable = true;
  rasterDesc.FillMode = D3D11_FILL_SOLID;
  rasterDesc.FrontCounterClockwise = false;
  rasterDesc.MultisampleEnable = false;
  rasterDesc.ScissorEnable = false;
  rasterDesc.SlopeScaledDepthBias = 0.0f;

  // Create the rasterizer state from the description we just filled out.
  if(HR_FAILED(dev->CreateRasterizerState(&rasterDesc, &pRasterState)))
  {
    return false;
  }

  // Now set the rasterizer state.
  devcon->RSSetState(pRasterState);

  // Set the viewport.
  D3D11_VIEWPORT viewport;
  ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

  viewport.TopLeftX = 0.0;
  viewport.TopLeftY = 0.0;
  viewport.MinDepth = 0.0;
  viewport.MaxDepth = 1.0;
  viewport.Width = S_WIDTH;
  viewport.Height = S_HEIGHT;

  devcon->RSSetViewports(1, &viewport);

  return true;
}


bool InitGame(HINSTANCE hInstance, HWND hWnd)
{
  TestScene *pStartingScene = new TestScene();
  pStartingScene->init(dev, devcon);
  g_gameMgr.init(hInstance, hWnd, dev, devcon, pStartingScene, S_WIDTH, S_HEIGHT);

  return true;
}


bool RenderFrame(void)
{
  // Background color.
  devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.5f, 0.5f, 1.0f));

  // Clear the depth buffer.
  devcon->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

  if (!g_gameMgr.update(dev, devcon))
  {
    LOGE("Game Mgr update failed");
    return false;
  }

  // Swap back buffer and displayed buffer.
  if (HR_FAILED(swapchain->Present(0, 0)))
  {
    return false;
  }

  return true;
}


bool CleanD3D(void)
{
  // Go into windowed mode if not already there.
  if (HR_FAILED(swapchain->SetFullscreenState(FALSE, NULL)))
  {
    return false;
  }

  g_gameMgr.release();

  RELEASE_NON_NULL(pRasterState);
  RELEASE_NON_NULL(pDepthStencilState);
  RELEASE_NON_NULL(pDepthStencilView);
  RELEASE_NON_NULL(pDepthStencilBuffer);
  RELEASE_NON_NULL(swapchain);
  RELEASE_NON_NULL(backbuffer);
  RELEASE_NON_NULL(dev);
  RELEASE_NON_NULL(devcon);

  return true;
}
