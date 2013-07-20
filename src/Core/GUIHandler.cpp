#include "GUIHandler.h"

#include <Core/memory.h>
#include "TopBar.h"
#include "TabBar.h"
#include <hge/hgegui.h>
#include <hge/hgefont.h>
#include <hge/hgeresource.h>

using namespace foundation;

extern hgeResourceManager* g_ResManager;
extern bool g_IsEditor;

namespace Scribble
{
#pragma warning( disable : 4355 )
	GUIHandler::GUIHandler() :
		mGUI( MAKE_NEW( memory_globals::default_allocator(), hgeGUI ) ),
		mTopBar( g_IsEditor ? MAKE_NEW( memory_globals::default_allocator(), TopBar, this ) : NULL ),
		mTabBar( g_IsEditor ? MAKE_NEW( memory_globals::default_allocator(), TabBar, this ) : NULL ),
		mFont( MAKE_NEW( memory_globals::default_allocator(), hgeFont, "../Media/Fonts/font1.fnt" ) )
	{
		hgeSprite* sprite = g_ResManager->GetSprite( "Cursor" );
		mGUI->SetCursor( sprite );
		mGUI->Enter();
	}
#pragma warning( default : 4355 )

	GUIHandler::~GUIHandler()
	{
		if( mTopBar )
		{
			MAKE_DELETE( memory_globals::default_allocator(), TopBar, mTopBar );
		}
		if( mTabBar )
		{
			MAKE_DELETE( memory_globals::default_allocator(), TabBar, mTabBar );
		}

		MAKE_DELETE( memory_globals::default_allocator(), hgeGUI, mGUI );
		MAKE_DELETE( memory_globals::default_allocator(), hgeFont, mFont );
	}

	TopBar* GUIHandler::ShowTopBar()
	{
		return mTopBar;
	}

	void GUIHandler::HideTopBar()
	{
	}

	TabBar* GUIHandler::ShowTabBar()
	{
		return mTabBar;
	}

	void GUIHandler::HideTabBar()
	{
	}

	void GUIHandler::Tick( float DeltaTime )
	{
		if( mTabBar )
		{
			mTabBar->Update( DeltaTime );
		}
		if( mTopBar )
		{
			mTopBar->Update( DeltaTime );
		}
		mGUI->Update( DeltaTime );
	}

	void GUIHandler::Render()
	{
		if( mTabBar )
		{
			mTabBar->Render();
		}
		if( mTopBar )
		{
			mTopBar->Render();
		}

		mGUI->Render();
	}

	hgeGUI* GUIHandler::GetGUI() const
	{ 
		return mGUI;
	}

	void GUIHandler::SetFontSettings( const FontSettings& Settings )
	{
		mFont->SetColor( Settings.Color );
		mFont->SetScale( Settings.Scale );
	}

	void GUIHandler::Printf( float X, float Y, DWORD Align, const char* Format, ... )
	{
		hgeFont::va_wrapper Args;
			
		va_start( Args.Args, Format );
		mFont->_printf( X, Y, Align, Format, Args );		
		va_end( Args.Args );
	}

	const hgeFont* GUIHandler::GetFont() const
	{
		return mFont;
	}
}