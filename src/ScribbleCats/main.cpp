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

// @DEBUG: Set this to 1 to se the result of a trace
#define DEBUG_TRACE 0

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

#if DEBUG_TRACE
bool SweepMoveSource = false;
Vector2 SweepSource, SweepDestination, SweepExtent;
TraceHit Trace;
#endif

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

#if DEBUG_TRACE
		CollisionComponent* Comp = CollisionComponent::CreateCircle( b2_staticBody, Vector2( 500, 500 ), 25.0f );
		g_Tileset->AttachComponent( Comp );

		SweepExtent = Vector2( 20, 20 );
#endif

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

#if DEBUG_TRACE
	if( g_Hge->Input_KeyUp( HGEK_LBUTTON ) )
	{
		SweepMoveSource = !SweepMoveSource;
	}

	Vector2& ToMove = SweepMoveSource ? SweepSource : SweepDestination;
	Vector2 MousePos;

	g_Hge->Input_GetMousePos( &MousePos.X, &MousePos.Y );
	
	ToMove += MousePos - Vector2( g_ScreenWidth / 2.0f, g_ScreenHeight / 2.0f );
	
	g_Hge->Input_SetMousePos( g_ScreenWidth / 2.0f, g_ScreenHeight / 2.0f );

	g_World->Trace( g_Cat, SweepSource, SweepDestination, SweepExtent, &Trace );
#endif

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
void DrawSweeping( const Vector2& From, const Vector2& To, const Vector2& Extent, const TraceHit& TraceResults )
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

	if( TraceResults.HitComponent != NULL )
	{
		DrawBox( TraceResults.HitLocation, Extent, ARGB( 255, 255, 0, 0 ) );
		DrawBox( TraceResults.HitComponent->GetLocation(), Vector2( 16.0f, 16.0f ), ARGB( 255, 255, 255, 255 ) ); 
		
		const Vector2 HitLocation = TraceResults.HitLocation;
		const Vector2 EndNormal = HitLocation + TraceResults.HitNormal * 20.0f;

		// Draw normal
		g_Hge->Gfx_RenderLine( HitLocation.X, HitLocation.Y, EndNormal.X, EndNormal.Y );
		// Draw left part of arrow
		Vector2 Flerp = TraceResults.HitNormal;
		Flerp.Rotate( 0.5f );
		g_Hge->Gfx_RenderLine( EndNormal.X, EndNormal.Y, EndNormal.X - Flerp.X * 10.0f, EndNormal.Y - Flerp.Y * 10.0f );
		// Draw right part of arrow
		Flerp = TraceResults.HitNormal;
		Flerp.Rotate( -0.5f );
		g_Hge->Gfx_RenderLine( EndNormal.X, EndNormal.Y, EndNormal.X - Flerp.X * 10.0f, EndNormal.Y - Flerp.Y * 10.0f );
	}
}

bool RenderFunc()
{
	float DeltaTime = g_Hge->Timer_GetDelta();
	
	g_Hge->Gfx_BeginScene();
	g_Hge->Gfx_Clear( ARGB( 0, 0, 0, 0 ) );

	g_World->Render();

	g_Font->printf( 10, 10, HGETEXT_LEFT, "FPS: %i", g_Hge->Timer_GetFPS() );

#if DEBUG_TRACE
	DrawSweeping( SweepSource, SweepDestination, SweepExtent, Trace );
#endif
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