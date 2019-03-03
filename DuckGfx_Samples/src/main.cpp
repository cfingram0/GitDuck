
#include "windows.h"
#include "DuckGfx.h"



#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR * szTitle = L"DuckGfx Samples";                // The title bar text
WCHAR * szWindowClass = L"Window";            // the main window class name
bool running = true;
                                                // Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND *);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
  if (!InitInstance(hInstance, nCmdShow, &windowHandle))
  {
    DWORD err = GetLastError();

    return FALSE;
  }

  if (!duckGfx::Init(windowHandle)) {
    return FALSE;
  }


  //single threaded render loop
  MSG msg;
  while (running) {
    //input
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    //logic

    //graphics
    duckGfx::Render();
  }


  duckGfx::ShutDown();
  return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEXW wcex;


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

  return RegisterClassExW(&wcex);
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

  HWND hWnd = CreateWindowW(szWindowClass, 
                            szTitle, 
                            WS_OVERLAPPEDWINDOW, 
                            CW_USEDEFAULT, 
                            0, 
                            1600, // width
                            900,  // height
                            nullptr, 
                            nullptr, 
                            hInstance, 
                            nullptr);

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
