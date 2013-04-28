#include "Cat.h"
#include "foundation/memory.h"
#include <memory>
#include <hge/hge.h>
#include "Tileset.h"
#include "World.h"

#include "AARB.h"
#include "PhysicsWorld.h"

extern HGE* g_Hge;

using namespace foundation;

namespace Scribble
{
	Cat::Cat( Vector2 Location ) :
		Actor( Location ),
		mAcceleration( 0, 0 )
	{
		mSprite = MAKE_NEW( memory_globals::default_allocator(), hgeSprite, NULL, Location.X, Location.Y, 20, 20 );
		mSprite->SetHotSpot( 10, 10 );

		mCollision._Center = mLocation;
		mCollision._Extent = Vector2( 10, 10 );

		mCurrentPhysics = PHYS_Falling;
	}

	Cat::~Cat()
	{
		MAKE_DELETE( memory_globals::default_allocator(), hgeSprite, mSprite );
	}

	void Cat::Tick( float Dt )
	{
		mUpHold = g_Hge->Input_GetKeyState( HGEK_UP );
		mRightHold = g_Hge->Input_GetKeyState( HGEK_RIGHT );
		mDownHold = g_Hge->Input_GetKeyState( HGEK_DOWN );
		mLeftHold = g_Hge->Input_GetKeyState( HGEK_LEFT );

		mJumpPressed = g_Hge->Input_KeyDown( HGEK_SPACE );
	
		Actor::Tick( Dt );
	}

	void Cat::SimulatePhysics( float Dt )
	{
		switch( mCurrentPhysics )
		{
			case PHYS_Falling:
				PhysFalling( Dt );
				break;
			case PHYS_Walking:
				PhysWalking( Dt );
				break;
			default:
				break;
		}
	}

	void Cat::PhysFalling( float Dt )
	{
		static const float GRAVITY = 666.0f;

		mAcceleration.X = 0;
		mAcceleration.Y = 0;

		mAcceleration.Y += GRAVITY;

		mVelocity += mAcceleration * Dt;

		if( mRightHold )
		{
			mVelocity.X += 66.0f * Dt;
		}
		if( mLeftHold )
		{
			mVelocity.X -= 66.0f * Dt;
		}

		Vector2 DesiredDestination = mLocation + mVelocity * Dt + 0.5f * mAcceleration * Dt * Dt;

		g_World->MoveActor( this, DesiredDestination );
	}

	void Cat::Landed( const CollisionData& CollisionInfo )
	{
		if( CollisionInfo._Normal.Y < -0.8f )
		{
			mCurrentPhysics = PHYS_Walking;
		}
	}

	void Cat::PhysWalking( float Dt )
	{
		if( mJumpPressed )
		{
			mCurrentPhysics = PHYS_Falling;
			mVelocity.Y -= 300.0f;

			return;
		}

		Vector2 MoveDirection( 0, 0 );
		mVelocity.X = 0;
		mVelocity.Y = 0;

		mAcceleration.X = 0;
		mAcceleration.Y = 0;

		if( mRightHold )
		{
			MoveDirection.X = 1;
		}
		if( mLeftHold )
		{
			MoveDirection.X = -1;
		}

		mVelocity = MoveDirection * 150.0f;

 		g_World->MoveActor( this, mLocation + Vector2( 0, -5.0f ) );
		g_World->MoveActor( this, mLocation + MoveDirection * 300.0f * Dt );

		if( !g_World->MoveActor( this, mLocation + Vector2( 0, 5.2f ) ) )
		{
			mCurrentPhysics = PHYS_Falling;
		}
	}

	void Cat::Render()
	{
		if( mSprite != NULL )
		{
			mSprite->Render( mLocation.X, mLocation.Y );
		}
	}
}