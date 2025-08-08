#include "Win32Application.h"
#include "Framework.h"
#include <WindowsX.h>

Win32Application::Win32Application(HINSTANCE hInstance, UINT width, UINT height) :
    m_width{ width },
    m_height{ height },
    m_aspectRatio{ static_cast<float>(width) / static_cast<float>(height) }
{
    CreateWnd(hInstance);
}

void Win32Application::CreateWnd(HINSTANCE hInstance)
{
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"MyGameClass";
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindow(
        windowClass.lpszClassName,
        GetTitle(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left, 
        windowRect.bottom - windowRect.top, 
        nullptr,        
        nullptr,        
        hInstance,
        nullptr);
}

void Win32Application::SetCustomWindowText(LPCWSTR text)
{
    std::wstring windowText = m_title + L" : " + text;
    SetWindowText(m_hwnd, windowText.c_str());
}

void Win32Application::OnResize(UINT width, UINT height)
{
    m_width = width;
    m_height = height;
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Framework* pSample = reinterpret_cast<Framework*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_SIZE:
        if (pSample)
        {
            pSample->OnResize(LOWORD(lParam), HIWORD(lParam), wParam == SIZE_MINIMIZED);
        }
        break;

    case WM_ACTIVATE:
        // wParam의 하위 워드(LOWORD)가 활성화 상태를 나타냅니다.
        if (LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
        {
            // 창이 활성화됨
            pSample->SetWndActivateState(true);
        }
        else // WA_INACTIVE
        {
            // 창이 비활성화됨
            pSample->SetWndActivateState(false);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
