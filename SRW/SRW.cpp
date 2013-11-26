
#include "SRW.h"
#include "Sound.h"

using namespace std;

//Direct3D 
LPDIRECT3D9 d3d = NULL; 
LPDIRECT3DDEVICE9 d3ddev = NULL; 
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPD3DXSPRITE spriteobj = NULL;

//DirectInput 
LPDIRECTINPUT8 dinput = NULL;
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
DIMOUSESTATE mouse_state;
char keys[256];
XINPUT_GAMEPAD controllers[4];

//Sound
CSoundManager *dsound = NULL;

bool Direct3D_Init(HWND window, int width, int height, bool fullscreen)
{
    
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return false;

    
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.hDeviceWindow = window;
    d3dpp.Windowed = (!fullscreen);
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil = 1;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;

    d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
    if (!d3ddev) return false;


    
    d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

   
    D3DXCreateSprite(d3ddev, &spriteobj);

    return 1;
}

void Direct3D_Shutdown()
{
    if (spriteobj) spriteobj->Release();

    if (d3ddev) d3ddev->Release();
    if (d3d) d3d->Release();
}
LPDIRECT3DTEXTURE9 LoadTexture(std::string filename, D3DCOLOR transcolor)
{  
	LPDIRECT3DTEXTURE9 texture = NULL;

	
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return NULL;

	D3DXCreateTextureFromFileEx( 
		d3ddev,                
		filename.c_str(),     
		info.Width,            
		info.Height,           
		1,                     
		D3DPOOL_DEFAULT,       
		D3DFMT_UNKNOWN,    
		D3DPOOL_DEFAULT,       
		D3DX_DEFAULT,         
		D3DX_DEFAULT,          
		transcolor,           
		&info,                 
		NULL,                 
		&texture );            


	if (result != D3D_OK) return NULL;

	return texture;
}
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
    
    D3DSURFACE_DESC desc;
    source->GetDesc(&desc);

 
    RECT source_rect = {0, 0, (long)desc.Width, (long)desc.Height };
    RECT dest_rect = { (long)x, (long)y, (long)x+desc.Width, (long)y+desc.Height};
    
    
    d3ddev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);

}

LPDIRECT3DSURFACE9 LoadSurface(string filename)
{
    LPDIRECT3DSURFACE9 image = NULL;
    
    
    D3DXIMAGE_INFO info; 
    HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
    if (result != D3D_OK)
        return NULL;

    //khoi tao surface
    result = d3ddev->CreateOffscreenPlainSurface(
        info.Width,         
        info.Height,        
        D3DFMT_X8R8G8B8, 
        D3DPOOL_DEFAULT,    
        &image,             
        NULL);              

    if (result != D3D_OK) return NULL;

    //load surface tu file
    result = D3DXLoadSurfaceFromFile(
        image,                 
        NULL,                  
        NULL,                   
        filename.c_str(),      
        NULL,                   
        D3DX_DEFAULT,          
        D3DCOLOR_XRGB(0,0,0),   
        NULL);                  

    
    if (result != D3D_OK) return NULL;

    return image;
}


void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, int framenum, int framew, int frameh, int columns)
{
    D3DXVECTOR3 position( (float)destx, (float)desty, 0 );
    D3DCOLOR white = D3DCOLOR_XRGB(255,255,255);

    RECT rect;
     rect.left = (framenum % columns) * framew;
    rect.top = (framenum / columns) * frameh;
    rect.right = rect.left + framew;
    rect.bottom = rect.top + frameh;

    spriteobj->Draw( texture, &rect, NULL, &position, white);
}

void Sprite_Animate(int &frame, int startframe, int endframe, int direction, int &starttime, int delay)
{
    if ((int)GetTickCount() > starttime + delay)
    {
        starttime = GetTickCount();

        frame += direction;
        if (frame > endframe) frame = startframe;
        if (frame < startframe) frame = endframe;
    }    
}

