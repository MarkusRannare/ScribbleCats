#include "World.h"

#include "array_functions.h"
#include "PhysicsWorld.h"

extern bool g_DebugRenderPhysics;

using namespace foundation;

#include "Cat.h"

namespace Scribble
{
	World::World() :
		mActors( memory_globals::default_allocator() ),
		mDeadActors( memory_globals::default_allocator() ),
		mNewActors( memory_globals::default_allocator() ),
		mPhysicsWorld( MAKE_NEW( memory_globals::default_allocator(), PhysicsWorld ) )
	{
	}

	World::~World()
	{
		MAKE_DELETE( memory_globals::default_allocator(), PhysicsWorld, mPhysicsWorld );
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

		if( g_DebugRenderPhysics )
		{
			mPhysicsWorld->DebugRender();
		}
	}

	PhysicsWorld* World::GetPhysicsWorld() const
	{
		return mPhysicsWorld;
	}

	bool World::SetActorLocation( Actor* Actor, const Vector2& ToLocation )
	{
		AARB TargetLocation;
		TargetLocation._Center = ToLocation;
		TargetLocation._Extent = Actor->mCollision._Extent;

		if( mPhysicsWorld->IsSpaceFree( TargetLocation ) )
		{
			Actor->mLocation = ToLocation;

			return true;
		}

		return false;
	}

	bool World::ResolveCollision( Actor* Actor, 
		const Vector2& TargetLocation,
		const CollisionData& CollisionInfo,
		float TimesliceRemaining,
		Vector2& out_NewWantedLocation )
	{
		Vector2 Displacement = TargetLocation - Actor->mLocation;

		Actor->mLocation += Displacement * ( CollisionInfo.FirstContact - 0.001f );

		// Kill all velocity in the direction of the normal for our current actor
		Actor->mVelocity -= Actor->mVelocity.Dot( CollisionInfo._Normal ) * CollisionInfo._Normal;

		// Calculate our new wanted displacement without any movement in the direction of the collision normal
		Displacement -= Displacement.Dot( CollisionInfo._Normal ) * CollisionInfo._Normal;

		out_NewWantedLocation = Actor->mLocation + Displacement * ( 1.0f - CollisionInfo.FirstContact ) * TimesliceRemaining;
		
		return Displacement == Vector2::ZERO;
	}

	bool World::MoveActor( Actor* Actor, const Vector2& ToLocation )
	{
		bool HitSomething = false;
		Vector2 TargetLocation = ToLocation;
		float TimesliceLeft = 1.0f;

		const float TIMESLICE_EPSILON = 0.00001f;
		int Counter = 0;

		while( TimesliceLeft > TIMESLICE_EPSILON )
		{
			CollisionData CollisionInfo;

			bool Result = mPhysicsWorld->SweepAARB( Actor->mLocation, TargetLocation, Actor->mCollision._Extent, &CollisionInfo );
		
			if( !Result )
			{
				Actor->mLocation = TargetLocation;
				return HitSomething;
			}
			else
			{
				HitSomething = true;

				Vector2 OldVelocity = Actor->mVelocity;

				Vector2 NewTargetLocation;
				if( ResolveCollision( Actor, TargetLocation, CollisionInfo, TimesliceLeft, NewTargetLocation ) )
				{
					break;
				}
				TargetLocation = NewTargetLocation;

				// We have possible landed
				if( OldVelocity.Y > 0 && Actor->mVelocity.Y == 0 )
				{
					Actor->Landed( CollisionInfo );
				}

				TimesliceLeft = ( 1.0f - CollisionInfo.FirstContact ) * TimesliceLeft;
				
				if( Counter++ > 32 )
				{
					break;
				}
			}
		}

		return HitSomething;
	}
}