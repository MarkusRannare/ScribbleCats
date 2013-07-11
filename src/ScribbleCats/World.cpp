#include "World.h"

#include "array_functions.h"
#include "PhysicsWorld.h"
#include "Box2DDebugDraw.h"
#include "CollisionComponent.h"
#include "foundation/temp_allocator.h"

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
		mComponents( memory_globals::default_allocator() ),
		mCollisionComponents( memory_globals::default_allocator() )
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

			if( CollisionComponent* ColCom = dynamic_cast<CollisionComponent*>( TheComponent ) )
			{
				array::push_back( mCollisionComponents, ColCom );
			}
		}
	}

	void World::ComponentDeattached( Component* TheComponent )
	{
		if( array::find( mComponents, TheComponent ) != INDEX_NONE )
		{
			array::remove_swap( mComponents, TheComponent );

			if( CollisionComponent* ColCom = dynamic_cast<CollisionComponent*>( TheComponent ) )
			{
				array::remove_swap( mCollisionComponents, ColCom );
			}
		}
	}

	void World::HandleNewAndOldActors()
	{
		for( size_t Idx = 0; Idx < array::size( mNewActors ); ++Idx )
		{
			array::push_back( mActors, mNewActors[Idx] );
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
		// @TODO: Just move the actor if it has no collision component
		assert( Actor != NULL );
		assert( false && "Not verified, this needs testing" );
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

					// Terminate query... something is already here
					return false;
				}

				bool mFoundAnyCollision;
		};

		// @TODO: Use collision component instead of mCollision as it's not supported anymore
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
		const TraceResult& CollisionInfo,
		float TimesliceRemaining,
		Vector2& out_NewWantedLocation )
	{
		Vector2 Displacement = TargetLocation - Actor->mLocation;

		Actor->mLocation += Displacement * ( CollisionInfo.HitTime - 0.001f );

		// Kill all velocity in the direction of the normal for our current actor
		Actor->mVelocity -= Actor->mVelocity.Dot( CollisionInfo.HitNormal ) * CollisionInfo.HitNormal;

		// Calculate our new wanted displacement without any movement in the direction of the collision normal
		Displacement -= Displacement.Dot( CollisionInfo.HitNormal ) * CollisionInfo.HitNormal;

		out_NewWantedLocation = Actor->mLocation + Displacement * ( 1.0f - CollisionInfo.HitTime ) * TimesliceRemaining;
		
		return Displacement == Vector2::ZERO;
	}

	static void CalculateQueryRegion( const Vector2& From, const Vector2& To, const Vector2& Extent, b2AABB& QueryRegion )
	{
		Vector2 MinLocation( min( From.X - Extent.X, To.X - Extent.X ), min( From.Y - Extent.Y, To.Y - Extent.Y ) );
		Vector2 MaxLocation( max( From.X + Extent.X, To.X + Extent.X ), max( From.Y + Extent.Y, To.Y + Extent.Y ) );
		QueryRegion.lowerBound.Set( MinLocation.X * TO_PHYSICS, MinLocation.Y * TO_PHYSICS );
		QueryRegion.upperBound.Set( MaxLocation.X * TO_PHYSICS, MaxLocation.Y * TO_PHYSICS );
	}

	// Compute the normal at the specified point.
	b2Vec2 ComputeNormal( const b2Shape& Shape, const b2Vec2& Point, const b2Transform& Xf )
	{
		// @TODO: Implement and verify
		assert( false && "Not actually tested at all! DO THIS NAO!" );

		if( Shape.GetType() == b2Shape::e_circle )
		{
			const b2CircleShape& Circle = static_cast<const b2CircleShape&>( Shape );
			b2Vec2 Normal = Point - b2Mul( Xf, /*circle.GetLocalPosition()*/ b2Vec2_zero );
			assert( false && "Not sure if the normal is calculated correctly, verify!" );
			Normal.Normalize();
			return Normal;
		}
		if( Shape.GetType() == b2Shape::e_polygon )
		{
			const b2PolygonShape& Poly = static_cast<const b2PolygonShape&>( Shape );
			int Count = Poly.GetVertexCount();
			//const b2Vec2* Vertices = Poly.m_vertices;// GetVertices();
			const b2Vec2* Normals = Poly.m_normals; // GetNormals();
			b2Vec2 P = b2MulT( Xf, Point );
			int NormalIndex = 0;
			float MaxSeparation = -b2_maxFloat;
			for( int Idx = 0; Idx < Count; ++Idx )
			{
				float Separation = b2Dot( P - Poly.GetVertex( Idx ), Normals[Idx] );
				if( Separation > MaxSeparation )
				{
					MaxSeparation = Separation;
					NormalIndex = Idx;
				}
			}
			return b2Mul( Xf.q, Normals[NormalIndex] );
		}
		if ( Shape.GetType() == b2Shape::e_edge )
		{
			assert( false && "Old code from old Box2D. Currently not compatible!" );
			/*const b2EdgeShape& Edge = static_cast<const b2EdgeShape&>( Shape );
			return b2Mul( Xf.R, Edge.GetNormalVector() );*/
		}
		assert( false );

		return b2Vec2_zero;
	}


	bool World::Trace( Actor* Source, const Vector2& From, const Vector2& To, const Vector2& Extent, TraceResult& Result, unsigned int Flags /* = 0 */ )
	{
		// @TODO: Should we allow tracing without a source, so that we don't ignore any collision from the trace?
		assert( Source );

		class TraceAABBCallback : public b2QueryCallback
		{
			public:
				// @TODO: Check if we should/need to do something about the allocator
				TraceAABBCallback( Actor* Source ) :
					HitComponents( Allocator ),
					Source( Source )
				{
				}

				// Called for each fixture found in the query AABB.
				// @return false to terminate the query.
				virtual bool ReportFixture( b2Fixture* fixture )
				{
					void* UserData = fixture->GetUserData();
					CollisionComponent* OwningComponent = (CollisionComponent*)UserData;

					// @TODO: Add other checks here, like, ownership checks
					if( OwningComponent && OwningComponent->GetOwner() != Source )
					{
						array::push_back( HitComponents, OwningComponent );
					}

					return true;
				}

				// Able to store 1024(512 in 64bit) results! This should be more than enough!
				TempAllocator4096 Allocator;
				Array<CollisionComponent*> HitComponents;
				Actor* Source;
		};

		// @TODO: See if there is a faster way to do the broadphase culling (b2DynamicTree?)
		b2AABB QueryRegion;
		CalculateQueryRegion( From, To, Extent, QueryRegion );
		TraceAABBCallback BroadphaseQuery( Source );
		mPhysicsWorld.QueryAABB( &BroadphaseQuery, QueryRegion );

		memset( &Result, 0, sizeof( TraceResult ) );

		// The box we will sweep
		b2PolygonShape SweepShape;
		SweepShape.SetAsBox( Extent.X * TO_PHYSICS, Extent.Y * TO_PHYSICS );

		// @TODO: Make cleaner setup for the sweeping
		// Setup the sweep
		b2TOIInput Input;
		Input.proxyA.Set( &SweepShape, 0 );
		Input.sweepA.alpha0 = 0;
		// @todo: Cache mass calculation or do it more efficient
		b2MassData Mass;
		SweepShape.ComputeMass( &Mass, 1.0f );
		Input.sweepA.localCenter = Mass.center;
		Input.sweepA.c0 = VectorToB2( From * TO_PHYSICS );
		Input.sweepA.c = VectorToB2( To * TO_PHYSICS );
		// don't sweep any rotation
		Input.sweepA.a0 = 0;
		Input.sweepA.a = 0;

		Input.tMax = 1.0f;

		bool HitSomething = false;
		b2TOIOutput FirstHit;
		FirstHit.t = 1.0001f;
		Array<CollisionComponent*>& TraceList = BroadphaseQuery.HitComponents;
		for( uint32_t Idx = 0; Idx < array::size( TraceList ); ++Idx )
		{
			CollisionComponent* TraceAgainst = TraceList[Idx];

			// @todo: Cache mass calculation or do it more efficient
			b2MassData Mass;
			TraceAgainst->mFixture->GetShape()->ComputeMass( &Mass, 1.0f );
			Input.proxyB.Set( TraceAgainst->mFixture->GetShape(), 0 );
			// Setup the other body as stationary
			Input.sweepB.alpha0 = 0;
			// @todo: Do we need to set this?
			Input.sweepB.localCenter = Mass.center;
			Input.sweepB.c0 = VectorToB2( TraceAgainst->GetLocation() * TO_PHYSICS );
			Input.sweepB.c = VectorToB2( TraceAgainst->GetLocation() * TO_PHYSICS );
			// don't sweep any rotation
			Input.sweepB.a0 = 0;
			Input.sweepB.a = 0;

			b2TOIOutput Output;

			b2TimeOfImpact( &Output, &Input );

			// @TODO: Handle other cases than touching...
			if( Output.state == b2TOIOutput::e_touching && Output.t < FirstHit.t )
			{
				Vector2 HitLocation = Vector2::Lerp( From, To, Output.t );
				Vector2 HitNormal = B2ToVector( ComputeNormal( *TraceAgainst->mFixture->GetShape(), VectorToB2( HitLocation * TO_PHYSICS ), TraceAgainst->mPhysicsBody->GetTransform() ) );

				// @TODO: Mabee... Move this somewhere else? A bit deep into the calculations right now
				// Ignore if we hit something at the initial time if we are moving along it or away from it
				// [<-][  ]	( This is no longer any collision Collision )
				// [->]		( Neither is this )
				// [  ] 
				float HitDot = HitNormal.Dot( Vector2::Normalize( To - From ) );
				if( Output.t <= 0 && ( HitDot == 0.0f || HitDot == 1.0f ) )
				{
					continue;
				}

				FirstHit = Output;
				Result.HitComponent = TraceAgainst;
				Result.HitActor = TraceAgainst->GetOwner();
				Result.HitLocation = HitLocation;
				Result.HitNormal = HitNormal;
				Result.HitTime = Output.t;
				HitSomething = true;
			}
		}

		return HitSomething;
	}

	bool World::MoveActor( Actor* Actor, const Vector2& ToLocation )
	{
		// No collision for the physics body, just move it there
		if( Actor->mCollisionComponent == NULL )
		{
			Actor->mLocation = ToLocation;
			return false;
		}

		bool HitSomething = false;
		Vector2 TargetLocation = ToLocation;
		Vector2 FromLocation = Actor->mLocation;
		float TimesliceLeft = 1.0f;

		const float TIMESLICE_EPSILON = 0.00001f;
		int Counter = 0;

		Vector2 OldVelocity = Actor->mVelocity;

		while( TimesliceLeft > TIMESLICE_EPSILON )
		{
			TraceResult Result;
			bool HitObject = Trace( Actor, FromLocation, TargetLocation, Vector2( 32, 28 ), Result );
					
			if( !HitObject )
			{
				Actor->mLocation = TargetLocation;
				
				break;
			}
			else
			{
				HitSomething = true;

				TimesliceLeft = ( 1.0f - Result.HitTime ) * TimesliceLeft;

				Vector2 NewTargetLocation;
				bool ShouldAbort = ResolveCollision( Actor, TargetLocation, Result, TimesliceLeft, NewTargetLocation );
				TargetLocation = NewTargetLocation;

				// We have possible landed
				if( OldVelocity.Y > 0 && Actor->mVelocity.Y == 0 )
				{
					Actor->Landed( Result );
				}

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

		return HitSomething;
	}
}