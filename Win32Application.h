//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once
#include "Framework.h"

class Framework;

class Win32Application
{
public:
    Win32Application() = default;
    Win32Application(UINT width, UINT height, std::wstring name);

    void CreateWnd(Framework* framework, HINSTANCE hInstance);

    void SetCustomWindowText(LPCWSTR text);

    static HWND GetHwnd() { return m_hwnd; }
    const WCHAR* GetTitle() const { return m_title.c_str(); }
    UINT GetWidth() const { return m_width; }
    UINT GetHeight() const { return m_height; }

protected:
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    static HWND m_hwnd;

    // Window title.
    std::wstring m_title;

    // Window dimensions.
    UINT m_width;
    UINT m_height;
    float m_aspectRatio;

};
