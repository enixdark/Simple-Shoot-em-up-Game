
#include "SRW.h"
#include <sstream>
#include "Database.h"
#include <time.h>
//#include "ZGameMainFrame.h"
using namespace std;


ODBC odbc;
LPD3DXFONT font;
LPD3DXFONT debugfont;
LPD3DXFONT Largefont;

const string APPTITLE = "SRW Project ";
const int SCREENW = 1000;
const int SCREENH = 700;
int x=20,y=6;
int total = 0;
bool check = false;

//time
bool paused = false;
DWORD refresh = 0;
DWORD screentime = 0;
double screenfps = 0.0;
double screencount = 0.0;
DWORD coretime = 0;
double corefps = 0.0;
double corecount = 0.0;
DWORD currenttime;

//background scrolling --------------------------------------------------------
const int BUFFERW = SCREENW * 2;
const int BUFFERH = SCREENH;
double scrollx=0;
double scrolly=0;
LPDIRECT3DSURFACE9 background = NULL,bg = NULL;
const double virtual_level_size = BUFFERW * 5;
double virtual_scrollx = 0;

//player 
LPDIRECT3DTEXTURE9 player_ship;
SPRITE player;
enum PLAYER_STATES
{
	NORMAL
};
PLAYER_STATES player_state = NORMAL;
PLAYER_STATES player_state_previous = NORMAL;
D3DXVECTOR2 position_history[8];
int position_history_index = 0;
DWORD position_history_timer = 0;
//state Game
enum PLAYER_GAME
{
	BEGIN = 0,
	END = 1
};
PLAYER_GAME GameStart = END;
//monster
const int MONSTER1 = 10;
const int MONSTER2 = 20;
LPDIRECT3DTEXTURE9 monster1_image;
LPDIRECT3DTEXTURE9 monster2_image;
SPRITE monster1[MONSTER1];
SPRITE monster2[MONSTER2];

//bullet 
LPDIRECT3DTEXTURE9 purple_fire,purple_fire1,thunder_fire ;
const int BULLETS = 10000;
SPRITE bullets[BULLETS];
SPRITE bullets1[BULLETS];
SPRITE bullets2[BULLETS];
SPRITE thunderbullets[8];
int player_shoot_timer = 0;
int enemy_shoot_timer = 0;
int firepower = 5;
int bulletcount = 0,bulletcount2 = 0;
int countfire=0;
int hpp[MONSTER1];
//explosion,frame and time
bool Explosion_enemy[MONSTER1];
bool Explosion_enemy2[MONSTER2];
float timer_enemy[MONSTER1];
float timer_enemy2[MONSTER2];
float time_shot[MONSTER1];
LPDIRECT3DTEXTURE9 explosion = NULL;
int frame = 0;
int starttime = 0;
//position destroy
int destroyx = -144,destroyy = 0;
bool destroyz = false;
//player of life
const int continues = 1;
bool live[continues];
int nextt;
float timer = 0;

//Sound 
CSound *gun = NULL;
CSound *foom = NULL;
CSound *Dead = NULL;
CSound *thunder_sound = NULL;
CSound *OPENING = NULL;
CSound * Selectmenu = NULL;
CSound * GO = NULL;
LPDIRECT3DSURFACE9 image = NULL;
LPDIRECT3DSURFACE9 Menu = NULL;
//user choose
LPDIRECT3DTEXTURE9 selius = NULL;
SPRITE selius1[2];
LPDIRECT3DTEXTURE9 cybuster = NULL;
SPRITE cybuster1[2];
int selius_vt = 1;
int cybuster_vt = 0;
//
bool newgame = true;
int choose = 1;
bool playopening = true;

/////
//init color background image
D3DCOLOR color = D3DCOLOR_ARGB(255,255,255,255);

//select fire
LPDIRECT3DTEXTURE9 fire_level;
std::string fire_image = "fire";
SPRITE fire_bullet;

int sum = 0;
bool live_player = true;
int time_appear = 0;
//
LPDIRECT3DTEXTURE9 text;
SPRITE text_talk;
std::string name = "";
std::string infor = "";

vector<string*> vv ;
int delay = 0;
int menuchange = 1;
int Score = 0;
// GameOver Value
int GOSH = SCREENH;
int Delay = 0;

//
clock_t timeplay ;
bool startgame = true;
ZGame::MainFrame *z = new ZGame::MainFrame();

void loadbackgroundmenu(std::string s);

