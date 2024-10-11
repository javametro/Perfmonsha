// MomoClient.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MomoClient.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <propvarutil.h>
#include <string>

// Add the following pragmas to link with the required libraries
#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "propsys.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND g_hwndVideo = NULL;                        // Video window handle

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void                PlayVideo(HWND hWnd, const std::wstring& source);

// Button IDs
#define IDC_BUTTON_START 1001
#define IDC_BUTTON_STOP 1002
#define IDC_BUTTON_PLAYVIDEO 1003

// Video playback variables
IMFMediaSource* pSource = NULL;
IMFMediaSession* pSession = NULL;
IMFTopology* pTopology = NULL;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize COM
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MOMOCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOMOCLIENT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Uninitialize COM
    CoUninitialize();

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOMOCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MOMOCLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 800, 600, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Create Start button
   CreateWindow(L"BUTTON", L"Start", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
      10, 10, 100, 30, hWnd, (HMENU)IDC_BUTTON_START, hInstance, nullptr);

   // Create Stop button
   CreateWindow(L"BUTTON", L"Stop", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
      120, 10, 100, 30, hWnd, (HMENU)IDC_BUTTON_STOP, hInstance, nullptr);

   // Create PlayVideo button
   CreateWindow(L"BUTTON", L"PlayVideo", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
      230, 10, 100, 30, hWnd, (HMENU)IDC_BUTTON_PLAYVIDEO, hInstance, nullptr);

   // Create video window
   g_hwndVideo = CreateWindowEx(0, L"STATIC", NULL, 
      WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
      10, 50, 780, 500, hWnd, NULL, hInstance, NULL);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
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
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDC_BUTTON_START:
                MessageBox(hWnd, L"Started!", L"Start Button", MB_OK);
                break;
            case IDC_BUTTON_STOP:
                if (pSession)
                {
                    pSession->Stop();
                }
                MessageBox(hWnd, L"Stopped!", L"Stop Button", MB_OK);
                break;
            case IDC_BUTTON_PLAYVIDEO:
                {
                    // Example: Play a local MP4 file
                    std::wstring mp4File = L"C:\\Users\\stephen\\Desktop\\test\\counting.mp4";
                    
                    // Check if the file exists before trying to play it
                    DWORD fileAttributes = GetFileAttributes(mp4File.c_str());
                    if (fileAttributes != INVALID_FILE_ATTRIBUTES && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                    {
                        PlayVideo(hWnd, mp4File);
                    }
                    else
                    {
                        MessageBox(hWnd, L"The specified video file does not exist.", L"File Not Found", MB_OK);
                    }
                }
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        if (pSession)
        {
            pSession->Shutdown();
            pSession->Release();
        }
        if (pSource)
        {
            pSource->Shutdown();
            pSource->Release();
        }
        if (pTopology)
        {
            pTopology->Release();
        }
        MFShutdown();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void PlayVideo(HWND hWnd, const std::wstring& source)
{
    HRESULT hr = S_OK;

    // Initialize Media Foundation
    hr = MFStartup(MF_VERSION);
    if (FAILED(hr))
    {
        MessageBox(hWnd, L"Failed to initialize Media Foundation", L"Error", MB_OK);
        return;
    }

    // Create the source resolver
    IMFSourceResolver* pSourceResolver = NULL;
    hr = MFCreateSourceResolver(&pSourceResolver);
    if (FAILED(hr))
    {
        MessageBox(hWnd, L"Failed to create source resolver", L"Error", MB_OK);
        return;
    }

    // Create the media source
    MF_OBJECT_TYPE objectType = MF_OBJECT_INVALID;
    IUnknown* pSourceUnk = NULL;
    hr = pSourceResolver->CreateObjectFromURL(
        source.c_str(),
        MF_RESOLUTION_MEDIASOURCE,
        NULL,
        &objectType,
        &pSourceUnk
    );
    pSourceResolver->Release();
    if (FAILED(hr))
    {
        MessageBox(hWnd, L"Failed to create media source", L"Error", MB_OK);
        return;
    }

    hr = pSourceUnk->QueryInterface(IID_PPV_ARGS(&pSource));
    pSourceUnk->Release();
    if (FAILED(hr))
    {
        MessageBox(hWnd, L"Failed to get media source interface", L"Error", MB_OK);
        return;
    }

    // Create the media session
    hr = MFCreateMediaSession(NULL, &pSession);
    if (FAILED(hr))
    {
        MessageBox(hWnd, L"Failed to create media session", L"Error", MB_OK);
        return;
    }

    // Create the topology
    hr = MFCreateTopology(&pTopology);
    if (FAILED(hr))
    {
        MessageBox(hWnd, L"Failed to create topology", L"Error", MB_OK);
        return;
    }

    // Create the presentation descriptor
    IMFPresentationDescriptor* pPD = NULL;
    hr = pSource->CreatePresentationDescriptor(&pPD);
    if (FAILED(hr))
    {
        MessageBox(hWnd, L"Failed to create presentation descriptor", L"Error", MB_OK);
        return;
    }

    // Get the number of streams
    DWORD cStreams = 0;
    hr = pPD->GetStreamDescriptorCount(&cStreams);
    if (FAILED(hr))
    {
        pPD->Release();
        MessageBox(hWnd, L"Failed to get stream count", L"Error", MB_OK);
        return;
    }

    // For each stream, create the topology nodes and add them to the topology
    for (DWORD i = 0; i < cStreams; i++)
    {
        IMFStreamDescriptor* pSD = NULL;
        BOOL fSelected = FALSE;
        hr = pPD->GetStreamDescriptorByIndex(i, &fSelected, &pSD);
        if (FAILED(hr))
        {
            pPD->Release();
            MessageBox(hWnd, L"Failed to get stream descriptor", L"Error", MB_OK);
            return;
        }

        if (fSelected)
        {
            IMFTopologyNode* pSourceNode = NULL;
            hr = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pSourceNode);
            if (FAILED(hr))
            {
                pSD->Release();
                pPD->Release();
                MessageBox(hWnd, L"Failed to create source node", L"Error", MB_OK);
                return;
            }

            hr = pSourceNode->SetUnknown(MF_TOPONODE_SOURCE, pSource);
            hr = pSourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPD);
            hr = pSourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pSD);

            IMFTopologyNode* pOutputNode = NULL;
            hr = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pOutputNode);
            if (FAILED(hr))
            {
                pSourceNode->Release();
                pSD->Release();
                pPD->Release();
                MessageBox(hWnd, L"Failed to create output node", L"Error", MB_OK);
                return;
            }

            // Create the video renderer
            IMFActivate* pRendererActivate = NULL;
            hr = MFCreateVideoRendererActivate(g_hwndVideo, &pRendererActivate);
            if (FAILED(hr))
            {
                MessageBox(hWnd, L"Failed to create video renderer activate", L"Error", MB_OK);
                return;
            }

            hr = pOutputNode->SetObject(pRendererActivate);
            pRendererActivate->Release();

            hr = pTopology->AddNode(pSourceNode);
            hr = pTopology->AddNode(pOutputNode);

            hr = pSourceNode->ConnectOutput(0, pOutputNode, 0);

            pSourceNode->Release();
            pOutputNode->Release();
        }

        pSD->Release();
    }

    pPD->Release();

    // Set the topology on the media session
    hr = pSession->SetTopology(0, pTopology);
    if (FAILED(hr))
    {
        MessageBox(hWnd, L"Failed to set topology", L"Error", MB_OK);
        return;
    }

    // Start playback
    PROPVARIANT varStart;
    PropVariantInit(&varStart);
    hr = pSession->Start(&GUID_NULL, &varStart);
    if (FAILED(hr))
    {
        MessageBox(hWnd, L"Failed to start playback", L"Error", MB_OK);
        return;
    }

    PropVariantClear(&varStart);
}