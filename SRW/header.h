
#ifndef HEADER_H
#define HEADER_H

#pragma once

#include <windows.h>
#include <iostream>
#include <vector>

using namespace std;


#include <d3dx9.h>
#include <d3d9.h>
#include <dinput.h>
#include <dxerr.h>

#include <CEGUI.h>
#include "CEGuiSample.h"
#include "CEGuiBaseApplication.h"
#include "RendererModules\Direct3D9\CEGUIDirect3D9Renderer.h"

#define SafeDelete(p)		{if(p) { delete (p); (p) = NULL;}}
#define SafeDeleteArray(p)	{if(p) { delete[] (p); (p) = NULL;}}
#define SafeRelease(p)		{if(p) { (p)->Release();(p) = NULL;}}

//=====================================================
//Debug
//#if defined(DEBUG) | defined(_DEBUG)
//#ifndef HR
//#define HR(x)                                      \
//{                                              \
//	HRESULT hr = x;                                \
//	if(FAILED(hr))                                 \
//{                                          \
//	DXTrace(__FILE__, __LINE__, hr, #x, TRUE); \
//}                                          \
//}
//#endif
//
//#else
//#ifndef HR
//#define HR(x) x;
//#endif
//#endif 

#endif