bool Game_Init(HWND window)
{
	AllocConsole() ;
	AttachConsole( GetCurrentProcessId() ) ;
	freopen( "CON", "w", stdout ) ;
    Direct3D_Init(window, SCREENW, SCREENH,false  );
    DirectInput_Init(window);
    DirectSound_Init(window);
	odbc.Connect();
	vv = odbc.ExecuteQueryVector("select * from Records order by Score desc");
	return true;
}
//
bool Menu_Init(HWND window)
{
	GO = LoadSound("sound\\GameOver.wav");
	OPENING = LoadSound("sound\\OPEN.wav");
	Selectmenu = LoadSound("sound\\select.wav");
	font = MakeFont("Arial",20);
	debugfont = MakeFont("Arial", 25);
	Largefont = MakeFont("Arial",100);
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	HRESULT result = 
		d3ddev->CreateOffscreenPlainSurface(
		BUFFERW,
		BUFFERH,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&Menu,
		NULL);
	if (result != D3D_OK) return false;

	result = D3DXLoadSurfaceFromFile(
		Menu,           
		NULL,              
		NULL,               
		"image\\Menu1.png",     
		NULL,               
		D3DX_DEFAULT,       
		0,                 
		NULL);             


	if (!SUCCEEDED(result)) return false;
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, 
		&backbuffer);
	//fill from imgae to background 
	z = new ZGame::MainFrame(D3DDEVTYPE_HAL, &d3ddev,window);
	return true;

}
void Reset(){


	for(int i =0 ;i <MONSTER1;i++){
		Explosion_enemy[i] = false;
		timer_enemy[i] = 0.0;
		time_shot[i] = rand()%10000;
	}
	for(int i =0 ;i <MONSTER2;i++){
		Explosion_enemy2[i] = false;
		timer_enemy2[i] = 0.0;
	}
	player.x = 100;
	player.y = 350;
	player.width = 144;
	player.height = 84;
	player.scaling = 0.5f;
	for (int i = 0; i < continues; i++)
	{
		live[i]= true;
	}
	nextt = 0;
	for (int n=0; n<4; n++)
		position_history[n] = D3DXVECTOR2(-100,0);
	for (int n=0; n<BULLETS; n++)
	{
		bullets[n].alive = false;
		bullets[n].x = 0;
		bullets[n].y = 0;
		bullets[n].width = 94;
		bullets[n].height = 55;

	}
	thunder_fire = LoadTexture("image\\thunder.png");
	for(int i = 0 ;i<8;i++){
		thunderbullets[i].x=0;
		thunderbullets[i].y=0;
		thunderbullets[i].width = 132;
		thunderbullets[i].height =41;
	}
	for (int n=0; n<BULLETS; n++)
	{
		bullets1[n].alive = false;
		bullets1[n].x = 0;
		bullets1[n].y = 0;
		bullets1[n].width = 63;
		bullets1[n].height = 60;
	}
	for (int n=0; n<MONSTER1; n++)
	{
		monster1[n].alive = true;
		monster1[n].width = 144;
		monster1[n].height = 83;
		monster1[n].x = (float)( 1000 + rand() % BUFFERW );
		monster1[n].y = (float)( rand() % SCREENH );
		monster1[n].velx = (float)((rand() % 4) * -2);
		monster1[n].vely = (float)((rand() % 2) * -2);
		if(monster1[n].velx>=-3)
			monster1[n].velx=-5;
		monster1[n].hp = 1;
		monster1[n].scaling = 0.5f;
	}
	for (int i=0; i<MONSTER2; i++)
	{
		monster2[i].alive = true;
		monster2[i].width = 156;
		monster2[i].height = 93;
		monster2[i].x = (float)( 1000 + rand() % BUFFERW );
		monster2[i].y = (float)( rand() % SCREENH );
		monster2[i].velx = (float)((rand() % 4) * -2);
		monster2[i].vely = 0.0;
		monster2[i].hp = 1;
		monster2[i].scaling = 0.5f;
	}
	fire_bullet.alive = false;
	fire_bullet.x = 0;//SCREENW;
	fire_bullet.y = 0;//SCREENH;
	fire_bullet.velx = 0.3;
	fire_bullet.vely = 0.3;
	fire_bullet.width = 93;
	fire_bullet.height = 55;

	text_talk.x = 0;
	text_talk.y = 0;
	text_talk.width = 1920;
	text_talk.height = 318;

	for(int i=0;i<2;i++){
		selius1[i].x = i*120;
		selius1[i].y = 0;
		selius1[i].width = 120;
		selius1[i].height = 115;
	}
	
	for(int i=0;i<2;i++){
		cybuster1[i].x = 183*i;
		cybuster1[i].y = 0;
		cybuster1[i].width = 183;
		cybuster1[i].height = 125;
	}
	position_history_index = 0;
	position_history_timer = 0;
	frame = 0;
	starttime = 0;
	//position destroy
	destroyx = -144,destroyy = 0;
	destroyz = false;
	player_shoot_timer = 0;
	enemy_shoot_timer = 0;
	firepower = 5;
	bulletcount = 0;
	bulletcount2 = 0;
	countfire=0;

}
bool Run_init(HWND window){

	

	/*font = MakeFont("Arial",20);
	debugfont = MakeFont("Arial", 25);*/
	
    //load a image ,fill background
    image = LoadSurface("image\\Space.png");
    if (!image) return false;
	//Menu = LoadSurface("image\\Space.png");
	//if (!Menu) return false;

	//load and init explosion 
	explosion = LoadTexture("image\\explosion_30_128.tga");
	if (!explosion) return false;
	//for(int i =0 ;i <MONSTER1;i++){
	//	Explosion_enemy[i] = false;
	//	timer_enemy[i] = 0.0;
	//	time_shot[i] = rand()%10000;
	//}
	//for(int i =0 ;i <MONSTER2;i++){
	//	Explosion_enemy2[i] = false;
	//	timer_enemy2[i] = 0.0;
	//	//time_shot2[i] = rand()%10000;
	//}
    //load and init player 
    player_ship = LoadTexture("image\\Frame.png");
   /* player.x = 100;
    player.y = 350;
    player.width = 144;
	player.height = 84;
	player.scaling = 0.5f;
	for (int i = 0; i < continues; i++)
	{
		live[i]= true;
	}
	nextt = 0;
	for (int n=0; n<4; n++)
		position_history[n] = D3DXVECTOR2(-100,0);
*/
    //load bullets
	
	purple_fire = LoadTexture("image\\fire.png");
	/*for (int n=0; n<BULLETS; n++)
	{
		bullets[n].alive = false;
		bullets[n].x = 0;
		bullets[n].y = 0;
		bullets[n].width = 94;
		bullets[n].height = 55;

	}
	thunder_fire = LoadTexture("image\\thunder.png");
	for(int i = 0 ;i<8;i++){
		thunderbullets[i].x=0;
		thunderbullets[i].y=0;
		thunderbullets[i].width = 132;
		thunderbullets[i].height =41;
	}*/
	purple_fire1 = LoadTexture("image\\Monsterfire1.png");
	/*for (int n=0; n<BULLETS; n++)
	{
		bullets1[n].alive = false;
		bullets1[n].x = 0;
		bullets1[n].y = 0;
		bullets1[n].width = 63;
		bullets1[n].height = 60;
	}*/
    //load enemy 
	monster1_image = LoadTexture("image\\Monster.png");
	/*for (int n=0; n<MONSTER1; n++)
	{
		monster1[n].alive = true;
		monster1[n].width = 144;
		monster1[n].height = 83;
		monster1[n].x = (float)( 1000 + rand() % BUFFERW );
		monster1[n].y = (float)( rand() % SCREENH );
		monster1[n].velx = (float)((rand() % 4) * -2);
		monster1[n].vely = (float)((rand() % 2) * -2);
		if(monster1[n].velx>=-3)
			monster1[n].velx=-5;
		monster1[n].hp = 1;
		monster1[n].scaling = 0.5f;
	}*/

	monster2_image = LoadTexture("image\\monster2.png");

	/*for (int i=0; i<MONSTER2; i++)
	{
		monster2[i].alive = true;
		monster2[i].width = 156;
		monster2[i].height = 93;
		monster2[i].x = (float)( 1000 + rand() % BUFFERW );
		monster2[i].y = (float)( rand() % SCREENH );
		monster2[i].velx = (float)((rand() % 4) * -2);
		monster2[i].vely = 0.0;
		monster2[i].hp = 1;
		monster2[i].scaling = 0.5f;
	}*/

   //load a bullets upgrade
	fire_level = LoadTexture("image\\fire.png");
	//fire_bullet.alive = false;
	//fire_bullet.x = 0;//SCREENW;
	//fire_bullet.y = 0;//SCREENH;
	//fire_bullet.velx = 0.3;
	//fire_bullet.vely = 0.3;
	//fire_bullet.width = 93;
	//fire_bullet.height = 55;
	//select menu
	text = LoadTexture("image\\select1.png");
	/*text_talk.x = 0;
	text_talk.y = 0;
	text_talk.width = 1920;
	text_talk.height = 318;*/
	//user choose
	selius = LoadTexture("image\\selius.png");
	/*for(int i=0;i<2;i++){
	selius1[i].x = i*120;
	selius1[i].y = 0;
	selius1[i].width = 120;
	selius1[i].height = 115;
	}*/
	cybuster = LoadTexture("image\\Cybuster1.png");
	/*for(int i=0;i<2;i++){
	cybuster1[i].x = 183*i;
	cybuster1[i].y = 0;
	cybuster1[i].width = 183;
	cybuster1[i].height = 125;
	}*/
	// load sound file
	Reset();
	gun = LoadSound("sound\\GUN.wav");
	foom = LoadSound("sound\\dark.wav");
	Dead = LoadSound("sound\\DEAD.wav");
	thunder_sound =LoadSound("sound\\BHIT.wav");
	
	
    
   

	
	//init Menu Start Game
	
	HRESULT result = d3ddev->CreateOffscreenPlainSurface(
		BUFFERW,
		BUFFERH,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&background,
		NULL);
	if (result != D3D_OK) return false;
    
    RECT source_rect = {0, 0, SCREENW, SCREENH };
    RECT dest_ul = { 0, 0, SCREENW, SCREENH };

    d3ddev->StretchRect(
        image, 
        &source_rect, 
        background, 
        &dest_ul, 
        D3DTEXF_NONE);

    
    RECT dest_ur = { SCREENW, 0, SCREENW*2, SCREENH };

    d3ddev->StretchRect(
        image, 
        &source_rect, 
        background, 
        &dest_ur, 
        D3DTEXF_NONE);

    
    d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, 
        &backbuffer);

	GameStart = BEGIN;
    //release image
    image->Release();
	return true;
}

