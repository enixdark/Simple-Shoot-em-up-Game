// Beginning Game Programming, Third Edition
// MyWindows.cpp

#include "SRW.h"
#include "Database.h"
using namespace std;
bool gameover = false;
bool check1 = true;
//extern int choose = 1;

LRESULT WINAPI WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
	case WM_DESTROY:
		gameover = true;
		PostQuitMessage(0);
		return 0;
	case WM_CHAR:
		getCE()->injectChar(wParam);
		break;  
	case WM_LBUTTONDOWN:
		getCE()->injectMouseButtonLDown();
		break;
	case WM_LBUTTONUP:
		getCE()->injectMouseButtonLUp();
		break;
		/*case WM_MOUSEMOVE:
		getCE()->injectMousePosition(lParam);
		break; */
	case WM_KEYDOWN:                            
		switch(wParam)
		{
		case VK_ESCAPE:                         
			getCE()->injectKeyDown(wParam,lParam);
			break;
		}
		break;
	case WM_KEYUP:
		getCE()->injectKeyUp(wParam,lParam);
		break;
	}	
			
    return DefWindowProc( hWnd, msg, wParam, lParam );
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	clock_t t = clock();
    //initialize window settings
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX); 
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = (WNDPROC)WinProc;
    wc.cbClsExtra	 = 0;
    wc.cbWndExtra	 = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = APPTITLE.c_str();
    wc.hIconSm       = NULL;
    RegisterClassEx(&wc);

    //create a new window
    HWND window = CreateWindow( APPTITLE.c_str(), APPTITLE.c_str(),
       WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
       SCREENW, SCREENH, NULL, NULL, hInstance, NULL);
    if (window == 0) return 0;

    //display the window
    ShowWindow(window, nCmdShow);
    UpdateWindow(window);
	
	//initialize the game
    if (!Game_Init(window)) return 0;
	//if (!Run_init(window)) return 0;
	if (!Menu_Init(window)) return 0;
    // main message loop
	MSG message;
	while (!gameover)
    {
        if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) 
	    {
		    TranslateMessage(&message);
		    DispatchMessage(&message);
	    }

        
		Game_Start(window);
        //Game_Run(window);
		/*if(Key_Down(DIK_ESCAPE) && check1 == true){
			std::string sum = std::to_string(getcountfire());
			std::string data = sum;
			sum = "so lan ban dan " + sum;
			clock_t e = (float)(clock() - t);
			if(MessageBox(NULL,sum.c_str(),"Hello",MB_OKCANCEL)==IDOK){
				odbc.Connect(); 
				std::string sql = "insert into Record(thoi_gian_choi,tong_luong_dan_su_dung) values ("+std::to_string(float(e)/CLOCKS_PER_SEC)+","+data+")";
				odbc.ExecuteNonQuery(sql.c_str());
				exit(0);
			}
			else
			{
				check1 = !Key_Down(DIK_ESCAPE);
				
			}
		} 

			if(Key_Down(DIK_SPACE)){
			check1 = true;
		}*/
		//check1 = true;
		
    }

    //shutdown
    Game_End();
	
	return message.wParam;
}

