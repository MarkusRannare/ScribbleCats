#include "Cat.h"
#include "foundation/memory.h"
#include <memory>
#include <hge/hge.h>
#include "Tileset.h"

#include "AARB.h"

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
		mSprite->SetHotSpot( 10, 10 );
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

	Physics::AARB DL;
	Physics::AARB CollidingBlock;
	Physics::AARB FeetCol;

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

		Physics::AARB OldCollision;
		Physics::CreateBox( mLocation, mSprite->GetWidth(), OldCollision );
		Physics::AARB OurCollision;
		Physics::CreateBox( DesiredDestination, mSprite->GetWidth(), OurCollision );

		DL = OurCollision;
		
		// Feet collision is five units belove our character
		Physics::AARB FeetCollision;
		// Same as character
		FeetCollision = OurCollision;
		// Move center down to bottom of character + 2.5f units
		FeetCollision._Center.y += FeetCollision._Extent.y + 2.5f;
		// Make y extent endend down to 5 units below it
		FeetCollision._Extent.y = 5.0f;

		FeetCol = FeetCollision;

		extern Tileset* g_Tileset;

		bool Collided = false;

		bool ColideBelowFeet = false;
		int NumCollisions = 0;
		Physics::AARB* CollisionData = g_Tileset->GetCollisionForLayer( 0, NumCollisions );
		for( int Idx = 0; Idx < NumCollisions; ++Idx )
		{
			if( Physics::Intersects( OurCollision, CollisionData[Idx] ) )
			{
				Collided = true;
				CollidingBlock = CollisionData[Idx];
			}
			else if( Physics::Intersects( FeetCollision, CollisionData[Idx] ) )
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
		
		Physics::AARB OldCollision;
		Physics::CreateBox( mLocation, mSprite->GetWidth(), OldCollision );
		Physics::AARB OurCollision;
		Physics::CreateBox( DesiredDestination, mSprite->GetWidth(), OurCollision );

		DL = OurCollision;
		
		extern Tileset* g_Tileset;

		bool Collided = false;
		bool ColideBelowFeet = false;
		int NumCollisions = 0;
		float ClosestDistSq = 9999999999.9f;
		int ClosestIdx = -1;
		Physics::AARB* CollisionData = g_Tileset->GetCollisionForLayer( 0, NumCollisions );
		for( int Idx = 0; Idx < NumCollisions; ++Idx )
		{
			if( Physics::Intersects( OurCollision, CollisionData[Idx] ) )
			{
				Collided = true;

				hgeVector CurrentCenter = CollisionData[Idx]._Center;
				hgeVector Dir = CurrentCenter - mLocation;
				float CurrentDistSq = Dir.Dot( &Dir );
				if( CurrentDistSq < ClosestDistSq )
				{
					ClosestDistSq = CurrentDistSq;
					ClosestIdx = Idx;
					CollidingBlock = CollisionData[Idx];
				}
				
				// We have penetrated the thing from above
				if( ( OurCollision._Center.y + OurCollision._Extent.y ) > ( CollisionData[Idx]._Center.y - CollisionData[Idx]._Extent.y )
					// And didn't do it before the move 
					&& ( OldCollision._Center.y - OldCollision._Center.y ) < ( CollisionData[Idx]._Center.y + CollisionData[Idx]._Extent.y ) )
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
			Physics::AARB& ClosestCollision = CollisionData[ClosestIdx];
			hgeVector CurrentCenter = ClosestCollision._Center;

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

	void DrawAARB( const Physics::AARB& A, DWORD Color )
	{
		// Top line
		g_Hge->Gfx_RenderLine( A._Center.x - A._Extent.x, A._Center.y - A._Extent.y, 
			A._Center.x + A._Extent.x, A._Center.y - A._Extent.y,
			Color );
		// Right line
		g_Hge->Gfx_RenderLine( A._Center.x + A._Extent.x, A._Center.y - A._Extent.y, 
			A._Center.x + A._Extent.x, A._Center.y + A._Extent.y,
			Color );

		// Bottom line
		g_Hge->Gfx_RenderLine( A._Center.x + A._Extent.x, A._Center.y + A._Extent.y, 
			A._Center.x - A._Extent.x, A._Center.y + A._Extent.y,
			Color );

		// Left line
		g_Hge->Gfx_RenderLine( A._Center.x - A._Extent.x, A._Center.y + A._Extent.y, 
			A._Center.x - A._Extent.x, A._Center.y - A._Extent.y,
			Color );
	}

	void Cat::Render()
	{
		if( mSprite != NULL )
		{
			mSprite->Render( mLocation.x, mLocation.y );

			//DrawAARB( OL, 0xffff0000 );
			DrawAARB( DL, 0xff00ff00 );
			DrawAARB( CollidingBlock, 0xffff0000 );
			DrawAARB( FeetCol, 0xff0000ff );
		}
	}
}