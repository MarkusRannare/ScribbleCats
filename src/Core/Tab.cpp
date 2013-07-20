#include "Tab.h"
#include <cstring>

#include <Core/memory.h>
#include "GUIHandler.h"

using namespace foundation;

namespace Scribble
{
	Tab::Tab( const char* Text, GUIHandler* Handler ) :
		mText( (char*)memory_globals::default_allocator().allocate( strlen( Text ) + 1 ) ),
		mHandler( Handler )
	{
		memcpy_s( mText, strlen( Text ) + 1, Text, strlen( Text ) + 1 );
	}

	Tab::~Tab()
	{
		memory_globals::default_allocator().deallocate( mText );
	}

	void Tab::Update( float DeltaTime )
	{
	}

	void Tab::Render()
	{
		FontSettings Settings;
		Settings.Color = ARGB( 255, 0, 0, 0 );
		Settings.Scale = 0.5f;
		mHandler->SetFontSettings( Settings );
		mHandler->Printf( 5, 25, HGETEXT_LEFT, "%s", mText );
	}
}