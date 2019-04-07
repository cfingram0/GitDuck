#include <vector>
#include "windows.h"
#include "DuckGfx.h"

#include "sample_framework.h"
#include "sample_test.h"
#include "cube_test.h"
#include "lighting_test.h"
#include "debug_draw_test.h"
#include "model_load_test.h"

#include "input.h"
#include "assimp\version.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
char * szTitle = "DuckGfx Samples";                // The title bar text
char * szWindowClass = "Window";            // the main window class name
bool running = true;
                                                // Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND *);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


std::vector<ISample*> samples;
ISample* pSample = nullptr;
int32_t currentSample = -1;
int32_t desiredSample = -1;



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPWSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  
  if (!MyRegisterClass(hInstance)) {
    DWORD err = GetLastError();
    return FALSE;
  }

  // Perform application initialization:
  HWND windowHandle;
  if (!InitInstance(hInstance, nCmdShow, &windowHandle)) {
    DWORD err = GetLastError();

    return FALSE;
  }

  if (!duckGfx::Init(windowHandle, 1600, 900)) {
    return FALSE;
  }

  uint32_t version = aiGetVersionMajor();

  samples.push_back(new Sample_Test());
  samples.push_back(new Cube_Test());
  samples.push_back(new Lighting_Test());
  samples.push_back(new DebugDraw_Test());
  samples.push_back(new ModelLoad_Test());
  desiredSample = 4;

  //single threaded render loop
  MSG msg;
  while (running) {
    if (desiredSample != currentSample) {
      if (pSample) {
        pSample->OnEnd();
        pSample->Shutdown();
      }

      currentSample = desiredSample;
      pSample = samples[currentSample];
      pSample->Init();
      pSample->OnStart();
    }

    input::Update();

    //input
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    //logic
    if (pSample)
      pSample->Update(1.0f / 60.0f);

    if (input::IsTriggered(Key::NUM_1)) {
      desiredSample = 0;
    }
    if (input::IsTriggered(Key::NUM_2)) {
      desiredSample = 1;
    }
    if (input::IsTriggered(Key::NUM_3)) {
      desiredSample = 2;
    }
    if (input::IsTriggered(Key::NUM_4)) {
      desiredSample = 3;
    }
    if (input::IsTriggered(Key::NUM_5)) {
      desiredSample = 4;
    }
    if (input::IsTriggered(Key::ESCAPE)) {
      running = false;
    }


    //graphics
    duckGfx::Render();
  }

  // shut down
  if (pSample) {
    pSample->OnEnd();
    pSample->Shutdown();
  }

  duckGfx::Shutdown();
  return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;


  ZeroMemory(&wcex, sizeof(WNDCLASSEX));

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = NULL;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszClassName = szWindowClass;

  return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND * outWnd)
{
  hInst = hInstance; // Store instance handle in our global variable

  HWND hWnd = CreateWindow(szWindowClass, 
                            szTitle, 
                            WS_OVERLAPPEDWINDOW, 
                            CW_USEDEFAULT, 
                            0, 
                            1616, // width
                            939,  // height
                            nullptr, 
                            nullptr, 
                            hInstance, 
                            nullptr);

  RECT client;
  GetClientRect(hWnd, &client);

  RECT window;
  GetWindowRect(hWnd, &window);

  if (!hWnd)
  {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  *outWnd = hWnd;

  return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  break;
  case WM_KEYDOWN:
    input::HandleKeyDownMsg(wParam);
    break;

  case WM_KEYUP:
    input::HandleKeyUpMsg(wParam);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    running = false;
    break;
  
  case WM_CLOSE:
    PostQuitMessage(0);
    running = false;
    break;

  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