/**
 **
 **/
void Game_End()
{
	font->Release();
	debugfont->Release();
    background->Release();
	purple_fire->Release();
    player_ship->Release();
	purple_fire1->Release();
	monster1_image->Release();
	fire_level->Release();
	delete thunder_sound;
	delete Dead;
	delete foom;
	delete gun;
	delete OPENING;
    DirectSound_Shutdown();
    DirectInput_Shutdown();
    Direct3D_Shutdown();
	
}



void move_player(float movex, float movey)
{
    //cannot move while overloading!
    
	float multi = 4.0f;
    player.x += movex * multi; 
    player.y += movey * multi;
}


/**
 **
 **/
const double PI = 3.1415926535;
const double PI_under_180 = 180.0f / PI;
const double PI_over_180 = PI / 180.0f;

double toRadians(double degrees)
{
    return degrees * PI_over_180;
}

double toDegrees(double radians)
{
    return radians * PI_under_180;
}

double wrap(double value, double bounds)
{
    double result = fmod(value, bounds);
    if (result < 0) result += bounds;
    return result;
}

double wrapAngleDegs(double degs)
{
    return wrap(degs, 360.0);
}

double LinearVelocityX(double angle)
{
    //angle -= 90;
    if (angle < 0) angle = 360 + angle;    
    return cos( angle * PI_over_180);
}

double LinearVelocityY(double angle)
{
    //angle -= 90;
    if (angle < 0) angle = 360 + angle;    
    return sin( angle * PI_over_180);
}




/**
 **
 **/
int find_bullet()
{
    int bullet = -1;
    for (int n=0; n<BULLETS; n++)
    {
        if (!bullets[n].alive) 
        {
            bullet = n;
            break;
        }
    }
    return bullet;
}

int find_bullet_enemy()
{
	int bullet = -1;
	for (int n = 0; n < BULLETS; n++)
	{
		if (!bullets1[n].alive) 
		{
			bullet = n;
			break;
		}
	}
	return bullet;
}
/**
 **
 **/



