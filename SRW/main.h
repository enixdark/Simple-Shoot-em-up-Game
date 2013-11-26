#ifndef __MAIN_H
#define __MAIN_H

//windows 相關
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

//directx 相關
#include <d3d9.h>
#include <d3dx9.h>

//CEGUI
#include <CEGUI.h>
#include "CEGuiSample.h"

#include <RendererModules\Direct3D9\CEGUIDirect3D9Renderer.h>
using namespace CEGUI;

//directx 相關
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dxof.lib")
#pragma comment(lib,"dxguid.lib")

//CEGUI 相關
#ifdef _DEBUG
#pragma comment(lib,"CEGUIBase_d.lib")
#pragma comment(lib,"CEGUIDirect3D9Renderer_d.lib")
#else
#pragma comment(lib,"CEGUIBase.lib")
#pragma comment(lib,"CEGUIDirect3D9Renderer.lib")
#endif

// 釋放記憶體
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//可以在視窗後面顯示一個 console 方便除錯
#pragma comment(linker, "/subsystem:console /entry:WinMainCRTStartup")	

//windows 相關
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//directx 相關
class MY_VERTEX
{
public:
	D3DXVECTOR3 p;
	DWORD       color;
};

const DWORD MY_VERTEX_FVF=(D3DFVF_XYZ | D3DFVF_DIFFUSE);

static LPDIRECT3D9				g_pD3D;			
static LPDIRECT3DDEVICE9		g_pD3DDevice;	
static D3DPRESENT_PARAMETERS	g_D3DPP;

static D3DXMATRIX g_proj;
static D3DXMATRIX g_view;
static D3DXMATRIX g_world;//

static LPDIRECT3DVERTEXBUFFER9 g_pVB;
static LPDIRECT3DTEXTURE9 g_pTexture;

void DirectXCreate(HWND hWnd);
void DirectXRender();
void DirectXDestroy();
void DirectXMatrix();
void DirectXRenderState();

//CEGUI 相關
void CEGUICreate();
void CEGUIDestroy();

class Demo6Sample /*: public CEGuiSample*/
{
public:
	// method to initialse the samples windows and events.
	virtual bool initialiseSample();

	// method to perform any required cleanup operations.
	virtual void cleanupSample(void);


	Demo6Sample(){

	}
	void createDemoWindows(LPDIRECT3DDEVICE9 g);
	void initDemoEventWiring(void);


	bool handleQuit(const CEGUI::EventArgs& e);
	bool handleAddColumn(const CEGUI::EventArgs& e);
	bool handleDeleteColumn(const CEGUI::EventArgs& e);
	bool handleAddRow(const CEGUI::EventArgs& e);
	bool handleDeleteRow(const CEGUI::EventArgs& e);
	bool handleSetItem(const CEGUI::EventArgs& e);
	bool handleSelectChanged(const CEGUI::EventArgs& e);
	bool handleSelectModeChanged(const CEGUI::EventArgs& e);
	bool handleContentsChanged(const CEGUI::EventArgs& e);
	void render(){
		CEGUI::System::getSingleton().renderGUI();
	}
	void click(LPARAM l){
		CEGUI::System::getSingleton().injectMousePosition((float)(LOWORD(l)), (float)(HIWORD(l)));
	}
};

#endif