void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y,int fx,int fy, int width, int height, 
						   int frame, int columns, float rotation, float scaleW, float scaleH, D3DCOLOR color)
{

	D3DXVECTOR2 scale( scaleW, scaleH );

	D3DXVECTOR2 trans( (float)x, (float)y );


	D3DXVECTOR2 center( (float)( width * scaleW )/2, (float)( height * scaleH )/2);


	D3DXMATRIX mat;
	D3DXMatrixTransformation2D( &mat, NULL, 0, &scale, &center, rotation, &trans );

	spriteobj->SetTransform( &mat );

	RECT srcRect = {fx, fy, fx + width, fy + height};
	spriteobj->Draw( image, &srcRect, NULL, NULL, color );


	D3DXMatrixIdentity( &mat );
	spriteobj->SetTransform( &mat );
}

void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, 
    int frame, int columns, float rotation, float scaleW, float scaleH, D3DCOLOR color)
{
  
    D3DXVECTOR2 scale( scaleW, scaleH );

    D3DXVECTOR2 trans( (float)x, (float)y );

    
    D3DXVECTOR2 center( (float)( width * scaleW )/2, (float)( height * scaleH )/2);

    
    D3DXMATRIX mat;
    D3DXMatrixTransformation2D( &mat, NULL, 0, &scale, &center, rotation, &trans );

    
    spriteobj->SetTransform( &mat );
	int fx = (frame % columns) * width;
    int fy = (frame / columns) * height;
    RECT srcRect = {fx, fy, fx + width, fy + height};

   
    spriteobj->Draw( image, &srcRect, NULL, NULL, color );


    D3DXMatrixIdentity( &mat );
    spriteobj->SetTransform( &mat );
}


void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height, 
    int frame, int columns, float rotation, float scaling, D3DCOLOR color)
{
    Sprite_Transform_Draw( image, x, y, width, height, frame, columns, rotation, scaling, scaling, color);
}


int Collision(SPRITE sprite1, SPRITE sprite2)
{
    RECT rect1;
    rect1.left = (long)sprite1.x;
    rect1.top = (long)sprite1.y;
    rect1.right = (long)(sprite1.x + sprite1.width * sprite1.scaling);
    rect1.bottom = (long)(sprite1.y + sprite1.height * sprite1.scaling);

    RECT rect2;
    rect2.left = (long)sprite2.x;
    rect2.top = (long)sprite2.y;
    rect2.right = (long)(sprite2.x + sprite2.width * sprite2.scaling);
    rect2.bottom = (long)(sprite2.y + sprite2.height * sprite2.scaling);

    RECT dest; 
    return IntersectRect(&dest, &rect1, &rect2);
}

bool CollisionD(SPRITE sprite1, SPRITE sprite2)
{
    double radius1, radius2;


    if (sprite1.width > sprite1.height)
        radius1 = (sprite1.width * sprite1.scaling) / 2.0;
    else
        radius1 = (sprite1.height * sprite1.scaling) / 2.0;


    double x1 = sprite1.x + radius1;
    double y1 = sprite1.y + radius1;
    D3DXVECTOR2 vector1( (float)x1, (float)y1);

  
    if (sprite2.width > sprite2.height)
        radius2 = (sprite2.width * sprite2.scaling) / 2.0;
    else
        radius2 = (sprite2.height * sprite2.scaling) / 2.0;

    double x2 = sprite2.x + radius2;
    double y2 = sprite2.y + radius2;
    D3DXVECTOR2 vector2( (float)x2, (float)y2);

    
    double deltax = vector1.x - vector2.x;
    double deltay = vector2.y - vector1.y;
    double dist = sqrt((deltax * deltax) + (deltay * deltay));

   
    return (dist < radius1 + radius2);
}

bool DirectInput_Init(HWND hwnd)
{
   
    DirectInput8Create(
        GetModuleHandle(NULL), 
        DIRECTINPUT_VERSION, 
        IID_IDirectInput8,
        (void**)&dinput,
        NULL);

    
    dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
    dikeyboard->SetDataFormat(&c_dfDIKeyboard);
    dikeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    dikeyboard->Acquire();

  
    dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
    dimouse->SetDataFormat(&c_dfDIMouse);
    dimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    dimouse->Acquire();
    d3ddev->ShowCursor(false);

    return true;
}

void DirectInput_Update()
{
    
    dimouse->Poll();
    if (!SUCCEEDED(dimouse->GetDeviceState(sizeof(DIMOUSESTATE),&mouse_state)))
    {
       
        dimouse->Acquire();
    }

   
    dikeyboard->Poll();
    if (!SUCCEEDED(dikeyboard->GetDeviceState(256,(LPVOID)&keys)))
    {
        
        dikeyboard->Acquire();
    }

  
    for (int i=0; i< 4; i++ )
    {
        ZeroMemory( &controllers[i], sizeof(XINPUT_STATE) );

        
        XINPUT_STATE state;
        DWORD result = XInputGetState( i, &state );

        
        if (result == 0) controllers[i] = state.Gamepad;
    }
}


