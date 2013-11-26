//***********************************************************************
//	filename: 	main.cpp
//	created:		23/3/2012
//	author:		Zodiac
//
//	purpose:	mainLoop
//*************************************************************************/
//***************************************************************************
// *   Copyright (C) Copyright (C) 2012-2012 Zodiac Hust
// *
// *   Permission is hereby granted, free of charge, to any person obtaining
// *   a copy of this software and associated documentation files (the
// *   "Software"), to deal in the Software without restriction, including
// *   without limitation the rights to use, copy, modify, merge, publish,
// *   distribute, sublicense, and/or sell copies of the Software, and to
// *   permit persons to whom the Software is furnished to do so, subject to
// *   the following conditions:
// *
// *   The above copyright notice and this permission notice shall be
// *   included in all copies or substantial portions of the Software.
// *
// *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// *   OTHER DEALINGS IN THE SOFTWARE.
// ***************************************************************************/
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <dxerr.h>

#include <CEGUI.h>
#include "CEGuiSample.h"
#include "CEGUIRender.h"
#include "CEGuiBaseApplication.h"

#include "RendererModules\Direct3D9\CEGUIDirect3D9Renderer.h"

//--D3DÓÃµÄlib¿â
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment (lib, "DxErr.lib")

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dinput8.lib")

#pragma comment(lib, "CEGUIBase_d.lib")
#pragma comment(lib, "CEGUIDirect3D9Renderer_d.lib")

LPDIRECT3DDEVICE9 device = NULL;



//int WINAPI WinMain(HINSTANCE hinstance,
//				   HINSTANCE prevInstance, 
//				   PSTR cmdLine,
//				   int showCmd)
//{
//	ZGame::MainFrame *myFrame = new ZGame::MainFrame(hinstance, D3DDEVTYPE_HAL, &device,true);
//	myFrame->run();
//}