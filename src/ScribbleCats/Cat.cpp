#include "Cat.h"
#include "foundation/memory.h"
#include <memory>
#include <hge/hge.h>
#include "Tileset.h"

extern HGE* g_Hge;

using namespace foundation;

namespace Scribble
{
	Cat::Cat( hgeVector Location ) :
		Actor( Location ),
		mCurrentPhysics( PHYS_Falling ),
		mVelocity( 0, 0 ),
		mAcceleration( 0, 0 )
	{
		mSprite = MAKE_NEW( memory_globals::default_allocator(), hgeSprite, NULL, Location.x, Location.y, 20, 20 );
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
	
		SimulatePhysics( Dt );
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

	void Cat::PhysWalking( float Dt )
	{
		hgeVector MoveDirection( 0, 0 );

		mAcceleration.x = 0;
		mAcceleration.y = 0;

		if( mRightHold )
		{
			MoveDirection.x = 1;
		}
		if( mLeftHold )
		{
			MoveDirection.x = -1;
		}
		
		mVelocity = MoveDirection * 100.0f;
		if( mJumpPressed )
		{
			mCurrentPhysics = PHYS_Falling;
			mVelocity.y -= 300.0f;
		}

		hgeVector DesiredDestination = mLocation + mVelocity * Dt + 0.5f * mAcceleration * Dt * Dt;

		hgeRect OldCollision( mLocation.x, mLocation.y, mLocation.x + mSprite->GetWidth(), mLocation.y + mSprite->GetHeight() );
		hgeRect OurCollision( DesiredDestination.x, DesiredDestination.y, DesiredDestination.x + mSprite->GetWidth(), DesiredDestination.y + mSprite->GetHeight() );
		hgeRect FeetCollision( OurCollision.x1, OurCollision.y2, OurCollision.x2, OurCollision.y2 + 5 );

		extern Tileset* g_Tileset;

		bool Collided = false;

		bool ColideBelowFeet = false;
		int NumCollisions = 0;
		hgeRect* CollisionData = g_Tileset->GetCollisionForLayer( 0, NumCollisions );
		for( int Idx = 0; Idx < NumCollisions; ++Idx )
		{
			if( OurCollision.Intersect( &CollisionData[Idx] ) )
			{
				Collided = true;
			}
			else if( FeetCollision.Intersect( &CollisionData[Idx] ) )
			{
				ColideBelowFeet = true;

				// If we have collided and have collision below our feets, then we don't need any more collision tests
				if( Collided )
				{
					break;
				}
			}
		}

		if( !ColideBelowFeet )
		{
			mCurrentPhysics = PHYS_Falling;
		}
		else if( !Collided )
		{
			mLocation = DesiredDestination;
		}
	}

	void Cat::PhysFalling( float Dt )
	{
		static const float GRAVITY = 666.0f;

		mAcceleration.x = 0;
		mAcceleration.y = 0;

		mAcceleration.y += GRAVITY;

		mVelocity += mAcceleration * Dt;
			
		if( mRightHold )
		{
			mVelocity.x += 66.6f * Dt;
		}
		if( mLeftHold )
		{
			mVelocity.x -= 66.6f * Dt;
		}

		hgeVector DesiredDestination = mLocation + mVelocity * Dt + 0.5f * mAcceleration * Dt * Dt;
		
		hgeRect OldCollision( mLocation.x, mLocation.y, mLocation.x + mSprite->GetWidth(), mLocation.y + mSprite->GetHeight() );
		hgeRect OurCollision( DesiredDestination.x, DesiredDestination.y, DesiredDestination.x + mSprite->GetWidth(), DesiredDestination.y + mSprite->GetHeight() );
		
		extern Tileset* g_Tileset;

		bool Collided = false;
		bool ColideBelowFeet = false;
		int NumCollisions = 0;
		hgeRect* CollisionData = g_Tileset->GetCollisionForLayer( 0, NumCollisions );
		for( int Idx = 0; Idx < NumCollisions; ++Idx )
		{
			if( OurCollision.Intersect( &CollisionData[Idx] ) )
			{
				Collided = true;
				// We have penetrated the thing from above
				if( CollisionData[Idx].y1 < OurCollision.y2
					// And didn't do it before the move 
					&& CollisionData[Idx].y1 > OldCollision.y2 )
				{
					ColideBelowFeet = true;
					break;
				}
			}
		}
				
		if( !Collided )
		{
			mLocation = DesiredDestination;
		}
		else if( ColideBelowFeet )
		{
			mCurrentPhysics = PHYS_Walking;
		}
	}

	void Cat::Render()
	{
		if( mSprite != NULL )
		{
			mSprite->Render( mLocation.x, mLocation.y );
		}
	}
}