int Mouse_X()
{
    return mouse_state.lX;
}

int Mouse_Y()
{
    return mouse_state.lY;
}

int Mouse_Button(int button)
{
    return mouse_state.rgbButtons[button] & 0x80;
}

bool Key_Down(int key) 
{
    return (bool)(keys[key] & 0x80);
}
bool Key_Up(int key) 
{
	return (bool)(keys[key] & 0x80);
}

void DirectInput_Shutdown()
{
    if (dikeyboard) 
    {
        dikeyboard->Unacquire();
        dikeyboard->Release();
        dikeyboard = NULL;
    }
    if (dimouse) 
    {
        dimouse->Unacquire();
        dimouse->Release();
        dimouse = NULL;
    }
}

LPD3DXFONT MakeFont(string name, int size)
{
	LPD3DXFONT font = NULL;

	D3DXFONT_DESC desc = {
		size,                   //height
		0,                      //width
		0,                      //weight
		0,                      //miplevels
		false,                  //italic
		DEFAULT_CHARSET,        //charset
		OUT_TT_PRECIS,          //output precision
		CLIP_DEFAULT_PRECIS,    //quality
		DEFAULT_PITCH,          //pitch and family
		""                      //font name
	};

	strcpy(desc.FaceName, name.c_str());

	D3DXCreateFontIndirect(d3ddev, &desc, &font);

	return font;
}

void FontPrint(LPD3DXFONT font, int x, int y, string text, D3DCOLOR color)
{
	//figure out the text boundary
	RECT rect = { x, y, 0, 0 };
	font->DrawText( NULL, text.c_str(), text.length(), &rect, DT_CALCRECT, color); 

	//print the text
	font->DrawText(spriteobj, text.c_str(), text.length(), &rect, DT_LEFT, color); 
}
/*
functions Sound Ouput
*/

bool DirectSound_Init(HWND hwnd)
{
   
    dsound = new CSoundManager();

    
    HRESULT result;
    result = dsound->Initialize(hwnd, DSSCL_PRIORITY);
    if (result != DS_OK) return false;

    
    result = dsound->SetPrimaryBufferFormat(2, 22050, 16);
    if (result != DS_OK) return false;

    
    return true;
}

void DirectSound_Shutdown()
{
    if (dsound) delete dsound;
}

CSound *LoadSound(string filename)
{
    HRESULT result;

   
    CSound *wave = NULL;

    
    char s[255];
    sprintf(s, "%s", filename.c_str());
    result = dsound->Create(&wave, s);
    if (result != DS_OK) wave = NULL;

    
    return wave;
}

void PlaySound(CSound *sound)
{
    sound->Play();
}

void LoopSound(CSound *sound)
{
    sound->Play(0, DSBPLAY_LOOPING);
}

void StopSound(CSound *sound)
{
    sound->Stop();
}




/**
 *  Mesh loading and rendering code
 **/

void SetCamera(float posx, float posy, float posz, float lookx, float looky, float lookz)
{
    float fov = D3DX_PI / 4.0;
    float aspectRatio = SCREENW / SCREENH;
    float nearRange = 1.0;
    float farRange = 2000.0;
    D3DXVECTOR3 updir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXVECTOR3 position = D3DXVECTOR3(posx, posy, posz);
    D3DXVECTOR3 target = D3DXVECTOR3(lookx, looky, lookz);

    //set the perspective 
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, fov, aspectRatio, nearRange, farRange);
    d3ddev->SetTransform(D3DTS_PROJECTION, &matProj);

    //set up the camera view matrix
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView, &position, &target, &updir);
    d3ddev->SetTransform(D3DTS_VIEW, &matView);
}


