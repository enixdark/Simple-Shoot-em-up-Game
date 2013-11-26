#include "main.h"
#include "CEGui.h"
#include <iostream>
//win 32 視窗程式進入點
//Demo6Sample app;
Demo6Sample app;
int i =0;
bool hand(const CEGUI::EventArgs&e);
void initevent();
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//填好視窗資料
	WNDCLASSEX wndclass = {sizeof(wndclass), 
		CS_HREDRAW|CS_VREDRAW,                //style
		WndProc,                             
		0, 0, hInstance,                     
		LoadIcon(NULL, IDI_APPLICATION),      //hIcon
		LoadCursor(NULL,IDC_ARROW),           //hCursor
		(HBRUSH)(COLOR_WINDOW+1),             //hbrBackground
		NULL,                                 //hMenu
		_T("window"), 
		LoadIcon(NULL, IDI_APPLICATION)};     //hIconSm

	RegisterClassEx(&wndclass);

	//產生視窗
	HWND hWnd = CreateWindowEx(
		0,                              //dwExStyle 
		_T("window"), _T("window"),             //視窗標題
		WS_OVERLAPPEDWINDOW,            //dwStyle
		CW_USEDEFAULT, CW_USEDEFAULT,
		640, 480, 
		NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//DirectX 初始化
	DirectXCreate(hWnd);
	//initevent();
	/*Demo6Sample app;*/
	//app.initialiseSample();
	//app.createDemoWindows(g_pD3DDevice);
	//app.initDemoEventWiring();
//app.initialiseSample();
	//CEGUI 資源與資料初始化
	CEGUICreate();
	
	MSG msg;
	while(TRUE)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
		    if(msg.message==WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//DirectX 繪圖
			DirectXRender();
		}
	}
	//CEGUI 資源釋放
	/*CEGUIDestroy();*/

	//DirectX 資源釋放
	DirectXDestroy();

	return (int)msg.wParam;
}

//訊息迴圈
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_MOUSEMOVE:
			//app.click(lParam);
			CEGUI::System::getSingleton().injectMousePosition((float)(LOWORD(lParam)), (float)(HIWORD(lParam)));
			break;
		case WM_LBUTTONDOWN:
			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
			
			break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//DirectX 初始化
void DirectXCreate(HWND hWnd)
{
	//初始化directX
	D3DDISPLAYMODE d3ddm;
    
	if((g_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION)) == 0) 
		return;
    if(FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm))) 
		return;
    
	ZeroMemory(&g_D3DPP, sizeof(D3DPRESENT_PARAMETERS));
	g_D3DPP.BackBufferCount			= 1;
	g_D3DPP.Windowed				= TRUE;				
	g_D3DPP.BackBufferFormat		= d3ddm.Format;			
	g_D3DPP.SwapEffect				= D3DSWAPEFFECT_DISCARD;	
	g_D3DPP.EnableAutoDepthStencil	= TRUE;						
	g_D3DPP.AutoDepthStencilFormat	= D3DFMT_D16;

	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
								   D3DCREATE_HARDWARE_VERTEXPROCESSING, 
								   &g_D3DPP, &g_pD3DDevice))) {
	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, 
								   D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
								   &g_D3DPP, &g_pD3DDevice))) {
	if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, 
								   D3DCREATE_SOFTWARE_VERTEXPROCESSING, 
								   &g_D3DPP, &g_pD3DDevice))) {
				return;
	}}}

	//初始化頂點資料
	if(FAILED(g_pD3DDevice->CreateVertexBuffer(4*sizeof(MY_VERTEX),D3DUSAGE_WRITEONLY,
												MY_VERTEX_FVF,D3DPOOL_MANAGED,
												&g_pVB, NULL ))) return;

	MY_VERTEX* v;
    g_pVB->Lock( 0, 0, (void**)&v, 0 );

    v[0].p = D3DXVECTOR3(-5.5f,  1.5f, 0.0f );
    v[1].p = D3DXVECTOR3(-1.5f,  1.5f, 0.0f );
    v[2].p = D3DXVECTOR3(-5.5f, -1.5f, 0.0f );
	v[3].p = D3DXVECTOR3(-1.5f, -1.5f, 0.0f );

    v[0].color = v[1].color = v[2].color = v[3].color =
	D3DXCOLOR( 1.0f, 1.0f,  1.0f,  1.0f);
    g_pVB->Unlock();
}
void DirectXMatrix()
{
	D3DXMatrixIdentity( &g_view);
	D3DXMatrixLookAtLH( &g_view, &D3DXVECTOR3(0.0f, 0.0f, -10.0f), 
								 &D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
								 &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
    g_pD3DDevice->SetTransform(D3DTS_VIEW, &g_view);

	D3DVIEWPORT9    vp;
	if(FAILED(g_pD3DDevice->GetViewport(&vp))) return;
	float aspect;
	aspect = (float)vp.Width / (float)vp.Height;
	D3DXMatrixIdentity(&g_proj);
	D3DXMatrixPerspectiveFovLH(&g_proj, D3DXToRadian(45.0f), aspect, 5.0f, 1000.0f);
	g_pD3DDevice->SetTransform(D3DTS_PROJECTION, &g_proj);

	D3DXMatrixIdentity(&g_world);
	g_pD3DDevice->SetTransform(D3DTS_WORLD, &g_world);
}
//DirectX 繪圖
void DirectXRender()
{
	g_pD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB(0,255,0),1.0f,0);
	g_pD3DDevice->BeginScene();

	//重設3D物件的矩陣
	DirectXMatrix();

	//重設3D物件的 render state
	DirectXRenderState();

	//3D物件的繪圖
	g_pD3DDevice->SetStreamSource( 0, g_pVB, 0, sizeof(MY_VERTEX) );
    g_pD3DDevice->SetFVF( MY_VERTEX_FVF );
    g_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	//CEGUI 繪圖
	/*CEGUI::System::getSingleton().renderGUI();*/
	app.render();
	g_pD3DDevice->EndScene();
	g_pD3DDevice->Present(0,0,0,0);
}

