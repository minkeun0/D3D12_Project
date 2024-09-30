#include "stdafx.h"
#include "Framework.h"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Framework gameWithWin32(hInstance, nCmdShow, 1000, 800, L"MyGame");
    return gameWithWin32.Run(hInstance, nCmdShow);
}