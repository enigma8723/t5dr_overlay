// This file contains the 'main' function.Program execution begins and ends there.

#include <iostream>
#include <stdint.h>
#include <windows.h>
#include <chrono>
#include <thread>
#include <mutex>

#include "Types.h"
#include "T5DR_Overlay.h"

const wchar_t* g_szClassName = L"myWindowClass";

T5DROverlay t5DROverlay;
OverlayData p1OverlayData{ 0 }, p2OverlayData{ 0 }, p1LastOverlayData{ 0 }, p2LastOverlayData{ 0 };
std::thread* subThread;

std::mutex mu;

bool OverlayNeedsUpdate(OverlayData overlayData, OverlayData lastOverlayData) {
    return !(
        overlayData.currentMoveId == lastOverlayData.currentMoveId
        && overlayData.firstActiveFrame == lastOverlayData.firstActiveFrame
        && overlayData.lastActiveFrame == lastOverlayData.lastActiveFrame
        && overlayData.frameAdvantage == lastOverlayData.frameAdvantage
        && overlayData.animLength == lastOverlayData.animLength
        && overlayData.currentMoveConnects == lastOverlayData.currentMoveConnects
    );
}

void PrepareOverlay() {
    t5DROverlay.AttachToProcess();
    t5DROverlay.QueryMovelists();
    t5DROverlay.CreateMovelistMap();
    t5DROverlay.QueryCancels();
    t5DROverlay.CreateCancelsMap();
    t5DROverlay.QueryExtraProperties();
    t5DROverlay.CreateExtraPropertiesMap();
}

void FetchOverlayData(OverlayData& p1OverlayData, OverlayData& p2OverlayData) {

    t5DROverlay.QueryCurrentMoveInfo();
    t5DROverlay.FetchOverlayData(p1OverlayData, p2OverlayData);
    //t5DROverlay.SetFirstRunFalse();

}


void UpdateOverlay(HWND hwnd, OverlayData* p1OverlayData, OverlayData* p1LastOverlayData, OverlayData* p2OverlayData, OverlayData* p2LastOverlayData) {

    //auto start = std::chrono::steady_clock::now();

    while (true) {
        mu.lock();
        FetchOverlayData(*p1OverlayData, *p2OverlayData);
        mu.unlock();

        // Prevents window flickering.
        if (OverlayNeedsUpdate(*p1OverlayData, *p1LastOverlayData) || OverlayNeedsUpdate(*p2OverlayData, *p2LastOverlayData)) {
            InvalidateRect(hwnd, NULL, TRUE);
            //UpdateWindow(hwnd);
        }
    }

    /*auto end = std::chrono::steady_clock::now();

    auto diffNs = end - start;
    auto diffMs = std::chrono::duration<double, std::milli>(diffNs).count();
    std::cout << "Duration : " << diffMs << " ms\n";*/

}


// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;

    string overlayString;
    std::wstring widestr;
    string p1PlusSymbol = "";
    string p2PlusSymbol = "";

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);

        
        if (p1OverlayData.frameAdvantage > 0) {
            p1PlusSymbol = "+";
        }

        if (p2OverlayData.frameAdvantage > 0) {
            p2PlusSymbol = "+";
        }

        mu.lock();
        overlayString = "p1 move id: " + std::to_string(p1OverlayData.currentMoveId) + "\n";
        overlayString += "p1 active frames: " + std::to_string(p1OverlayData.firstActiveFrame) + " - " + std::to_string(p1OverlayData.lastActiveFrame) + "\n";
        overlayString += "p1 frame advantage: " + p1PlusSymbol + std::to_string(p1OverlayData.frameAdvantage) + "\n";
        overlayString += "p1 move anim length: " + std::to_string(p1OverlayData.animLength) + "\n";
        overlayString += "p1 move connects?: " + std::to_string(p1OverlayData.currentMoveConnects) + "\n\n\n";
        overlayString += "p2 move id: " + std::to_string(p2OverlayData.currentMoveId) + "\n";
        overlayString += "p2 active frames: " + std::to_string(p2OverlayData.firstActiveFrame) + " - " + std::to_string(p2OverlayData.lastActiveFrame) + "\n";
        overlayString += "p2 frame advantage: " + p2PlusSymbol + std::to_string(p2OverlayData.frameAdvantage) + "\n";
        overlayString += "p2 move anim length: " + std::to_string(p2OverlayData.animLength) + "\n";
        overlayString += "p2 move connects?: " + std::to_string(p2OverlayData.currentMoveConnects) + "\n";
        widestr = std::wstring(overlayString.begin(), overlayString.end());

        p1LastOverlayData = p1OverlayData;
        p2LastOverlayData = p2OverlayData;
        mu.unlock();

        DrawText(hdc, widestr.c_str(), -1, &rect, DT_EDITCONTROL | DT_NOCLIP | DT_CENTER | DT_VCENTER);
        EndPaint(hwnd, &ps);
        return 0;

    case WM_DESTROY:
        subThread->detach();
        subThread->~thread();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;


    PrepareOverlay();

    //Step 1: Registering the Window Class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+3);
    wc.hbrBackground = CreateSolidBrush(RGB(0, 128, 128));
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        L"The title of my window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 222, 249,
        NULL, NULL, hInstance, NULL);
    if (hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);


    // Constructs the new thread and runs it. Does not block execution.
    std::thread sub(UpdateOverlay, hwnd, &p1OverlayData, &p1LastOverlayData, &p2OverlayData, &p2LastOverlayData);
    subThread = &sub;

    // Step 3: The Message Loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}

