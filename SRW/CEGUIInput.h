

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include "header.h"

#define ITEMS_NUM 10 

namespace ZGame
{
	class InputManager
	{
	public:
		
		InputManager();

	
		~InputManager();

		
		bool initD3DInput();

		
		bool initMouse(HWND hwnd);

		
		bool initKeyboard(HWND hwnd);

		
		void CaptureMouse();

		
		void ReleaseCOMObject();

		
		bool isKeyPressed(int key);

	private:
		
		LPDIRECTINPUT8 m_pIDirectInput;

		
		LPDIRECTINPUTDEVICE8 m_pIDirectInputMouseDevice;

		
		LPDIRECTINPUTDEVICE8 m_pIDirectInputKeyboardDevice;

		DIDEVICEOBJECTDATA m_diDeviceObjectData[ITEMS_NUM];
	public:
		long m_lMouseMoveX; 
		long m_lMouseMoveY;  
		long m_lMouseMoveZ;  
		BOOL m_lMouseLeftButton;
		BOOL m_lMouseRightButton;

	private:
		static InputManager* m_pInputManager;
	};
}
