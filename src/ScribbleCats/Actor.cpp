#include "Actor.h"
#include "string.h"
#include "World.h"
#include "array_functions.h"

using namespace foundation;

namespace Scribble
{
	Actor::Actor( Vector2 Location ) :
		mLocation( Location ),
		mVelocity( 0, 0 ),
		mCurrentPhysics( PHYS_NONE ),
		mPhysicsBody( NULL ),
		mAttachedComponents( memory_globals::default_allocator() )
	{
		memset( &mCollision, 0, sizeof( mCollision ) );		
	}

	Actor::~Actor()
	{
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

	void Actor::Landed( const CollisionData& CollisionInfo )
	{
	}

	void Actor::AttachComponent( Component* AComponent )
	{
		 int FoundIndex = array::find( mAttachedComponents, AComponent );

		 if( FoundIndex == INDEX_NONE )
		 {
			array::push_back( mAttachedComponents, AComponent );
		 }
	}

	void Actor::DeattachComponent( Component* AComponent )
	{
		int FoundIndex = array::find( mAttachedComponents, AComponent );

		if( FoundIndex != INDEX_NONE )
		{
			array::remove_swap( mAttachedComponents, AComponent );
		}
	}
}