
#include "CEGUIInput.h"
#include <d3dx9.h>
#include "CEGUISystem.h"

#include <CEGUI.h>
#include "CEGuiSample.h"
#include "CEGuiBaseApplication.h"
#include "RendererModules\Direct3D9\CEGUIDirect3D9Renderer.h"


namespace ZGame
{
	
	class MainFrame
	{
	public:

		MainFrame(){}
		MainFrame(D3DDEVTYPE deviceType, IDirect3DDevice9** device,HWND&hwnd);
		~MainFrame();
		void render();
		void createSingletons();
		void initResources();
		void subscribeEvent(const CEGUI::String& widgetName, const CEGUI::String& eventName, const CEGUI::Event::Subscriber& method);
		void Update();
		bool mes(const  CEGUI::EventArgs &e);

		void injectChar(WPARAM wParam){
			CEGUI::System::getSingleton().injectChar((CEGUI::utf32)wParam);
		}
		void injectMouseButtonLDown(){
			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
		}
		void injectMousePosition(LPARAM lParam){
			CEGUI::System::getSingleton().injectMousePosition((float)(LOWORD(lParam)), (float)(HIWORD(lParam)));
		}
		void injectMouseButtonLUp(){
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
		}
		void injectKeyDown(WPARAM wParam,LPARAM lParam);
		void injectKeyUp(WPARAM wParam,LPARAM lParam);
		std::string getTextbox();

	private:
		
		bool createRenderWindow(HINSTANCE hInstance, D3DDEVTYPE deviceType, IDirect3DDevice9** device, bool windowed);
		void initDirect3D(IDirect3DDevice9** device, const HWND& hwnd);
		void initCEGUI(IDirect3DDevice9** device);

		
	private:
		
		CEGUI::Window*   root;
		ZGame::InputManager* m_pInputManager;
		HWND m_hwnd;

	
	};

	

	
}

