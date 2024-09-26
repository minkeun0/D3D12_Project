// Project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Framework.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Framework gameWithWin32(hInstance, nCmdShow, 1000, 600, L"MyGame");
    return gameWithWin32.Run(hInstance, nCmdShow);
}