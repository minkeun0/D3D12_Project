// Project.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Scene.h"

_Use_decl_annotations_
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    Scene sample(1280, 720, L"MyGame");
    return Win32Application::Run(&sample, hInstance, nCmdShow);
}