void player_shoot()
{
	
   /* std::cout<<timeGetTime()<<std::endl;
	std::cout<<player_shoot_timer<<std::endl;
	std::cout<<"===========================\n";*/
    if ((int)timeGetTime() < player_shoot_timer + 100) 
		return;
	else
		player_shoot_timer = timeGetTime();
		PlaySound(gun);
  
    switch(firepower)
    {
        case 1: 
        {
            //create a bullet
			for(int i = 1; i<BULLETS;i++){
            int b1 = find_bullet();
            if (b1 == -1) return;
            bullets[b1].alive = true;
            bullets[b1].rotation = 0.0;
            bullets[b1].velx = 30.0f;
            bullets[b1].vely = 0.0f;
            /*bullets[b1].x = player.x + player.width/1.1;
            bullets[b1].y = player.y + player.height/3;
                - bullets[b1].height/2;*/
			bullets[b1].x = player.x + player.width/1.5;
            bullets[b1].y = player.y + player.height/6.5;
                //- bullets[b1].height/2;
			    //create bullet 1
			    
			
			
			}
        }
        break;
  
    case 5:
    {
		
        //create bullet 1
        int b1 = find_bullet();
        if (b1 == -1) return;
        bullets[b1].alive = true;
        bullets[b1].rotation = 0.0;
        bullets[b1].velx = 12.0f;
        bullets[b1].vely = 0.0f;
        bullets[b1].x = player.x + player.width/2;
        bullets[b1].y = player.y + player.height/2 
            - bullets[b1].height/2;
        bullets[b1].y -= 12;

        //create bullet 2
        int b2 = find_bullet();
        if (b2 == -1) return;
        bullets[b2].alive = true;
        bullets[b2].rotation = 0.0;
        bullets[b2].velx = 12.0f;
        bullets[b2].vely = 0.0f;
        bullets[b2].x = player.x + player.width/2;
        bullets[b2].y = player.y + player.height/2 
            - bullets[b2].height/2;
        bullets[b2].y += 12;

        //create bullet 3
        int b3 = find_bullet();
        if (b3 == -1) return;
        bullets[b3].alive = true;
        bullets[b3].rotation = -4.0;// 86.0;
        bullets[b3].velx = (float) (12.0 * 
            LinearVelocityX( bullets[b3].rotation ));
        bullets[b3].vely = (float) (12.0 * 
            LinearVelocityY( bullets[b3].rotation ));
        bullets[b3].x = player.x + player.width/2;
        bullets[b3].y = player.y + player.height/2 
            - bullets[b3].height/2;
        bullets[b3].y -= 20;

        //create bullet 4
        int b4 = find_bullet();
        if (b4 == -1) return;
        bullets[b4].alive = true;
        bullets[b4].rotation = 4.0;// 94.0;
        bullets[b4].velx = (float) (12.0 * 
            LinearVelocityX( bullets[b4].rotation ));
        bullets[b4].vely = (float) (12.0 * 
            LinearVelocityY( bullets[b4].rotation ));
        bullets[b4].x = player.x + player.width/2;
        bullets[b4].y = player.y + player.height/2 
            - bullets[b4].height/2;
        bullets[b4].y += 20;
    }
    break;
    }
}

void enemy_shoot()
{
	
	if ((int)timeGetTime() < enemy_shoot_timer + 1000) return;
	enemy_shoot_timer = timeGetTime();
    PlaySound(gun);
   
            //create a bullet
			//for(int i = 1; i<BULLETS;i++){
            for (int n=0; n<MONSTER1; n++)
			{
				int b2 = find_bullet_enemy();
				if (b2 == -1) return;
				
					
				//bullets1[b2].alive = true;
				if(monster1[n].alive)
					bullets1[b2].alive = true;
				bullets1[b2].rotation = 0.0;
			    bullets1[b2].velx = 7.0f;
                bullets1[b2].vely = 0.0f;
       
			    bullets1[b2].x = monster1[n].x - 20 ;//- viruses[n].width/1.5;
                bullets1[b2].y = monster1[n].y + monster1[n].height/2.5;
			}
			
      
}
/**
 **
 **/
