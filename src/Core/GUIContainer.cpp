#include "GUIContainer.h"

#include "array_functions.h"

using namespace foundation;

namespace Scribble
{
	GUIContainer::GUIContainer( GUIHandler* Handler, const Vector2& Location ) :
		mSubContainers( memory_globals::default_allocator() ),
		mHandler( Handler ),
		mLocation( Location )
	{
	}

	GUIContainer::~GUIContainer()
	{
		for( uint32_t Idx = 0; Idx < array::size( mSubContainers ); ++Idx )
		{
			MAKE_DELETE( memory_globals::default_allocator(), GUIContainer, mSubContainers[Idx] );
		}
	}

	void GUIContainer::AddSubContainer( GUIContainer* Container )
	{
		if( array::find( mSubContainers, Container ) == INDEX_NONE )
		{
			array::push_back( mSubContainers, Container );
		}
	}

	void GUIContainer::RemoveSubContainer( GUIContainer* Container )
	{
		int FoundIdx = array::find( mSubContainers, Container );
		if( FoundIdx != INDEX_NONE )
		{
			array::remove_swap_index( mSubContainers, FoundIdx );
		}
	}

	void GUIContainer::Enter()
	{
	}

	void GUIContainer::Exit()
	{
	}

	void GUIContainer::Tick( float DeltaTime )
	{
		for( uint32_t Idx = 0; Idx < array::size( mSubContainers ); ++Idx )
		{
			mSubContainers[Idx]->Tick( DeltaTime );
		}
	}

	void GUIContainer::Render()
	{
		for( uint32_t Idx = 0; Idx < array::size( mSubContainers ); ++Idx )
		{
			mSubContainers[Idx]->Render();
		}
	}
}