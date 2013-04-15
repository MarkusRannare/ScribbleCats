#include "PhysicsWorld.h"
#include "Body.h"

#include "foundation/memory.h"
#include "foundation/array.h"

using namespace foundation;

namespace Scribble
{
	PhysicsWorld::PhysicsWorld() :
		mBodies( memory_globals::default_allocator() )
	{
	}

	PhysicsWorld::~PhysicsWorld()
	{
		for( size_t Idx = 0; Idx < array::size( mBodies ); ++Idx )
		{
		}
	}

	Body* PhysicsWorld::CreateBody( Shape* Shape )
	{
		Body* body = MAKE_NEW( memory_globals::default_allocator(), Body );

		array::push_back( mBodies, body );

		return body;
	}

	void PhysicsWorld::DestroyBody( Body* Body )
	{
	}
}