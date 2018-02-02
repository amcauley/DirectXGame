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

// Globals. TODO: Start moving these into appropriate classes.
IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;
ID3D11RenderTargetView *backbuffer;

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
  scd.SampleDesc.Count = 4;                              // how many multisamples
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

  pBackBuffer->Release();

  // Set the render target as the back buffer.
  devcon->OMSetRenderTargets(1, &backbuffer, NULL);

  // Set the viewport.
  D3D11_VIEWPORT viewport;
  ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
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

  RELEASE_NON_NULL(swapchain);
  RELEASE_NON_NULL(backbuffer);
  RELEASE_NON_NULL(dev);
  RELEASE_NON_NULL(devcon);

  return true;
}
