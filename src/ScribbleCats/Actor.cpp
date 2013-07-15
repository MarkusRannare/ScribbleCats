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
		mComponents( memory_globals::default_allocator() )
	{
	}

	Actor::~Actor()
	{
		for( int32_t Idx = array::size( mComponents ) - 1; Idx >= 0; --Idx )
		{
			Component* Comp = mComponents[Idx];

			DeattachComponent( Comp );
			// @TODO: Make a better way to deallocate components
			MAKE_DELETE( memory_globals::default_allocator(), Component, Comp );
			array::pop_back( mComponents );
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

	void Actor::Landed( const TraceHit& CollisionInfo )
	{
	}

	void Actor::AttachComponent( Component* AComponent )
	{
		 int FoundIndex = array::find( mComponents, AComponent );

		 if( FoundIndex == INDEX_NONE )
		 {
			array::push_back( mComponents, AComponent );
			AComponent->AttachedTo( this );
		 }
	}

	void Actor::DeattachComponent( Component* AComponent )
	{
		int FoundIndex = array::find( mComponents, AComponent );

		if( FoundIndex != INDEX_NONE )
		{
			array::remove_swap( mComponents, AComponent );
			AComponent->DeattachedFrom( this );
		}
	}
}