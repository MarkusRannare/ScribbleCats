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
	Cat::Cat( Vector2 Location ) :
		Actor( Location ),
		mCurrentPhysics( PHYS_Falling ),
		mVelocity( 0, 0 ),
		mAcceleration( 0, 0 )
	{
		mSprite = MAKE_NEW( memory_globals::default_allocator(), hgeSprite, NULL, Location.X, Location.Y, 20, 20 );
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

	AARB DL;
	AARB CollidingBlock;
	AARB FeetCol;

	void Cat::PhysWalking( float Dt )
	{
		Vector2 MoveDirection( 0, 0 );

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
		
		mVelocity = MoveDirection * 100.0f;
		if( mJumpPressed )
		{
			mCurrentPhysics = PHYS_Falling;
			mVelocity.Y -= 300.0f;
		}

		Vector2 DesiredDestination = mLocation + mVelocity * Dt + 0.5f * mAcceleration * Dt * Dt;

		AARB OldCollision;
		aarb::CreateBox( mLocation, mSprite->GetWidth(), OldCollision );
		AARB OurCollision;
		aarb::CreateBox( DesiredDestination, mSprite->GetWidth(), OurCollision );

		DL = OurCollision;
		
		// Feet collision is five units belove our character
		AARB FeetCollision;
		// Same as character
		FeetCollision = OurCollision;
		// Move center down to bottom of character + 2.5f units
		FeetCollision._Center.Y += FeetCollision._Extent.Y + 2.5f;
		// Make y extent endend down to 5 units below it
		FeetCollision._Extent.Y = 5.0f;

		FeetCol = FeetCollision;

		extern Tileset* g_Tileset;

		bool Collided = false;

		bool ColideBelowFeet = false;
		int NumCollisions = 0;
		AARB* CollisionData = g_Tileset->GetCollisionForLayer( 0, NumCollisions );
		for( int Idx = 0; Idx < NumCollisions; ++Idx )
		{
			if( aarb::Intersects( OurCollision, CollisionData[Idx] ) )
			{
				Collided = true;
				CollidingBlock = CollisionData[Idx];
			}
			else if( aarb::Intersects( FeetCollision, CollisionData[Idx] ) )
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

		mAcceleration.X = 0;
		mAcceleration.Y = 0;

		mAcceleration.Y += GRAVITY;

		mVelocity += mAcceleration * Dt;
			
		if( mRightHold )
		{
			mVelocity.X += 66.6f * Dt;
		}
		if( mLeftHold )
		{
			mVelocity.X -= 66.6f * Dt;
		}

		Vector2 DesiredDestination = mLocation + mVelocity * Dt + 0.5f * mAcceleration * Dt * Dt;
		
		AARB OldCollision;
		aarb::CreateBox( mLocation, mSprite->GetWidth(), OldCollision );
		AARB OurCollision;
		aarb::CreateBox( DesiredDestination, mSprite->GetWidth(), OurCollision );

		DL = OurCollision;
		
		extern Tileset* g_Tileset;

		bool Collided = false;
		bool ColideBelowFeet = false;
		int NumCollisions = 0;
		float ClosestDistSq = 9999999999.9f;
		int ClosestIdx = -1;
		AARB* CollisionData = g_Tileset->GetCollisionForLayer( 0, NumCollisions );
		for( int Idx = 0; Idx < NumCollisions; ++Idx )
		{
			if( aarb::Intersects( OurCollision, CollisionData[Idx] ) )
			{
				Collided = true;

				Vector2 CurrentCenter = CollisionData[Idx]._Center;
				Vector2 Dir = CurrentCenter - mLocation;
				float CurrentDistSq = Dir.Dot( Dir );
				if( CurrentDistSq < ClosestDistSq )
				{
					ClosestDistSq = CurrentDistSq;
					ClosestIdx = Idx;
					CollidingBlock = CollisionData[Idx];
				}
				
				// We have penetrated the thing from above
				if( ( OurCollision._Center.Y + OurCollision._Extent.Y ) > ( CollisionData[Idx]._Center.Y - CollisionData[Idx]._Extent.Y )
					// And didn't do it before the move 
					&& ( OldCollision._Center.Y - OldCollision._Center.Y ) < ( CollisionData[Idx]._Center.Y + CollisionData[Idx]._Extent.Y ) )
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
			mVelocity.X = 0;
			mVelocity.Y = 0;
		}
		else
		{
			AARB& ClosestCollision = CollisionData[ClosestIdx];
			Vector2 CurrentCenter = ClosestCollision._Center;

			Vector2 CenterToLocation = mLocation - CurrentCenter;
			CenterToLocation.Normalize();

			static const Vector2 Up( 0.0f, -1.0f );
			static const Vector2 Right( 1.0f, 0.0f );
			static const Vector2 Down( 0.0f, 1.0f );
			static const Vector2 Left( -1.0f, 0.0f );

			Vector2 HitNormal;
			// Collision from top
			if( CenterToLocation.Dot( Up ) > 0 )
			{
				HitNormal = Up;
			}
			else if( CenterToLocation.Dot( Right ) > 0 )
			{
				HitNormal = Right;
			}
			else if( CenterToLocation.Dot( Left ) > 0 )
			{
				HitNormal = Left;
			}
			else
			{
				HitNormal = Down;
			}

			// Don't move in the direction of the normal, just slide across the surface
			Vector2 MoveVect = DesiredDestination - mLocation;
			MoveVect += MoveVect.Dot( HitNormal ) * -HitNormal;

			mLocation += MoveVect;

			// Remove the velocity in the direction of the hit normal
			mVelocity += mVelocity.Dot( HitNormal ) * -HitNormal;			
		}
	}

	void DrawAARB( const AARB& A, DWORD Color )
	{
		// Top line
		g_Hge->Gfx_RenderLine( A._Center.X - A._Extent.X, A._Center.Y - A._Extent.Y, 
			A._Center.X + A._Extent.X, A._Center.Y - A._Extent.Y,
			Color );
		// Right line
		g_Hge->Gfx_RenderLine( A._Center.X + A._Extent.X, A._Center.Y - A._Extent.Y, 
			A._Center.X + A._Extent.X, A._Center.Y + A._Extent.Y,
			Color );

		// Bottom line
		g_Hge->Gfx_RenderLine( A._Center.X + A._Extent.X, A._Center.Y + A._Extent.Y, 
			A._Center.X - A._Extent.X, A._Center.Y + A._Extent.Y,
			Color );

		// Left line
		g_Hge->Gfx_RenderLine( A._Center.X - A._Extent.X, A._Center.Y + A._Extent.Y, 
			A._Center.X - A._Extent.X, A._Center.Y - A._Extent.Y,
			Color );
	}

	void Cat::Render()
	{
		if( mSprite != NULL )
		{
			mSprite->Render( mLocation.X, mLocation.Y );

			//DrawAARB( OL, 0xffff0000 );
			DrawAARB( DL, 0xff00ff00 );
			DrawAARB( CollidingBlock, 0xffff0000 );
			DrawAARB( FeetCol, 0xff0000ff );
		}
	}
}