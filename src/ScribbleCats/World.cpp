#include "World.h"

#include "array_functions.h"
#include "PhysicsWorld.h"
#include "Box2DDebugDraw.h"

extern bool g_DebugRenderPhysics;

using namespace foundation;

#include "Cat.h"

namespace Scribble
{
	World::World() :
		mActors( memory_globals::default_allocator() ),
		mDeadActors( memory_globals::default_allocator() ),
		mNewActors( memory_globals::default_allocator() ),
		mPhysicsWorld( b2Vec2( 0.0f, 10.0f ) ),
		mComponents( memory_globals::default_allocator() )
	{
		mDebugPhysics = MAKE_NEW( memory_globals::default_allocator(), Box2DDebugDraw );
		mPhysicsWorld.SetDebugDraw( mDebugPhysics );
	}

	World::~World()
	{
		MAKE_DELETE( memory_globals::default_allocator(), b2Draw, mDebugPhysics );
	}

	void World::Destroy( Actor* Actor )
	{
		array::add_unique( mDeadActors, Actor );
	}

	void World::Tick( float Dt )
	{
		const int32 VELOCITY_ITERATIONS = 6;
		const int32 POSITION_ITERATIONS = 2;
		const float LARGEST_TIMESTEP = 1.0f / 60.0f;
	
		// Let the physics work @ 60Hz or faster
		float TimestepLeft = Dt;
		do 
		{
			float CurrentTimestep = min( TimestepLeft, LARGEST_TIMESTEP );
			mPhysicsWorld.Step( CurrentTimestep, VELOCITY_ITERATIONS, POSITION_ITERATIONS );
			TimestepLeft =- CurrentTimestep;
		} while ( TimestepLeft > 0.0f );

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

	void World::ComponentAttached( Component* TheComponent )
	{
		if( array::find( mComponents, TheComponent ) == INDEX_NONE )
		{
			array::push_back( mComponents, TheComponent );
		}
	}

	void World::ComponentDeattached( Component* TheComponent )
	{
		if( array::find( mComponents, TheComponent ) != INDEX_NONE )
		{
			array::remove_swap( mComponents, TheComponent );
		}
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
			mDebugPhysics->ClearFlags( 0xffffffff );
			mDebugPhysics->AppendFlags( b2Draw::e_shapeBit );
			mPhysicsWorld.DrawDebugData();
		}
	}

	b2World& World::GetPhysicsWorld()
	{
		return mPhysicsWorld;
	}

	bool World::SetActorLocation( Actor* Actor, const Vector2& ToLocation )
	{
		class FreeSpaceCallback : public b2QueryCallback
		{
			public:
				FreeSpaceCallback() :
					mFoundAnyCollision( false )
				{
				}

				bool ReportFixture( b2Fixture* fixture )
				{
					mFoundAnyCollision = true;

					return false;
				}

				bool mFoundAnyCollision;
		};

		b2AABB TargetLocation;
		TargetLocation.lowerBound = VectorToB2( ( ToLocation - Actor->mCollision._Extent ) * TO_PHYSICS );
		TargetLocation.upperBound = VectorToB2( ( ToLocation + Actor->mCollision._Extent ) * TO_PHYSICS );
		
		FreeSpaceCallback Callback;
		mPhysicsWorld.QueryAABB( &Callback, TargetLocation );

		if( !Callback.mFoundAnyCollision )
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
		if( Actor->mPhysicsBody == NULL )
		{
			Actor->mLocation = ToLocation;
			return false;
		}

		bool HitSomething = false;
		Vector2 TargetLocation = ToLocation;
		float TimesliceLeft = 1.0f;

		const float TIMESLICE_EPSILON = 0.00001f;
		int Counter = 0;

		Vector2 OldVelocity = Actor->mVelocity;

		while( TimesliceLeft > TIMESLICE_EPSILON )
		{
			CollisionData CollisionInfo;

			b2TOIInput Input;
//			Input.proxyA

			b2TOIOutput Output;
			memset( &Output, 0, sizeof( b2TOIOutput ) );

			b2TimeOfImpact( &Output, &Input );
			//bool Result = mPhysicsWorld->SweepAARB( Actor->mLocation * TO_PHYSICS, TargetLocation * TO_PHYSICS, Actor->mCollision._Extent * TO_PHYSICS, &CollisionInfo );
		
			if( Output.state == b2TOIOutput::e_separated )
			{
				Actor->mLocation = TargetLocation;
				
				break;
			}
			else
			{
				HitSomething = true;

				Vector2 NewTargetLocation;
				bool ShouldAbort = ResolveCollision( Actor, TargetLocation, CollisionInfo, TimesliceLeft, NewTargetLocation );
				TargetLocation = NewTargetLocation;

				// We have possible landed
				if( OldVelocity.Y > 0 && Actor->mVelocity.Y == 0 )
				{
					Actor->Landed( CollisionInfo );
				}

				TimesliceLeft = ( 1.0f - CollisionInfo.FirstContact ) * TimesliceLeft;
				
				if( Counter++ > 32 )
				{
					ShouldAbort = true;
				}

				if( ShouldAbort )
				{
					break;
				}
			}
		}

		Actor->mCollision._Center = Actor->mLocation;

		return HitSomething;
	}
}