#include "World.h"

#include "array_functions.h"

using namespace foundation;

namespace Scribble
{
	World::World() :
		mActors( memory_globals::default_allocator() ),
		mDeadActors( memory_globals::default_allocator() ),
		mNewActors( memory_globals::default_allocator() )
	{
	}

	World::~World()
	{
	}

	void World::Destroy( Actor* Actor )
	{
		array::add_unique( mDeadActors, Actor );
	}

	void World::Tick( float Dt )
	{
		for( size_t Idx = 0; Idx < array::size( mActors ); ++Idx )
		{
			mActors[Idx]->Tick( Dt );
		}
		HandleNewAndOldActors();		
	}

	void World::ClearActorList()
	{
		for( size_t Idx = 0; Idx < array::size( mActors ); ++Idx )
		{
			Destroy( mActors[Idx] );
		}
		array::clear( mActors );
		
		for( size_t Idx = 0; Idx < array::size( mNewActors ); ++Idx )
		{
			Destroy( mNewActors[Idx] );
		}
		array::clear( mNewActors );
		
		HandleNewAndOldActors();
	}

	void World::HandleNewAndOldActors()
	{
		for( size_t Idx = 0; Idx < array::size( mNewActors ); ++Idx )
		{
			array::push_back( mActors, mNewActors[ Idx ] );
		}
		array::clear( mNewActors );
		
		for( size_t Idx = 0; Idx < array::size( mDeadActors ); ++Idx )
		{
			Actor* Dead = mDeadActors[Idx];
			array::remove_swap( mActors, Dead );
			MAKE_DELETE( memory_globals::default_allocator(), Actor, Dead );
		}
		array::clear( mDeadActors );
	}

	void World::Render()
	{
		for( size_t Idx = 0; Idx < array::size( mActors ); ++Idx )
		{
			mActors[Idx]->Render();
		}
	}
}