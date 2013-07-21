#include <memory>
#include <core/memory.h>
#include <core/string_stream.h>

#include <hge\hge.h>
#include <hge\hgeresource.h>
#include <hge\hgegui.h>
#include <Core/CollisionComponent.h>

#include <Core/World.h>
#include <Core/Tileset.h>
#include "Cat.h"
#include <cassert>
#include <Core/TabBar.h>
#include <Core/TopBar.h>
#include <Core/GUIHandler.h>
#include <Core/TopButton.h>
#include <Core/EditorExtension.h>

// @DEBUG: Set this to 1 to se the result of a trace
#define DEBUG_TRACE 0

using namespace foundation;
using namespace Scribble;

bool g_IsEditor = false;

HGE* g_Hge = NULL;
Tileset* g_Tileset = NULL;
World* g_World = NULL;
hgeResourceManager* g_ResManager = NULL;
GUIHandler* g_GUI = NULL;

bool g_DebugRenderPhysics = true;

bool FrameFunc();
bool RenderFunc();
bool GfxRestoreFunc();

#if DEBUG_TRACE
bool SweepMoveSource = false;
Vector2 SweepSource, SweepDestination, SweepExtent;
TraceHit Trace;
#endif

Array<EditorExtension*>* g_EditorExtensions = NULL;
int g_ScreenWidth = 1280;
int g_ScreenHeight = 720;

Cat* g_Cat = NULL;

short* LoadTileLayer( const char* FromFile );
void FindEditorExtensions( Array<EditorExtension*>& Out_Extensions );

HTARGET g_TargetTexture;
hgeSprite* g_WorldSprite = NULL;

INT WINAPI WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	foundation::memory_globals::init();
	// Scope up so that arrays are deallocated before the memory subsystem does
	{
		Array<EditorExtension*> Extensions( memory_globals::default_allocator() );
		g_EditorExtensions = &Extensions;

		if( strstr( lpCmdLine, "-editor" ) != NULL )
		{
			g_IsEditor = true;
		}

		g_Hge = hgeCreate(HGE_VERSION);

		g_Hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
		g_Hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
		g_Hge->System_SetState(HGE_TITLE, "Scribbor catzorz");
	
		g_Hge->System_SetState(HGE_SCREENWIDTH, g_ScreenWidth );
		g_Hge->System_SetState(HGE_SCREENHEIGHT, g_ScreenHeight );
	
		g_Hge->System_SetState(HGE_INIFILE, "../Media/Config.ini" );
		g_Hge->System_SetState(HGE_LOGFILE, "Game.log" );
	
		g_Hge->System_SetState(HGE_FPS, 60);
	
		g_Hge->System_SetState(HGE_WINDOWED, true);
		g_Hge->System_SetState(HGE_USESOUND, true);
		g_Hge->System_SetState(HGE_SHOWSPLASH, false);

		g_Hge->System_SetState(HGE_HIDEMOUSE, true);
		g_Hge->System_SetState(HGE_GFXRESTOREFUNC, GfxRestoreFunc);

		if( g_Hge->System_Initiate() )
		{
			g_ResManager = MAKE_NEW( memory_globals::default_allocator(), hgeResourceManager, "../Media/Resources.res" );

			g_World = MAKE_NEW( memory_globals::default_allocator(), World );
			g_GUI = MAKE_NEW( memory_globals::default_allocator(), GUIHandler );
			if( g_IsEditor )
			{
				FindEditorExtensions( Extensions );
				g_GUI->SetRootGUIContainer( g_GUI->CreateEditorContainer() );

				g_TargetTexture = g_Hge->Target_Create( g_ScreenWidth, g_ScreenHeight - 20 - 24, false );
				g_WorldSprite = MAKE_NEW( memory_globals::default_allocator(), hgeSprite, g_Hge->Target_GetTexture( g_TargetTexture ), 0, 0, (float)g_ScreenWidth, (float)g_ScreenHeight - 20 - 24 );

				g_Tileset = g_World->Spawn<Tileset>( Vector2( 0, 0 ) );
				g_Cat = g_World->Spawn<Cat>( Vector2( 200, 50 ) );
				g_Tileset->AddLayer( 100, 100, 32.0f, LoadTileLayer( "ArtTest.MAR" ), g_Hge->Texture_Load( "../Media/Textures/Tileset.png" ) ); 
			}
			else
			{
				g_Tileset = g_World->Spawn<Tileset>( Vector2( 0, 0 ) );
				g_Cat = g_World->Spawn<Cat>( Vector2( 200, 50 ) );
				g_Tileset->AddLayer( 100, 100, 32.0f, LoadTileLayer( "ArtTest.MAR" ), g_Hge->Texture_Load( "../Media/Textures/Tileset.png" ) ); 

#if DEBUG_TRACE
				CollisionComponent* Comp = CollisionComponent::CreateCircle( b2_staticBody, Vector2( 500, 500 ), 25.0f );
				g_Tileset->AttachComponent( Comp );

				SweepExtent = Vector2( 20, 20 );
#endif
			}

			g_Hge->System_Start();
		}
		else
		{	
			// If HGE initialization failed show error message
			MessageBoxA(NULL, g_Hge->System_GetErrorMessage(), "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
		}

		g_World->ClearActorList();
	
		MAKE_DELETE( memory_globals::default_allocator(), hgeSprite, g_WorldSprite );
		g_Hge->Target_Free( g_TargetTexture );

		for( uint32_t Idx = 0; Idx < array::size( Extensions ); ++Idx )
		{
			Extensions[Idx]->Destroy();
		}
		MAKE_DELETE( memory_globals::default_allocator(), GUIHandler, g_GUI );
		MAKE_DELETE( memory_globals::default_allocator(), World, g_World );
		MAKE_DELETE( memory_globals::default_allocator(), hgeResourceManager, g_ResManager );

		g_Hge->Release();
		 
	}
	foundation::memory_globals::shutdown();
	
	return EXIT_SUCCESS;
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

bool windowed = true;

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
	if( g_Hge->Input_KeyDown( HGEK_K ) )
	{
		windowed = !windowed;
		g_Hge->System_SetState(HGE_WINDOWED, windowed );
	}

	g_World->Tick( DeltaTime );

	g_GUI->Tick( DeltaTime );

#if DEBUG_TRACE
	if( g_Cat )
	{
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
	}
#endif

	return false;
}

