#include "Actor.h"
#include "string.h"
#include "World.h"

namespace Scribble
{
	Actor::Actor( Vector2 Location ) :
		mLocation( Location ),
		mVelocity( 0, 0 ),
		mCurrentPhysics( PHYS_NONE )
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

	void Actor::Landed( const CollisionData& CollisionInfo )
	{
	}
}