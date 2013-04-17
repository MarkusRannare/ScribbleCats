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
		float ClosestDistSq = 9999999999.9f;
		int ClosestIdx = -1;
		hgeRect* CollisionData = g_Tileset->GetCollisionForLayer( 0, NumCollisions );
		for( int Idx = 0; Idx < NumCollisions; ++Idx )
		{
			if( OurCollision.Intersect( &CollisionData[Idx] ) )
			{
				Collided = true;

				hgeVector CurrentCenter( CollisionData[Idx].x2 - CollisionData[Idx].x1, CollisionData[Idx].y2 - CollisionData[Idx].y1 );
				hgeVector Dir = CurrentCenter - mLocation;
				float CurrentDistSq = Dir.Dot( &Dir );
				if( CurrentDistSq < ClosestDistSq )
				{
					ClosestDistSq = CurrentDistSq;
					ClosestIdx = Idx;
				}

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
			mVelocity.x = 0;
			mVelocity.y = 0;
		}
		else
		{
			hgeRect& ClosestCollision = CollisionData[ClosestIdx];
			hgeVector CurrentCenter( ClosestCollision.x2 - ClosestCollision.x1, ClosestCollision.y2 - ClosestCollision.y1 );

			hgeVector CenterToLocation = mLocation - CurrentCenter;
			CenterToLocation.Normalize();

			static const hgeVector Up( 0.0f, -1.0f );
			static const hgeVector Right( 1.0f, 0.0f );
			static const hgeVector Down( 0.0f, 1.0f );
			static const hgeVector Left( -1.0f, 0.0f );

			hgeVector HitNormal;
			// Collision from top
			if( CenterToLocation.Dot( &Up ) > 0 )
			{
				HitNormal = Up;
			}
			else if( CenterToLocation.Dot( &Right ) > 0 )
			{
				HitNormal = Right;
			}
			else if( CenterToLocation.Dot( &Left ) > 0 )
			{
				HitNormal = Left;
			}
			else
			{
				HitNormal = Down;
			}

			// Don't move in the direction of the normal, just slide across the surface
			hgeVector MoveVect = DesiredDestination - mLocation;
			MoveVect += MoveVect.Dot( &HitNormal ) * -HitNormal;

			mLocation += MoveVect;

			// Remove the velocity in the direction of the hit normal
			mVelocity += mVelocity.Dot( &HitNormal ) * -HitNormal;			
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