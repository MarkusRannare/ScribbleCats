#include "TabBar.h"

#include <new>
#include <Core/memory.h>
#include "Tab.h"
#include "array_functions.h"
#include "GUIHandler.h"

#include <hge/hgesprite.h>

extern int g_ScreenWidth;

using namespace foundation;

namespace Scribble
{
	TabBar::TabBar( GUIHandler* Handler ) :
		GUIContainer( Handler ),
		mSprite( MAKE_NEW( memory_globals::default_allocator(), hgeSprite,  NULL, 0, 0, (float)g_ScreenWidth, 20.0f ) ),
		mTabs( memory_globals::default_allocator() )
	{
	}

	TabBar::~TabBar()
	{
		MAKE_DELETE( memory_globals::default_allocator(), hgeSprite, mSprite );
	}

	void TabBar::Tick( float Dt )
	{
	}

	void TabBar::Render()
	{
		mSprite->Render( 0, 20 );
	}

	Tab* TabBar::AddTab( const char* Text )
	{
		Tab* NewTab = new Tab( Text, mHandler );
		array::push_back( mTabs, NewTab );
		mHandler->GetGUI()->AddCtrl( NewTab );

		return NewTab;
	}

	void TabBar::RemoveTab( Tab* Tab )
	{
		array::remove_swap( mTabs, Tab );
		mHandler->GetGUI()->DelCtrl( Tab->id );
		MAKE_DELETE( memory_globals::default_allocator(), Tab, Tab );
	}
}