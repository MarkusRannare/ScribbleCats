#include "TopBar.h"

#include <hge/hgesprite.h>
#include <Core/memory.h>
#include "array_functions.h"
#include "TopButton.h"
#include "GUIHandler.h"
#include "Vector2.h"

extern int g_ScreenWidth;

using namespace foundation;

namespace Scribble
{
	TopBar::TopBar( GUIHandler* Handler ) :
		GUIContainer( Handler ),
		mSprite( MAKE_NEW( memory_globals::default_allocator(), hgeSprite, NULL, 0, 0, (float)g_ScreenWidth, 23.5f ) ),
		mButtons( memory_globals::default_allocator() )
	{
		mSprite->SetColor( ARGB( 255, 200, 200, 200 ) );
	}

	TopBar::~TopBar()
	{
		MAKE_DELETE( memory_globals::default_allocator(), hgeSprite, mSprite );
	}

	TopButton* TopBar::AddButton( const char* Text )
	{
		TopButton* NewButton = new TopButton( Text, mHandler );

		array::push_back( mButtons, NewButton );
		mHandler->GetGUI()->AddCtrl( NewButton );

		return NewButton;
	}

	void TopBar::RemoveButton( TopButton* Button )
	{
		array::remove_swap( mButtons, Button );
		// Also deletes the button
		mHandler->GetGUI()->DelCtrl( Button->id );
	}

	void TopBar::Tick( float DeltaTime )
	{
		Vector2 Location( 5, 5 );
		for( uint32_t Idx = 0; Idx < array::size( mButtons ); ++Idx )
		{
			mButtons[Idx]->SetLocation( Location.X, Location.Y, Location.X );
		}
	}

	void TopBar::Render()
	{
		mSprite->Render( 0, 0 );
	}
}