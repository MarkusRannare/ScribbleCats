#include "Actor.h"
#include "string.h"
#include "World.h"
#include "array_functions.h"
#include "Component.h"

using namespace foundation;

namespace Scribble
{
	Actor::Actor( Vector2 Location ) :
		mLocation( Location ),
		mVelocity( 0, 0 ),
		mCurrentPhysics( PHYS_NONE ),
		mAttachedComponents( memory_globals::default_allocator() )
	{
		memset( &mCollision, 0, sizeof( mCollision ) );		
	}

	Actor::~Actor()
	{
		for( int32_t Idx = array::size( mAttachedComponents ) - 1; Idx >= 0; --Idx )
		{
			Component* Comp = mAttachedComponents[Idx];

			DeattachComponent( Comp );
			// @TODO: Make a better way to deallocate components
			MAKE_DELETE( memory_globals::default_allocator(), Component, Comp );
			array::pop_back( mAttachedComponents );
		}
	}

	void Actor::Tick( float Dt )
	{
		SimulatePhysics( Dt );
	}

	void Actor::SimulatePhysics( float Dt )
	{
		Dt = Dt;
	}

	void Actor::Render()
	{
	}

	bool Actor::SetLocation( const Vector2& Location )
	{
		return g_World->SetActorLocation( this, Location );
	}

	const Vector2& Actor::GetLocation() const
	{
		return mLocation;
	}

	void Actor::Landed( const TraceResult& CollisionInfo )
	{
	}

	void Actor::AttachComponent( Component* AComponent )
	{
		 int FoundIndex = array::find( mAttachedComponents, AComponent );

		 if( FoundIndex == INDEX_NONE )
		 {
			array::push_back( mAttachedComponents, AComponent );
			AComponent->AttachedTo( this );
		 }
	}

	void Actor::DeattachComponent( Component* AComponent )
	{
		int FoundIndex = array::find( mAttachedComponents, AComponent );

		if( FoundIndex != INDEX_NONE )
		{
			array::remove_swap( mAttachedComponents, AComponent );
			AComponent->DeattachedFrom( this );
		}
	}
}