void SplitPath(const string& inputPath, string* pathOnly, string* filenameOnly)
{
    string fullPath( inputPath );
    replace( fullPath.begin(), fullPath.end(), '\\', '/');
    string::size_type lastSlashPos = fullPath.find_last_of('/');

    // check for there being no path element in the input
    if (lastSlashPos == string::npos)
    {
        *pathOnly="";
        *filenameOnly = fullPath;
    }
    else {
        if (pathOnly) {
            *pathOnly = fullPath.substr(0, lastSlashPos);
        }
        if (filenameOnly)
        {
            *filenameOnly = fullPath.substr( 
                lastSlashPos + 1,
                fullPath.size() - lastSlashPos - 1 );
        }
    }
}

bool DoesFileExist(const string &filename)
{
    return (_access(filename.c_str(), 0) != -1);
}

bool FindFile(string *filename)
{
    if (!filename) return false;

    //look for file using original filename and path
    if (DoesFileExist(*filename)) return true;

    //since the file was not found, try removing the path
    string pathOnly;
    string filenameOnly;
    SplitPath(*filename,&pathOnly,&filenameOnly);

    //is file found in current folder, without the path?
    if (DoesFileExist(filenameOnly))
    {
        *filename=filenameOnly;
        return true;
    }

    //not found
    return false;
}

MODEL *LoadModel(string filename)
{
    MODEL *model = (MODEL*)malloc(sizeof(MODEL));
    LPD3DXBUFFER matbuffer;
    HRESULT result;

    //load mesh from the specified file
    result = D3DXLoadMeshFromX(
        filename.c_str(),               //filename
        D3DXMESH_SYSTEMMEM,     //mesh options
        d3ddev,                 //Direct3D device
        NULL,                   //adjacency buffer
        &matbuffer,             //material buffer
        NULL,                   //special effects
        &model->material_count, //number of materials
        &model->mesh);          //resulting mesh

    if (result != D3D_OK)
    {
        MessageBox(0, "Error loading model file", APPTITLE.c_str(), 0);
        return NULL;
    }

    //extract material properties and texture names from material buffer
    LPD3DXMATERIAL d3dxMaterials = (LPD3DXMATERIAL)matbuffer->GetBufferPointer();
    model->materials = new D3DMATERIAL9[model->material_count];
    model->textures  = new LPDIRECT3DTEXTURE9[model->material_count];

    //create the materials and textures
    for(DWORD i=0; i<model->material_count; i++)
    {
        //grab the material
        model->materials[i] = d3dxMaterials[i].MatD3D;

        //set ambient color for material 
        model->materials[i].Ambient = model->materials[i].Diffuse;

        model->textures[i] = NULL;
        if (d3dxMaterials[i].pTextureFilename != NULL) 
        {
            string filename = d3dxMaterials[i].pTextureFilename;
            if( FindFile(&filename) )
            {
                result = D3DXCreateTextureFromFile(d3ddev, filename.c_str(), &model->textures[i]);
                if (result != D3D_OK) 
                {
                    MessageBox(NULL, "Could not find texture file", "Error", MB_OK);
                    return false;
                }
            }
        }
    }

    //done using material buffer
    matbuffer->Release();

    return model;
}

void DeleteModel(MODEL *model)
{
    //remove materials from memory
    if( model->materials != NULL ) 
        delete[] model->materials;

    //remove textures from memory
    if (model->textures != NULL)
    {
        for( DWORD i = 0; i < model->material_count; i++)
        {
            if (model->textures[i] != NULL)
                model->textures[i]->Release();
        }
        delete[] model->textures;
    }
    
    //remove mesh from memory
    if (model->mesh != NULL)
        model->mesh->Release();

    //remove model struct from memory
    if (model != NULL)
        free(model);
    
}


void DrawModel(MODEL *model)
{
    //any materials in this mesh?
    if (model->material_count == 0) 
    {
        model->mesh->DrawSubset(0);
    }
    else {
        //draw each mesh subset
        for( DWORD i=0; i < model->material_count; i++ )
        {
            // Set the material and texture for this subset
            d3ddev->SetMaterial( &model->materials[i] );

            if (model->textures[i]) 
            {
                if (model->textures[i]->GetType() == D3DRTYPE_TEXTURE) 
                {
                    D3DSURFACE_DESC desc;
                    model->textures[i]->GetLevelDesc(0, &desc);
                    if (desc.Width > 0) 
                    {
                        d3ddev->SetTexture( 0, model->textures[i] );
                    }
                }
            }
            
            // Draw the mesh subset
            model->mesh->DrawSubset( i );
        }
    }
}