//DirectX 資源釋放
void DirectXDestroy()
{
	SAFE_RELEASE(g_pVB);
	SAFE_RELEASE(g_pD3DDevice);
	SAFE_RELEASE(g_pD3D);
}
//CEGUI 資源與資料初始化
void CEGUICreate()
{
	CEGUI::Direct3D9Renderer* myRenderer = &CEGUI::Direct3D9Renderer::create(g_pD3DDevice);
	CEGUI::System::create(*myRenderer);

	CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

	rp->setResourceGroupDirectory("schemes", "datafiles/schemes/");
	rp->setResourceGroupDirectory("imagesets","datafiles/imagesets/");
	rp->setResourceGroupDirectory("fonts", "datafiles/fonts/");
	rp->setResourceGroupDirectory("layouts", "datafiles/layouts/");
	rp->setResourceGroupDirectory("looknfeels","datafiles/looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts","datafiles/lua_scripts/");
	rp->setResourceGroupDirectory("schemas", "../../XMLRefSchema/");
 
	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
 
	SchemeManager::getSingleton().create("TaharezLook.scheme");
    System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
    WindowManager& winMgr = WindowManager::getSingleton();
    DefaultWindow* root = (DefaultWindow*)winMgr.createWindow("DefaultWindow", "Root");
    System::getSingleton().setGUISheet(root);
    FrameWindow* wnd = (FrameWindow*)winMgr.createWindow("TaharezLook/FrameWindow", "Demo Window");
    root->addChildWindow(wnd);
    wnd->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.25f)));
    wnd->setSize(UVector2(cegui_reldim(0.5f), cegui_reldim( 0.5f)));
    wnd->setMaxSize(UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
    wnd->setMinSize(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.1f)));
    wnd->setText("Hello World!");

	PushButton *btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Demo6/ControlPanel/ColumnPanel/DelColButton"));
	wnd->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.67f)));
	btn->setSize(UVector2(cegui_reldim(0.4f), cegui_reldim( 0.2f)));
	btn->setText("Delete Column");
	btn->subscribeEvent(PushButton::EventClicked,&hand);
}
//CEGUI 資源釋放

bool hand(const CEGUI::EventArgs&e ){
	i++;
	std::cout<<i;
	return true;
}

//void initevent(){
//	WindowManager& winMgr = WindowManager::getSingleton();
//
//	// subscribe handler that adds a new column
//	winMgr.getWindow("Demo6/ControlPanel/ColumnPanel/AddColButton")->
//		subscribeEvent(PushButton::EventClicked,&hand);
//}
void CEGUIDestroy()
{
   CEGUI::Renderer *renderer = CEGUI::System::getSingleton().getRenderer();
   CEGUI::System::getSingleton().destroy();
   delete renderer;
}

void  DirectXRenderState()
{
	//  setup vertex stream 
	g_pD3DDevice -> SetFVF(NULL);
  
	//  set device states 
	g_pD3DDevice -> SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	g_pD3DDevice -> SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	g_pD3DDevice -> SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	g_pD3DDevice -> SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	g_pD3DDevice -> SetRenderState(D3DRS_FOGENABLE, FALSE);
	g_pD3DDevice -> SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
   
	//  setup texture addressing settings/* 
	g_pD3DDevice -> SetSamplerState(  0 , D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pD3DDevice -> SetSamplerState(  0 , D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
   
	//  setup colour calculations 
	g_pD3DDevice -> SetTextureStageState( 0 , D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pD3DDevice -> SetTextureStageState( 0 , D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pD3DDevice -> SetTextureStageState( 0 , D3DTSS_COLOROP, D3DTOP_SELECTARG1);
   
	//  setup alpha calculations 
	g_pD3DDevice -> SetTextureStageState( 0 , D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice -> SetTextureStageState( 0 , D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	g_pD3DDevice -> SetTextureStageState( 0 , D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
   
	//  setup filtering 
	g_pD3DDevice -> SetSamplerState( 0 , D3DSAMP_MINFILTER, D3DTEXF_POINT);
	g_pD3DDevice -> SetSamplerState( 0 , D3DSAMP_MAGFILTER, D3DTEXF_POINT);
   
	//  disable texture stages we do not need. 
	g_pD3DDevice -> SetTextureStageState( 1 , D3DTSS_COLOROP, D3DTOP_DISABLE);
   
	//  setup scene alpha blending 
	g_pD3DDevice -> SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pD3DDevice -> SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	g_pD3DDevice -> SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO); 
}