void Game_Run(HWND window)
{
	clock_t t = clock();
    if (!d3ddev) return;
    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
        D3DCOLOR_XRGB(0,0,100), 1.0f, 0);

    
    currenttime = timeGetTime();
	
    corecount += 1.0;
    if (currenttime > coretime + 20000) 
    {
        corefps = corecount;
        corecount = 0.0;
        coretime = currenttime;
		std::cout<<timeGetTime()<<std::endl;
		for (int n=0; n<MONSTER1; n++)
		{
			if(monster1[n].alive == false){
			monster1[n].alive = true;
			monster1[n].x = (float)( 1000 + rand() % BUFFERW );
			monster1[n].y = (float)( rand() % SCREENH );
			monster1[n].velx = (float)((rand() % 4) * -2);
			monster1[n].vely = (float)((rand() % 2) * -2);
			timer_enemy[n] = 0.0;
			Explosion_enemy[n] = false;
			
			}
			
		}
		for (int i=0; i<MONSTER2; i++)
		{
			if(monster2[i].alive == false){
				monster2[i].alive = true;
				monster2[i].x = (float)( 1000 + rand() % BUFFERW );
				monster2[i].y = (float)( rand() % SCREENH );
				monster2[i].velx = (float)((rand() % 4) * -2);
				monster2[i].vely = 0.0;
				timer_enemy2[i] = 0.0;
				Explosion_enemy2[i] = false;
			}
		}
    }

	
   

    if (currenttime > refresh + 10)
    {
        refresh = currenttime;

        DirectInput_Update();
		cout<<player.x<<endl;
        if (!paused)
        {
            player_state = NORMAL;
			if(Key_Down(DIK_UP) || Key_Down(DIK_DOWN) || Key_Down(DIK_LEFT) || Key_Down(DIK_RIGHT)){
				if(check==true){
					//player_ship = LoadTexture("image\\Frame.png");
					check=false;
				}
				if (Key_Down(DIK_UP) || Key_Down(DIK_W) )
					if(player.y>=0)
						move_player(0,-1);
					else
					{
						player.y=0;
					}	

				if (Key_Down(DIK_DOWN) || Key_Down(DIK_S) )
					if(player.y<=SCREENH-player.height)
						move_player(0,1);
					else
					{
						player.y=SCREENH-player.height;
							
					}

				if (Key_Down(DIK_LEFT) || Key_Down(DIK_A) )
					if(player.x>=0)
						move_player(-1,0);
					else
					{
						player.x=0;
					}

				if (Key_Down(DIK_RIGHT) || Key_Down(DIK_D) )
					if(player.x<=SCREENW-player.width)
						move_player(1,0);
					else
					{
						player.x = SCREENW-player.width;
					}
			}
           
           

            if (Key_Down(DIK_SPACE)){

				
				/*if(player.x<=0 || player.x>=SCREENW-player.width)
				x*=-1;
				if(player.y<=0 || player.y>=SCREENH-player.height)
				y*=-1;
				move_player(x,y);*/
				total+=1;
				std::cout<<total;
				if(check==false){
					player_ship = LoadTexture("image\\Frame1.png");
					check=true;
				}
				countfire++;
				player_shoot();
				/*enemy_shoot();*/

			}
			

			
			
            //update background scrolling
            scrollx += 5.0;
            if (scrolly < 0)
                scrolly = BUFFERH - SCREENH;
            if (scrolly > BUFFERH - SCREENH)
                scrolly = 0;
            if (scrollx < 0)
                scrollx = BUFFERW - SCREENW;
            if (scrollx > BUFFERW - SCREENW)
                scrollx = 0;

            //update virtual scroll position,get time boss appear
            virtual_scrollx+= 5.0;
            if (virtual_scrollx > virtual_level_size)
                virtual_scrollx = 0.0;


            //count bullets used
			bulletcount = 0;
			for (int n=0; n<BULLETS; n++)
			{
				if (bullets[n].alive) 
				{
					bulletcount++;
					bullets[n].x += bullets[n].velx;
					bullets[n].y += bullets[n].vely;

					if (bullets[n].x < 0 || bullets[n].x > SCREENW
						|| bullets[n].y < 0 || bullets[n].y > SCREENH)
						bullets[n].alive = false;
				}
			}
			bulletcount2 = 0;
			for (int n=0; n<BULLETS; n++)
			{
				if (bullets1[n].alive) 
				{
					bulletcount2++;
					bullets1[n].x -= bullets1[n].velx;
					bullets1[n].y -= bullets1[n].vely;

					if (bullets1[n].x < 0 || bullets1[n].x > SCREENW
						|| bullets1[n].y < 0 || bullets1[n].y > SCREENH)
						bullets1[n].alive = false;
				}
			}

            //update enemy viruses
            for (int n=0; n<MONSTER1; n++)
            {
                if (monster1[n].alive) 
                {
                    
					//viruses[n].hp = hpp[n];
                    monster1[n].x += monster1[n].velx;
                    if (monster1[n].x < -144.0f)
                        monster1[n].x = (float)virtual_level_size;
                    if (monster1[n].x > (float)virtual_level_size)
                        monster1[n].x = -144.0f;

                    
                    monster1[n].y += monster1[n].vely;
                    if (monster1[n].y < -83.0f)
                        monster1[n].y = SCREENH;
                    if (monster1[n].y > SCREENH)
                        monster1[n].y = -83.0f;

                }
            }

			//update monster2
			for (int n=0; n<MONSTER2; n++)
			{
				if (monster2[n].alive) 
				{
					
					//viruses[n].hp = hpp[n];
					monster2[n].x += monster2[n].velx;
					if (monster2[n].x < -153.0f)
						monster2[n].x = (float)virtual_level_size;
					if (monster2[n].x > (float)virtual_level_size)
						monster2[n].x = -153.0f;

				
					monster2[n].y += monster2[n].vely;
					if (monster2[n].y < -95.0f)
						monster2[n].y = SCREENH;
					if (monster2[n].y > SCREENH)
						monster2[n].y = -95.0f;

				}
			}
///////////////////////////////////////////////////////////////////////////////////////////////
           

   
        screencount += 1.0;
        if (currenttime > screentime + 1000) 
        {
            screenfps = screencount;
            screencount = 0.0;
            screentime = currenttime;
        }

  
      

    

    }


    //examine every live monster for collision
    for (int v=0; v<MONSTER1; v++)
    {
        if (monster1[v].alive) 
        {
            
            for (int b=0; b<BULLETS; b++)
            {
                if (bullets[b].alive)
                {
					
					
                    if (Collision(monster1[v], bullets[b]))
                    {
						monster1[v].hp--;
						
						bullets[b].alive = false;
						if(monster1[v].hp <= 0){
							Score+=760;
							monster1[v].alive = false;
							//bullets1[v].alive = false;
							destroyx = monster1[v].x;
							destroyy = monster1[v].y;
							Explosion_enemy[v] = true;
													}
                    }
                }
            }

        }
	}

	//// bullet && monster2

	for (int v=0; v<MONSTER2; v++)
	{
		if (monster2[v].alive) 
		{
			
			for (int b=0; b<BULLETS; b++)
			{
				if (bullets[b].alive)
				{


					if (Collision(monster2[v], bullets[b]))
					{
						monster2[v].hp--;
						
						bullets[b].alive = false;
						if(monster2[v].hp <= 0){
							Score+=1200;
							monster2[v].alive = false;
							//bullets1[v].alive = false;
							destroyx = monster2[v].x;
							destroyy = monster2[v].y;
							Explosion_enemy2[v] = true;
						}
					}
				}
			}

		}
	}
	//// 


	//if thunderbullets activities
	for (int v=0; v<MONSTER1; v++)
	{
		if (monster1[v].alive) 
		{
			//test collision with every live bullet
			for (int b=0; b<8; b++)
			{
				if (Collision(monster1[v],thunderbullets[b]))
				{
					monster1[v].hp--;
					//bullets[b].alive = false;
					if(monster1[v].hp <= 0){
						Score+=670;
						monster1[v].alive = false;
						//bullets1[v].alive = false;
						destroyx = monster1[v].x;
						destroyy = monster1[v].y;
						Explosion_enemy[v] = true;
					}
				}
			}
		}

	}
	////thunder && monster2

	for (int v=0; v<MONSTER2; v++)
	{
		if (monster2[v].alive) 
		{
			//test collision with every live bullet
			for (int b=0; b<8; b++)
			{
				if (Collision(monster2[v],thunderbullets[b]))
				{
					monster2[v].hp--;
					//bullets[b].alive = false;
					if(monster2[v].hp <= 0){
						Score+=1200;
						monster2[v].alive = false;
						//bullets1[v].alive = false;
						destroyx = monster2[v].x;
						destroyy = monster2[v].y;
						Explosion_enemy2[v] = true;
					}
				}
			}
		}

	}

	//
	if(live[nextt] && live_player){
		for (int i=0; i<MONSTER1; i++)
			if(Collision(player,monster1[i]) && monster1[i].alive ){
				live[nextt++] = false;
				destroyx = player.x;
				destroyy = player.y;
				player.x = 0;
				player.y = 0;
				live_player = false;
				destroyz = true;

			}
			for (int i=0; i<MONSTER2; i++)
				if(Collision(player,monster2[i]) && monster2[i].alive ){
					live[nextt++] = false;
					destroyx = player.x;
					destroyy = player.y;
					player.x = 0;
					player.y = 0;
					live_player = false;
					destroyz = true;

				}
		for (int b=0; b<BULLETS; b++)
			if (bullets1[b].alive)
			{

				if (Collision(player,bullets1[b]))
				{

						bullets1[b].alive = false;
						live[nextt++] = false;
						destroyx = player.x;
						destroyy = player.y;
						player.x = 0;
						player.y = 0;
						destroyz = true;
						live_player = false;
				}

			}
	}

	if(nextt==continues){
		
		choose = 4;
	}
    } //for



    //draw scrolling background
    RECT source_rect = {
        (long)scrollx, 
        (long)scrolly, 
        (long)scrollx+SCREENW, 
        (long)scrolly+SCREENH 
    };
    RECT dest_rect = { 0, 0, SCREENW, SCREENH};
    d3ddev->StretchRect(background, &source_rect, backbuffer, 
        &dest_rect, D3DTEXF_NONE);


    //begin rendering
    if (d3ddev->BeginScene())
    {
        spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

        switch(player_state)
        {
            case NORMAL: 
            {
                
				if(live_player){
                if (player_state_previous != player_state)
                {
                    for (int n=0; n<8; n++)
                    {
                        position_history[n].x = player.x;
                        position_history[n].y = player.y;
                    }
                }

                D3DCOLOR shadowcolor = D3DCOLOR_ARGB( 250, 255, 240, 240 );

                if (currenttime > position_history_timer + 40)
                {
                    position_history_timer = currenttime;
                    position_history_index++;
                    if (position_history_index > 7)
                    {
                        position_history_index = 7;
                        for (int a=1; a<8; a++)
                            position_history[a-1] = position_history[a];
                    }

                    position_history[position_history_index].x = player.x;
                    position_history[position_history_index].y = player.y;
                }

                for (int n=0; n<8; n++)
                {
                    shadowcolor = D3DCOLOR_ARGB( 20 + n*10, 0, 240, 240 );

                    
                    Sprite_Transform_Draw( 
                        player_ship, 
                        (int)position_history[n].x,
                        (int)position_history[n].y,
                        player.width, 
                        player.height, 
                        0, 1, 0.0f, 1.0f, 
                        shadowcolor);
                }

                D3DCOLOR shipcolor = D3DCOLOR_ARGB(255, 255, 255, 255);
                Sprite_Transform_Draw(
                    player_ship, 
                    (int)player.x, 
                    (int)player.y, 
                    player.width, 
                    player.height, 
                    0, 1, 0.0f, 1.0f, 
                    shipcolor);
			

            }
			else{
				sum++;
				if(sum%3==0){
					D3DCOLOR shipcolor = D3DCOLOR_ARGB(255, 255, 255, 255);
					Sprite_Transform_Draw(
						player_ship, 
						(int)player.x, 
						(int)player.y, 
						player.width, 
						player.height, 
						0, 1, 0.0f, 1.0f, 
						shipcolor);
				}
				if(sum%500==0)
					live_player = true;
			}
            break;
        }
		}
       
        player_state_previous = player_state;


        for (int n=0; n<MONSTER1; n++)
		{

			//is this virus sprite alive?
			if (monster1[n].alive)
			{
				//is this virus sprite visible on the screen?
				time_shot[n] += (rand()%1000 - rand()%1000);

				if (monster1[n].x > -144.0f && monster1[n].x < SCREENW)
				{

					Sprite_Transform_Draw( 
						monster1_image,
						(int)monster1[n].x, 
						(int)monster1[n].y,
						monster1[n].width, 
						monster1[n].height,
						0, 1, 0.0f, 1.0f, 
						color);

				}
				if(time_shot[n]>20000 && (monster1[n].x>0 && monster1[n].x<SCREENW && monster1[n].y>0 && monster1[n].y<SCREENH)){
					enemy_shoot();
					time_shot[n] = rand()%10000;
				}

			}

		}


		for (int n=0; n<MONSTER2; n++)
		{

			
			if (monster2[n].alive)
			{
				
				/*time_shot[n] += (rand()%1000 - rand()%1000);*/

				if (monster2[n].x > -153.0f && monster2[n].x < SCREENW)
				{

					Sprite_Transform_Draw( 
						monster2_image,
						(int)monster2[n].x, 
						(int)monster2[n].y,
						monster2[n].width, 
						monster2[n].height,
						0, 1, 0.0f, 1.0f, 
						color);

				}

				/*if(time_shot[n]>20000 && (viruses[n].x>0 && viruses[n].x<SCREENW && viruses[n].y>0 && viruses[n].y<SCREENH)){
					enemy_shoot();
					time_shot[n] = rand()%10000;
				}*/

			}

		}

        //draw bullets
		
		for (int n=0; n<BULLETS; n++)
		{
		if (bullets[n].alive) 
		{

		Sprite_Transform_Draw( 
		purple_fire,
		(int)bullets[n].x, 
		(int)bullets[n].y,
		bullets[n].width, 
		bullets[n].height,
		0, 1, 
		(float)toRadians(bullets[n].rotation), 
		1.0f, 
		color);
		}
		}
		if(Key_Down(DIK_V)){
		
		
			int x = player.x,y=0;
			for (int i = 0; i < 8; i++)
			{
				thunderbullets[i].x=player.x + player.width+120*i;
				thunderbullets[i].y=player.y+20;
				PlaySound(thunder_sound);
				Sprite_Animate(frame, 0, 4, 1, starttime, frame);
				Sprite_Draw_Frame(thunder_fire,thunderbullets[i].x,thunderbullets[i].y,frame, 130, 40, 1);
				
			}	
		}
			
		

	
		for (int n=0; n<BULLETS; n++)
		{
			
			if (bullets1[n].alive) 
			{
				Sprite_Transform_Draw( 
					purple_fire1,
					(int)bullets1[n].x, 
					(int)bullets1[n].y,
					bullets1[n].width, 
					bullets1[n].height,
					0, 1, 
					(float)toRadians(bullets1[n].rotation), 
					1.0f, 
					color);
			}
		}
 //   if(total<300){
 //   //total +=1;
	//string shot = "screen_"+std::to_string(total)+".bmp";
	//purple_fire = LoadTexture("shots\\"+shot);
	//
	//Sprite_Transform_Draw( 
	//	purple_fire,
	//	(int)0, 
	//	(int)0,
	//	486, 
	//	668,
	//	0, 1, 
	//	(float)toRadians(0), 
	//	1.0f, 
	//	color);
	//}

		if(virtual_scrollx>1000){
			fire_bullet.alive = true;
			if(fire_bullet.alive){

				
				
				fire_bullet.x += rand()%5/float(10);
				fire_bullet.y += rand()%5/float(10);
				if(fire_bullet.y<=0 || fire_bullet.y>=SCREENH){
					fire_bullet.vely *= -1;
				}
				Sprite_Transform_Draw( 
					fire_level,
					(int)fire_bullet.x+1, 
					(int)fire_bullet.y+1,
					fire_bullet.width, 
					fire_bullet.height,
					0, 1, 
					(float)toRadians(fire_bullet.rotation), 
					1.0f, 
					color);
			}
		}
		
       
        //detroy monster 1
		for(int i =0;i<MONSTER1;i++){
			
			if(Explosion_enemy[i] && timer_enemy[i]<2.0){
				Sprite_Animate(frame, 0, 29, 1, starttime, 30);
				Sprite_Draw_Frame(explosion,monster1[i].x,monster1[i].y, frame, 128, 128, 6);
				
				PlaySound(Dead);
				timer_enemy[i] += 0.01;
			}
		}

		//detroy monster 2
		for(int i =0;i<MONSTER2;i++){

			if(Explosion_enemy2[i] && timer_enemy2[i]<2.0){
				Sprite_Animate(frame, 0, 29, 1, starttime, 30);
				Sprite_Draw_Frame(explosion,monster2[i].x,monster2[i].y, frame, 128, 128, 6);

				PlaySound(Dead);
				timer_enemy2[i] += 0.01;
			}
		}

		if(destroyz){
		timer += 0.01;
		Sprite_Animate(frame, 0, 29, 1, starttime, 30);
		Sprite_Draw_Frame(explosion,destroyx,destroyy, frame, 128, 128, 6);
		if(timer>2.0){
			destroyz = false;
			timer = 0.0;
		}
		}
		int y = SCREENH-12;
		FontPrint(debugfont, 0,0,"Live = " +  std::to_string(continues-nextt), color);

		FontPrint(debugfont, 0, y-12, 
			"Score = " + std::to_string(Score)
			+ "p" ,color);

		
        spriteobj->End();

        d3ddev->EndScene();
        d3ddev->Present(NULL, NULL, NULL, NULL);
    }
	
	

}

