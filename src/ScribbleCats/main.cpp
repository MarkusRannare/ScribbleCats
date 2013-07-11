#include <memory>
#include <foundation\memory.h>

#include <hge\hge.h>
#include <hge\hgeresource.h>
#include <hge\hgefont.h>
#include <hge\hgegui.h>
#include "CollisionComponent.h"

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
hgeResourceManager* g_ResManager = NULL;

bool g_DebugRenderPhysics = true;

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

	g_Hge->System_SetState(HGE_HIDEMOUSE, true);

	if(g_Hge->System_Initiate())
	{
		g_ResManager = MAKE_NEW( memory_globals::default_allocator(), hgeResourceManager, "../Media/Resources.res" );

		g_Font = MAKE_NEW( memory_globals::default_allocator(), hgeFont, "../Media/Fonts/font1.fnt" );

		g_World = MAKE_NEW( memory_globals::default_allocator(), World );
		g_Tileset = g_World->Spawn<Tileset>( Vector2( 0, 0 ) );
		g_Cat = g_World->Spawn<Cat>( Vector2( 200, 50 ) );
		g_Tileset->AddLayer( 100, 100, 32.0f, LoadTileLayer( "ArtTest.MAR" ), g_Hge->Texture_Load( "../Media/Textures/Tileset.png" ) ); 

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
	MAKE_DELETE( memory_globals::default_allocator(), hgeResourceManager, g_ResManager );

	g_Hge->Release();

	foundation::memory_globals::shutdown();
	
	return EXIT_SUCCESS;
}

bool FrameFunc()
{
	float DeltaTime = g_Hge->Timer_GetDelta();

	if( g_Hge->Input_KeyDown( HGEK_P ) )
	{
		g_DebugRenderPhysics = !g_DebugRenderPhysics;
	}
	if( g_Hge->Input_KeyDown( HGEK_R ) )
	{
		g_Hge->Texture_Free( g_Tileset->mLayers[0].SourceTexture );
		 g_Tileset->mLayers[0].SourceTexture = g_Hge->Texture_Load( "../Media/Textures/Tileset.png" );
	}
	
	g_World->Tick( DeltaTime );

	return false;
}

/** Debug function for drawing boxes... */
void DrawBox( const Vector2& Location, const Vector2& Extent, DWORD Color )
{
	g_Hge->Gfx_RenderLine( Location.X - Extent.X, Location.Y - Extent.Y, Location.X + Extent.X, Location.Y - Extent.Y, Color );
	g_Hge->Gfx_RenderLine( Location.X + Extent.X, Location.Y - Extent.Y, Location.X + Extent.X, Location.Y + Extent.Y, Color );
	g_Hge->Gfx_RenderLine( Location.X + Extent.X, Location.Y + Extent.Y, Location.X - Extent.X, Location.Y + Extent.Y, Color );
	g_Hge->Gfx_RenderLine( Location.X - Extent.X, Location.Y + Extent.Y, Location.X - Extent.X, Location.Y - Extent.Y, Color );
}

/** Debug function for drawing the results of a sweep! Good for debugging! */
void DrawSweeping( const Vector2& From, const Vector2& To, const Vector2& Extent, const TraceResult& TraceResults )
{
	DrawBox( From, Extent, ARGB( 255, 0, 255, 0 ) );
	DrawBox( To, Extent, ARGB( 255, 0, 0, 255 ) );

	if( To.Y < From.Y && From.X < To.X
		|| To.Y > From.Y && From.X > To.X )
	{
		g_Hge->Gfx_RenderLine( 
			From.X - Extent.X, 
			From.Y - Extent.Y, 
			To.X - Extent.X, 
			To.Y - Extent.Y );
	}

	if( To.Y > From.Y && From.X < To.X 
		|| To.Y < From.Y && From.X > To.X )
	{ 
		g_Hge->Gfx_RenderLine( 
			From.X + Extent.X, 
			From.Y - Extent.Y, 
			To.X + Extent.X, 
			To.Y - Extent.Y );
	}

	if( To.Y < From.Y && From.X < To.X
		|| To.Y > From.Y && From.X > To.X )
	{
		g_Hge->Gfx_RenderLine( 
			From.X + Extent.X, 
			From.Y + Extent.Y, 
			To.X + Extent.X, 
			To.Y + Extent.Y );
	}

	if( To.Y > From.Y && From.X < To.X
		|| To.Y < From.Y && From.X > To.X )
	{
		g_Hge->Gfx_RenderLine( 
			From.X - Extent.X, 
			From.Y + Extent.Y, 
			To.X - Extent.X, 
			To.Y + Extent.Y );
	}

	/*extern Array<Scribble::CollisionComponent*>* g_DebugBroadphaseComponents;
	extern Array<Scribble::CollisionComponent*>* g_DebugHitComponents;

	for( uint32_t Idx = 0; Idx < array::size( *g_DebugBroadphaseComponents ); ++Idx )
	{
		CollisionComponent* Component = (*g_DebugBroadphaseComponents)[Idx];
		DrawBox( B2ToVector( Component->mPhysicsBody->GetPosition() ) * TO_WORLD, Vector2( 16.0f, 16.0f ), ARGB( 255, 255, 255, 0 ) ); 
	}

	for( uint32_t Idx = 0; Idx < array::size( *g_DebugHitComponents ); ++Idx )
	{
		CollisionComponent* Component = (*g_DebugHitComponents)[Idx];
		DrawBox( B2ToVector( Component->mPhysicsBody->GetPosition() ) * TO_WORLD, Vector2( 16.0f, 16.0f ), ARGB( 255, 255, 0, 255 ) ); 
	}*/

	if( TraceResults.HitComponent != NULL )
	{
		DrawBox( TraceResults.HitLocation, Extent, ARGB( 255, 255, 0, 0 ) );
		DrawBox( B2ToVector( ((CollisionComponent*)TraceResults.HitComponent)->mPhysicsBody->GetPosition() ) * TO_WORLD, Vector2( 16.0f, 16.0f ), ARGB( 255, 255, 255, 255 ) ); 
		g_Hge->Gfx_RenderLine( TraceResults.HitLocation.X, TraceResults.HitLocation.Y, TraceResults.HitLocation.X + TraceResults.HitNormal.X * 10.0f, TraceResults.HitLocation.Y + TraceResults.HitNormal.Y * 10.0f );
	}
}

bool RenderFunc()
{
	float DeltaTime = g_Hge->Timer_GetDelta();
	
	g_Hge->Gfx_BeginScene();
	g_Hge->Gfx_Clear( ARGB( 0, 0, 0, 0 ) );

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