bool RenderFunc()
{
	float DeltaTime = g_Hge->Timer_GetDelta();
	
	// Render the world to a rendertarget if it's in the editor
	g_Hge->Gfx_BeginScene( g_IsEditor ? g_TargetTexture : NULL );
	g_Hge->Gfx_Clear( ARGB( 0, 0, 0, 0 ) );

	g_World->Render();
	FontSettings Settings;
	Settings.Color = ARGB( 255, 255, 255, 255 );
	Settings.Scale = 1.0f;
	g_GUI->SetFontSettings( Settings );
	g_GUI->Printf( 10, 10, HGETEXT_LEFT, "FPS: %i", g_Hge->Timer_GetFPS() );

#if DEBUG_TRACE
	DrawSweeping( SweepSource, SweepDestination, SweepExtent, Trace );
#endif
	g_Hge->Gfx_EndScene();

	g_Hge->Gfx_BeginScene();
	if( g_IsEditor )
	{
		g_Hge->Gfx_Clear( ARGB( 0, 0, 0, 0 ) );
		g_WorldSprite->Render( 0, 43.0f );
	}
	// GUI should always render without any transform
	g_Hge->Gfx_SetTransform();
	g_GUI->Render();
	
	g_Hge->Gfx_EndScene();

	return false;
}

bool GfxRestoreFunc()
{
	// Add all rendertargets here so that the game doesn't crash when they are invalidated
	g_WorldSprite->SetTexture( g_Hge->Target_GetTexture( g_TargetTexture ) );

	return true;
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

typedef EditorExtension* (WINAPIV* CreateExtensionFunc)( Allocator*, Allocator* );

void FindEditorExtensions( Array<EditorExtension*>& Out_Extensions )
{
	WIN32_FIND_DATAA Data;
	const char* Path = "EditorExtensions/*.dll";
	
	HANDLE FirstFile = FindFirstFileA( Path, &Data );

	// Invalid directory
	if( FirstFile == INVALID_HANDLE_VALUE )
	{
		return;
	}

	do
	{
		using namespace string_stream;
		Buffer TmpFileName( memory_globals::default_allocator() );
		TmpFileName << "EditorExtensions/" << Data.cFileName;

		char FullFileName[MAX_PATH];
		int FullFileNameSize = GetFullPathNameA( c_str( TmpFileName ), MAX_PATH, FullFileName, NULL );

		if( HMODULE Library = LoadLibraryA( FullFileName ) )
		{
			CreateExtensionFunc CreateExtension = (CreateExtensionFunc)GetProcAddress( Library, "CreateExtension" );
			
			if( CreateExtension )
			{
				array::push_back( Out_Extensions, CreateExtension( &memory_globals::default_allocator(), &memory_globals::default_scratch_allocator() ) );
			}
		}
	}while( FindNextFileA( FirstFile, &Data ) != NULL || GetLastError() != ERROR_NO_MORE_FILES );
}