void Init_Menu(HWND window){

	    DirectInput_Update();
		d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, 
			&backbuffer);


		
		
		if(d3ddev->BeginScene()){
		
		if(playopening){
			PlaySound(OPENING);
			playopening = false;
		}
		d3ddev->StretchRect(Menu, NULL, backbuffer, NULL, D3DTEXF_NONE);

		if (Key_Down(DIK_SPACE) && menuchange==1)
		{
			//delete Selectmenu;
			//delete OPENING;
			StopSound(OPENING);
			choose=2;
			if(GameStart == END)
				Run_init(window);
		}
		else if(Key_Down(DIK_SPACE) && menuchange==2){
			StopSound(OPENING);
			choose=3;
		}
		else if (Key_Down(DIK_SPACE) && menuchange==3)
		{
			
			exit(0);
		}
		
		cout<<menuchange;
		if (Key_Down(DIK_DOWN))
		{
			if(menuchange <3)
				menuchange +=1;
			loadbackgroundmenu(std::to_string(menuchange));
			newgame =false;
		}

		if (Key_Down(DIK_UP))
		{
			if(menuchange >0)
				menuchange -=1;
			loadbackgroundmenu(std::to_string(menuchange)); 
			newgame = true;
		
		}
		
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
		}
}
void Choose_Menu(HWND window){


	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, 
		&backbuffer);
	
	DirectInput_Update();
	if(Key_Down(DIK_LEFT)){
		PlaySound(Selectmenu);
		selius_vt = 1;
		cybuster_vt = 0;
		name = "Vanbeo";
	}

	if(Key_Down(DIK_RIGHT)){
		PlaySound(Selectmenu);
		selius_vt = 0;
		cybuster_vt = 1;
		name = "Masaki";
	}
	if(Key_Down(DIK_RETURN) && selius_vt == 1)
		choose = 5;

	if(d3ddev->BeginScene()){

		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		d3ddev->StretchRect(Menu, NULL, backbuffer, NULL, D3DTEXF_NONE);
		
		
		D3DXLoadSurfaceFromFile(
				Menu,            
				NULL,               
				NULL,               
				"image\\select.png",    
				NULL,               
				D3DX_DEFAULT,       
				0,                 
				NULL); 

		

		Sprite_Transform_Draw( 
			selius,
			300, 
			200,
			selius1[selius_vt].x,
			selius1[selius_vt].y,
			selius1[selius_vt].width, 
			selius1[selius_vt].height,
			0,1, 
			(float)toRadians(0), 
			1.0f,1.0f, 
			color);

		Sprite_Transform_Draw( 
			cybuster,
			500, 
			200,
			cybuster1[cybuster_vt].x,
			cybuster1[cybuster_vt].y,
			cybuster1[cybuster_vt].width, 
			cybuster1[cybuster_vt].height,
			0,1, 
			(float)toRadians(0), 
			1.0f,1.0f, 
			color);

		 
		Sprite_Transform_Draw( 
			text,
			(int)text_talk.x, 
			(int)(SCREENH-(text_talk.height*0.52f)),
			text_talk.width, 
			text_talk.height,
			0,1, 
			(float)toRadians(0), 
			0.52f, 
			color);
		Sprite_Transform_Draw( 
			LoadTexture("image\\"+name+".png"),
			(int)text_talk.x+70, 
			(int)(SCREENH-(text_talk.height*0.47f)),
			111, 
			110,
			0,1, 
			(float)toRadians(0), 
			1.01f, 
			color);
		FontPrint(debugfont, 150, 26, 
			"SYSTEM INFORMATION ",color);

		FontPrint(font,text_talk.x+300,SCREENH-(text_talk.height*0.43f), 
			name,color);
		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}
}
void Game_Over(HWND window){

	if(GOSH < SCREENH - 70)
		PlaySound(GO);
	DirectInput_Update();
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, 
		&backbuffer);
	if(Key_Down(DIK_RETURN)){
		
		/*choose = 1;
		PlaySound(OPENING);
		menuchange = 1;
		GOSH = SCREENH;
		loadbackgroundmenu(std::to_string(menuchange));
		Reset();*/
	}
	if(d3ddev->BeginScene()){

		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		d3ddev->StretchRect(Menu, NULL, backbuffer, NULL, D3DTEXF_NONE);
		D3DXLoadSurfaceFromFile(
			Menu,            
			NULL,               
			NULL,               
			"image\\Rank.png",    
			NULL,               
			D3DX_DEFAULT,       
			0,                 
			NULL);

		
		if(GOSH > SCREENH/2-50){
			FontPrint(Largefont,SCREENW/4,GOSH,"GAMEOVER",color);
			GOSH-=6;
		}
		else{
			Delay++;
			FontPrint(Largefont,SCREENW/4,SCREENH/2-50,"GAMEOVER",color);
		}
		if(Delay > 70)
			choose = 6;
		

		
		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}	
}

