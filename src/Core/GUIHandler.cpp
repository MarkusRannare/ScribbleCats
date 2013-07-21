#include "GUIHandler.h"

#include <Core/memory.h>
#include "TopBar.h"
#include "TabBar.h"
#include <hge/hgegui.h>
#include <hge/hgefont.h>
#include <hge/hgeresource.h>
#include "EditorGUIContainer.h"
using namespace foundation;

extern hgeResourceManager* g_ResManager;
extern bool g_IsEditor;

namespace Scribble
{
#pragma warning( disable : 4355 )
	GUIHandler::GUIHandler() :
		mGUI( MAKE_NEW( memory_globals::default_allocator(), hgeGUI ) ),
		mFont( MAKE_NEW( memory_globals::default_allocator(), hgeFont, "../Media/Fonts/font1.fnt" ) ),
		mRootContainer( NULL )
	{
		hgeSprite* sprite = g_ResManager->GetSprite( "Cursor" );
		mGUI->SetCursor( sprite );
		mGUI->Enter();
	}
#pragma warning( default : 4355 )

	GUIHandler::~GUIHandler()
	{
		MAKE_DELETE( memory_globals::default_allocator(), hgeGUI, mGUI );
		MAKE_DELETE( memory_globals::default_allocator(), hgeFont, mFont );

		// @TODO: Is this correct ownership? How do we handle switching of root containers?
		if( mRootContainer )
		{
			MAKE_DELETE( memory_globals::default_allocator(), GUIContainer, mRootContainer );
		}
	}

	void GUIHandler::Tick( float DeltaTime )
	{
		if( mRootContainer )
		{
			mRootContainer->Tick( DeltaTime );
		}

		mGUI->Update( DeltaTime );
	}

	void GUIHandler::Render()
	{
		if( mRootContainer )
		{
			mRootContainer->Render();
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

	void GUIHandler::SetRootGUIContainer( GUIContainer* Container )
	{
		if( mRootContainer != NULL )
		{
			mRootContainer->Exit();
		}

		mRootContainer = Container;

		if( mRootContainer )
		{
			mRootContainer->Enter();
		}
	}

	GUIContainer* GUIHandler::CreateEditorContainer()
	{
		return MAKE_NEW( memory_globals::default_allocator(), EditorGUIContainer, this );
	}
}