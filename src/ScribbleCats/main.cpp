#include <memory>
#include <foundation\memory.h>

#include <hge\hge.h>
#include <hge\hgeresource.h>
#include <hge\hgefont.h>
#include <hge\hgegui.h>

#include "World.h"
#include "Tileset.h"
#include "Cat.h"
#include <cassert>

using namespace foundation;
using namespace Scribble;

HGE* g_Hge = NULL;
Tileset* g_Tileset = NULL;
World* g_World = NULL;
hgeFont* g_Font = NULL;

bool FrameFunc();
bool RenderFunc();

int g_ScreenWidth = 1280;
int g_ScreenHeight = 720;

Cat* g_Cat = NULL;

short* LoadTileLayer( const char* FromFile );

INT WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	foundation::memory_globals::init();

	g_Hge = hgeCreate(HGE_VERSION);

	g_Hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	g_Hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	g_Hge->System_SetState(HGE_TITLE, "Scribbor catzorz");
	
	g_Hge->System_SetState(HGE_SCREENWIDTH, g_ScreenWidth );
	g_Hge->System_SetState(HGE_SCREENHEIGHT, g_ScreenHeight );
	
	g_Hge->System_SetState(HGE_INIFILE, "../Media/Config.ini" );
	
	g_Hge->System_SetState(HGE_FPS, 60);
	
	g_Hge->System_SetState(HGE_WINDOWED, true);
	g_Hge->System_SetState(HGE_USESOUND, true);
	g_Hge->System_SetState(HGE_SHOWSPLASH, false);

	g_Hge->System_SetState(HGE_HIDEMOUSE, false);

	if(g_Hge->System_Initiate())
	{
		g_Font = MAKE_NEW( memory_globals::default_allocator(), hgeFont, "../Media/Fonts/font1.fnt" );

		g_World = MAKE_NEW( memory_globals::default_allocator(), World );
		g_Tileset = MAKE_NEW( memory_globals::default_allocator(), Tileset, 32, 0, 0 );
		g_Cat = g_World->Spawn<Cat>( hgeVector( 180, 100 ) );
		g_Tileset->AddLayer( 100, 100, LoadTileLayer( "CollisionTest.MAR" ), g_Hge->Texture_Load( "../Media/Textures/Tileset.png" ) ); 

		g_Hge->System_Start();
	}
	else
	{	
		// If HGE initialization failed show error message
		MessageBoxA(NULL, g_Hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
	}

	g_World->ClearActorList();

	MAKE_DELETE( memory_globals::default_allocator(), hgeFont, g_Font );
	MAKE_DELETE( memory_globals::default_allocator(), World, g_World );
	MAKE_DELETE( memory_globals::default_allocator(), Tileset, g_Tileset );

	g_Hge->Release();

	foundation::memory_globals::shutdown();
	
	return EXIT_SUCCESS;
}

bool FrameFunc()
{
	float DeltaTime = g_Hge->Timer_GetDelta();

	g_World->Tick( DeltaTime );
	g_Tileset->Tick( 1280/2, 720/2 );
		
	return false;
}

bool RenderFunc()
{
	float DeltaTime = g_Hge->Timer_GetDelta();
	
	g_Hge->Gfx_BeginScene();
	g_Hge->Gfx_Clear( ARGB( 0, 0, 0, 0 ) );

	g_Tileset->Render();
	g_World->Render();

	g_Font->printf( 10, 10, HGETEXT_LEFT, "FPS: %i", g_Hge->Timer_GetFPS() );

	g_Hge->Gfx_EndScene();

	return false;
}

short* LoadTileLayer( const char* FromFile )
{
	static char NameBuffer[1024];
	sprintf( NameBuffer, "../Media/Maps/%s", FromFile );
	FILE* Map = fopen( NameBuffer, "r" );

	assert( Map != NULL && "Failed to load map" );

	// obtain file size:
	fseek( Map, 0, SEEK_END);
	long int FileSize = ftell( Map );
	rewind( Map );

	int NumTiles = (int)( FileSize / sizeof(short) );

	short* Tiles = (short*)memory_globals::default_allocator().allocate( sizeof(short)*NumTiles );
	fread( Tiles, sizeof(short), NumTiles, Map );

	fclose( Map );
	
	return Tiles;
}