void Rank_Menu(HWND window){
	DirectInput_Update();
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, 
		&backbuffer);

	if(d3ddev->BeginScene()){

		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		d3ddev->StretchRect(Menu, NULL, backbuffer, NULL, D3DTEXF_NONE);
		D3DXLoadSurfaceFromFile(
			Menu,            
			NULL,               
			NULL,               
			"image\\Rank.png",    
			NULL,               
			D3DX_DEFAULT,       
			0,                 
			NULL);
	
		if(Key_Down(DIK_ESCAPE)){
			choose = 1;
			PlaySound(OPENING);
			menuchange = 1;
			loadbackgroundmenu(std::to_string(menuchange));
		}

		FontPrint(debugfont,200,100,"RANK",color);
		FontPrint(debugfont,450,100,"PLAYER",color);
		FontPrint(debugfont,700,100,"SCORE",color);
		for(auto it=0;it<6;it++)  
		{  
			for(int i=0;i<
				odbc.GetColCount();i++)  
			{  
				if(i==0)
					FontPrint(font,i*250+200,it*50+200,to_string(it+1),color);
				else
				{
					string p = vv[it][i];
					cout<<it<<" "<<i<<endl;
					FontPrint(font,i*250+200,it*50+200,p,color);
				}
			}
		} 
	spriteobj->End();
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
    }	
}
void InputName(HWND window){


	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, 
		&backbuffer);
	if(Key_Down(DIK_RETURN)){
		double HighScore = Score*(1+live_player/10)+50*(timeplay/CLOCKS_PER_SEC);
		std::string sql = "insert into Records(Name,Score) values ('"+z->getTextbox()+"',"+std::to_string(HighScore)+")";
		odbc.ExecuteNonQuery(sql.c_str());
		vv = odbc.ExecuteQueryVector("select * from Records order by Score desc");
		PlaySound(OPENING);
		menuchange = 1;
		GOSH = SCREENH;
		loadbackgroundmenu(std::to_string(menuchange));
		Reset();
		choose = 1;
		
	}
	DirectInput_Update();
	if(d3ddev->BeginScene()){

		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		d3ddev->StretchRect(Menu, NULL, backbuffer, NULL, D3DTEXF_NONE);
		z->render();
		
		spriteobj->End();
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}
}
void Game_Start(HWND window){
	//DirectInput_Init(window);
	switch (choose)
	{
	case 6: InputName(window);
		break;
	case 5:
		if(startgame){
		timeplay = clock();
		startgame = false;
		}
		Game_Run(window);
		break;
	case 4:
		if(!startgame){
			timeplay = clock() - timeplay;
			startgame = true;
			
		}
		Game_Over(window);
		break;
	case 3:
		Rank_Menu(window);
		break;
	case 2:
		//PlaySound(foom);
		Choose_Menu(window);
		//Game_Run(window);
		break;
	case 1:
		Init_Menu(window);
		break;
	}
}
//sub function
int getcountfire(){
	return countfire;
}

void loadbackgroundmenu(std::string s){
	PlaySound(Selectmenu);
	std::string img = "image\\Menu"+s+".png";
	D3DXLoadSurfaceFromFile(
		Menu,            
		NULL,               
		NULL,               
		img.c_str(),    
		NULL,               
		D3DX_DEFAULT,       
		0,                 
		NULL); 
	
}
ZGame::MainFrame * getCE(){
	return z;
}
//void WindownProc(UINT msg,WPARAM wParam, LPARAM lParam){
//	z->WindownProcCE(msg,wParam,lParam);
//}
