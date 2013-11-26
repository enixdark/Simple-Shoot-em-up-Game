
#pragma  once
#include "CEGUIRender.h"
#include "header.h"
#include <Mmsystem.h>
#pragma   comment(lib, "winmm.lib ")

using namespace CEGUI;

namespace ZGame
{
	UINT V(WPARAM wParam, LPARAM lParam)
	{
		if(HIWORD(lParam) & 0x0F00) 
		{ 
			UINT scancode = MapVirtualKey(wParam, 0); 
			return scancode | 0x80; 
		} 
		else 
		{ 
			return HIWORD(lParam) & 0x00FF; 
		} 
	}
	void MainFrame::injectKeyDown(WPARAM wParam,LPARAM lParam){
		CEGUI::System::getSingleton().injectKeyDown((CEGUI::utf32)(V(wParam, lParam)));
	}
	void MainFrame::injectKeyUp(WPARAM wParam,LPARAM lParam){
		CEGUI::System::getSingleton().injectKeyUp((CEGUI::utf32)(V(wParam, lParam)));
	}
	MainFrame::MainFrame(D3DDEVTYPE deviceType, 
							 IDirect3DDevice9** device,HWND&hwnd) 
	{
		//create needed singleton managers
		createSingletons();
		initDirect3D(device,hwnd);
		initCEGUI(device);
		initResources();
		
	}

	MainFrame::~MainFrame()
	{

	}

	void MainFrame::initResources()
	{
		
		
		CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
			(CEGUI::System::getSingleton().getResourceProvider());

		rp->setResourceGroupDirectory("schemes", "datafiles/schemes/");
		rp->setResourceGroupDirectory("imagesets", "datafiles/imagesets/");
		rp->setResourceGroupDirectory("fonts", "datafiles/fonts/");
		rp->setResourceGroupDirectory("layouts", "datafiles/layouts/");
		rp->setResourceGroupDirectory("looknfeels", "datafiles/looknfeel/");
		rp->setResourceGroupDirectory("lua_scripts", "datafiles/lua_scripts/");
		rp->setResourceGroupDirectory("schemas", "../../XMLRefSchema/");
		
		rp->setResourceGroupDirectory("schemas", "datafiles/xml_schemas/");

		CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);
		
		CEGUI::Imageset::setDefaultResourceGroup("imagesets");
		CEGUI::Font::setDefaultResourceGroup("fonts");
		CEGUI::Scheme::setDefaultResourceGroup("schemes");
		CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
		CEGUI::WindowManager::setDefaultResourceGroup("layouts");
		CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

		
		CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
		if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
			parser->setProperty("SchemaDefaultResourceGroup", "schemas");

	
		CEGUI::SchemeManager::getSingleton().create( "TaharezLook.scheme" );
		
		System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
		
		CEGUI::FontManager::getSingleton().create( "DejaVuSans-10.font" );

		// Font defaulting
		if(!FontManager::getSingleton().isDefined("DejaVuSans-10"))
		{
			System::getSingleton().setDefaultFont("DejaVuSans-10");
		}

		WindowManager& winMgr = WindowManager::getSingleton();
		DefaultWindow* root = (DefaultWindow*)winMgr.createWindow("DefaultWindow", "Root");
		System::getSingleton().setGUISheet(root);

		/*CEGUI::Window* wnd = (CEGUI::Window*)winMgr.createWindow("Zodiac/Timer", "Demo Window");
		root->addChildWindow(wnd);

		wnd->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.25f)));
		wnd->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.005f)));

		wnd->setMaxSize(UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
		wnd->setMinSize(UVector2(cegui_reldim(0.1f), cegui_reldim( 0.1f)));*/

		Editbox* ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "NewColIDBox"));
		root->addChildWindow(ebox);
		ebox->setPosition(UVector2(cegui_reldim(0.3f), cegui_reldim( 0.25f)));
		ebox->setSize(UVector2(cegui_reldim(0.4f), cegui_reldim( 0.1f)));
		//ebox->setFont("FairChar-30.font");
		ebox->setText("");
		
		/*PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "AddColButton"));
		root->addChildWindow(btn);
		btn->setPosition(UVector2(cegui_reldim(0.81f), cegui_reldim( 0.32f)));
		btn->setSize(UVector2(cegui_reldim(0.15f), cegui_reldim( 0.2f)));
		btn->setText("Add");
		subscribeEvent("AddColButton",PushButton::EventClicked,Event::Subscriber(&ZGame::MainFrame::mes,this));*/
		//CEGUI::Window *editText = static_cast<Editbox*>(CEGUI::WindowManager::getSingleton().getWindow("ChatDemo/Text"));
		///*Editbox* m_chatText = static_cast<Editbox*> (editText);*/
		//root->addChildWindow(editText);
		//addChatText(editText->getText());

		//editText->setText("");
	}

	

	void MainFrame::render()
	{
		/*if (NULL==m_pDevice)
			return ;*/
		
		Update();
		CEGUI::System::getSingleton().renderGUI();
		
	}

	

	void MainFrame::createSingletons()
	{
		
		new InputManager();
	}

	void MainFrame::initCEGUI(IDirect3DDevice9** device)
	{
		
		CEGUI::Direct3D9Renderer& myRenderer = CEGUI::Direct3D9Renderer::create(*device);
		CEGUI::System::create( myRenderer );
	}

	void MainFrame::subscribeEvent(const CEGUI::String& widgetName, const CEGUI::String& eventName, const CEGUI::Event::Subscriber& method)
	{
		WindowManager& winMgr = WindowManager::getSingleton();
		
		if (winMgr.isWindowPresent(widgetName))
		{
			winMgr.getWindow(widgetName)->subscribeEvent(eventName,method);
		}
	}

	void MainFrame::initDirect3D(IDirect3DDevice9** device,const HWND &hwnd)
	{
		

		m_pInputManager = new ZGame::InputManager();
		m_pInputManager->initD3DInput();
		m_pInputManager->initMouse(hwnd);
		m_pInputManager->initKeyboard(hwnd);
	}

	void MainFrame::Update()
	{
		POINT  mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(m_hwnd,&mousePos);
		CEGUI::System::getSingleton().injectMousePosition( (float)mousePos.x, (float)mousePos.y);
	}

	bool MainFrame::mes(const CEGUI::EventArgs &e){
		WindowManager& winMgr = WindowManager::getSingleton();
		Editbox* idbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("NewColIDBox"));
		String s = idbox->getText();
		MessageBox(NULL,s.c_str(),"Hello",MB_OK);
		return true;
	}
	std::string MainFrame::getTextbox(){
		Editbox* idbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("NewColIDBox"));
		String s = idbox->getText();
		return s.c_str();
	}
}