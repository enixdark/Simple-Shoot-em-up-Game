
#include "CEGUIInput.h"
#include "d3d9.h"
namespace ZGame
{
	InputManager::InputManager(void)
	{
		m_pIDirectInput=NULL;
		m_pIDirectInputMouseDevice=NULL;
		m_pIDirectInputKeyboardDevice=NULL;
	}

	InputManager::~InputManager(void)
	{
		ReleaseCOMObject();
	}

	bool InputManager::initD3DInput()
	{
		HRESULT hr;
		
		hr=DirectInput8Create(GetModuleHandle(NULL),DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&m_pIDirectInput,NULL);
		if(FAILED(hr))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}
		return TRUE;
	}

	bool InputManager::initKeyboard(HWND hWnd)
	{
		HRESULT hr;

		hr = m_pIDirectInput->CreateDevice(GUID_SysKeyboard,&m_pIDirectInputKeyboardDevice,NULL);
		if (FAILED(hr))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
			SafeRelease(m_pIDirectInput);
		}
		hr = m_pIDirectInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
		if(FAILED(hr))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
			SafeRelease(m_pIDirectInputKeyboardDevice);
			SafeRelease(m_pIDirectInput);
			return false;
		}
		
		hr=m_pIDirectInputKeyboardDevice->SetCooperativeLevel(hWnd,DISCL_BACKGROUND|DISCL_NONEXCLUSIVE);
		if(FAILED(hr))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
			SafeRelease(m_pIDirectInputKeyboardDevice);
			SafeRelease(m_pIDirectInput);
			return false;
		}
		
		DIPROPDWORD dipROPWORD;
		dipROPWORD.diph.dwSize=sizeof(DIPROPDWORD);
		dipROPWORD.diph.dwHeaderSize=sizeof(DIPROPHEADER);
		dipROPWORD.diph.dwObj=0;
		dipROPWORD.diph.dwHow=DIPH_DEVICE;
		dipROPWORD.dwData=0; 
		if(FAILED(m_pIDirectInputKeyboardDevice->SetProperty(DIPROP_BUFFERSIZE,&dipROPWORD.diph)))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
			SafeRelease(m_pIDirectInputKeyboardDevice);
			SafeRelease(m_pIDirectInput);
			return false;
		}
		
		hr=m_pIDirectInputKeyboardDevice->Acquire();
		if(FAILED(hr))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
			SafeRelease(m_pIDirectInputKeyboardDevice);
			SafeRelease(m_pIDirectInput);
			return false;
		}

		return true;
	}

	bool InputManager::initMouse(HWND hWnd){
		HRESULT hr;

	
		hr=m_pIDirectInput->CreateDevice(GUID_SysMouse,&m_pIDirectInputMouseDevice,NULL);
		if(FAILED(hr))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
			SafeRelease(m_pIDirectInput);
			return false;
		}
		
		hr=m_pIDirectInputMouseDevice->SetDataFormat(&c_dfDIMouse);
		if(FAILED(hr))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
			SafeRelease(m_pIDirectInputMouseDevice);
			SafeRelease(m_pIDirectInput);
			return false;
		}
	
		hr=m_pIDirectInputMouseDevice->SetCooperativeLevel(hWnd,DISCL_BACKGROUND|DISCL_NONEXCLUSIVE);
		if(FAILED(hr))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
			SafeRelease(m_pIDirectInputMouseDevice);
			SafeRelease(m_pIDirectInput);
			return false;
		}
		
		DIPROPDWORD dipROPWORD;
		dipROPWORD.diph.dwSize=sizeof(DIPROPDWORD);
		dipROPWORD.diph.dwHeaderSize=sizeof(DIPROPHEADER);
		dipROPWORD.diph.dwObj=0;
		dipROPWORD.diph.dwHow=DIPH_DEVICE;
		dipROPWORD.dwData=ITEMS_NUM; 
		if(FAILED(m_pIDirectInputMouseDevice->SetProperty(DIPROP_BUFFERSIZE,&dipROPWORD.diph)))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);

			SafeRelease(m_pIDirectInputMouseDevice);
			SafeRelease(m_pIDirectInput);
			return false;
		}
		
		hr=m_pIDirectInputMouseDevice->Acquire();
		if(FAILED(hr))
		{
			MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
			SafeRelease(m_pIDirectInputMouseDevice);
			SafeRelease(m_pIDirectInput);
			return false;
		}
		ShowCursor(FALSE);
		m_lMouseLeftButton=FALSE;
		m_lMouseRightButton=FALSE;
		return true;
	}

	void InputManager::CaptureMouse()
	{
		HRESULT hr;
		DWORD dwReadNum=1;
		int i;
		
		m_lMouseMoveX = 0; m_lMouseMoveY = 0; m_lMouseMoveZ = 0;
		ZeroMemory(m_diDeviceObjectData,sizeof(DIDEVICEOBJECTDATA)*ITEMS_NUM);
		
		for(i=0;i<ITEMS_NUM;i++)
		{
			hr=m_pIDirectInputMouseDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),&m_diDeviceObjectData[i],&dwReadNum,0);
			if(hr==DIERR_INPUTLOST)
			{
				m_pIDirectInputMouseDevice->Acquire();
				hr=m_pIDirectInputMouseDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),&m_diDeviceObjectData[i],&dwReadNum,0);
				if(FAILED(hr)){
					MessageBox(NULL,"loi","Error",MB_OK|MB_ICONINFORMATION);
					return;
				}
			}
			if(m_diDeviceObjectData[i].dwOfs==DIMOFS_X){
				m_lMouseMoveX+=m_diDeviceObjectData[i].dwData;
			}
			if(m_diDeviceObjectData[i].dwOfs==DIMOFS_Y){
				m_lMouseMoveY+=m_diDeviceObjectData[i].dwData;
			}
			if(m_diDeviceObjectData[i].dwOfs==DIMOFS_Z){
				m_lMouseMoveZ+=m_diDeviceObjectData[i].dwData;
			}
			if (m_diDeviceObjectData[i].dwOfs==DIMOFS_BUTTON0){
				m_lMouseLeftButton=m_diDeviceObjectData[i].dwData;
			}
			if (m_diDeviceObjectData[i].dwOfs==DIMOFS_BUTTON1){
				m_lMouseRightButton=m_diDeviceObjectData[i].dwData;
			}
		}
	}

	bool InputManager::isKeyPressed(int key)
	{
		BYTE g_key_buffer[256];
		ZeroMemory(g_key_buffer,sizeof(g_key_buffer));
		
		HRESULT hr = m_pIDirectInputKeyboardDevice->GetDeviceState(sizeof(g_key_buffer), g_key_buffer);

		if(hr == DIERR_INPUTLOST)
		{
			
			m_pIDirectInputKeyboardDevice->Acquire();

			if(FAILED(m_pIDirectInputKeyboardDevice->GetDeviceState(sizeof(g_key_buffer), g_key_buffer)))
			{
				MessageBox(NULL, "Get keyboard state failed.", "ERROR", MB_OK | MB_ICONINFORMATION);
				return FALSE;
			}
		}

		return (g_key_buffer[key] & 0x80);        
	}


	void InputManager::ReleaseCOMObject()
	{
		if(m_pIDirectInputMouseDevice)
			m_pIDirectInputMouseDevice->Unacquire();   
		SafeRelease(m_pIDirectInputMouseDevice);

		if(m_pIDirectInputKeyboardDevice)
			m_pIDirectInputKeyboardDevice->Unacquire();
		SafeRelease(m_pIDirectInputKeyboardDevice);
		SafeRelease(m_pIDirectInput);
	}
}