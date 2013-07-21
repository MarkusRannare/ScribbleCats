#include "EditorGUIContainer.h"
#include "TabBar.h"
#include "TopBar.h"
#include "memory.h"
#include "TopButton.h"
#include "EditorExtension.h"
#include "array_functions.h"

using namespace foundation;

#define CREATE_CONTAINER( Class ) MAKE_NEW( memory_globals::default_allocator(), Class, mHandler )
#define DESTORY_CONTAINER( Class, Pointer ) MAKE_DELETE( memory_globals::default_allocator(), Class, Pointer )

namespace Scribble
{
	EditorGUIContainer::EditorGUIContainer( GUIHandler* Handler ) :
		GUIContainer( Handler )
	{
		TabBar* TabBarContainer  = CREATE_CONTAINER( TabBar );
		TabBarContainer->AddTab( "Test" );
		AddSubContainer( TabBarContainer );

		TopBar* TopBarContainer = CREATE_CONTAINER( TopBar );
		TopButton* File = TopBarContainer->AddButton( "File" );
		File->AddSubText( "New" );
		File->AddSubText( "Open" );
		File->AddSubText( "Save" );
		File->AddSubText( "Save As" );
		File->AddSubText( "Quit" );

		TopButton* Tools = TopBarContainer->AddButton( "Tools" );
		extern Array<EditorExtension*>* g_EditorExtensions;
		for( uint32_t Idx = 0; Idx < array::size( *g_EditorExtensions ); ++Idx )
		{
			Tools->AddSubText( (*g_EditorExtensions)[Idx]->GetExtensionName() );
		}
		AddSubContainer( TopBarContainer );
	}

	EditorGUIContainer::~EditorGUIContainer()
	{
	}

	void EditorGUIContainer::Enter()
	{
	}

	void EditorGUIContainer::Leave()
	{
	}

	void EditorGUIContainer::Tick( float DeltaTime )
	{
		GUIContainer::Tick( DeltaTime );
	}

	void EditorGUIContainer::Render()
	{
		GUIContainer::Render();
	}
}