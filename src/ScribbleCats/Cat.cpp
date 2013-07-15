#include "Cat.h"
#include "foundation/memory.h"
#include <memory>
#include <hge/hge.h>
#include "Tileset.h"
#include "World.h"

#include "AABB.h"
#include <hge/hgeresource.h>
#include "box2d\Box2D.h"
#include "CollisionComponent.h"

extern HGE* g_Hge;
extern hgeResourceManager* g_ResManager;

using namespace foundation;

namespace Scribble
{
	Cat::Cat( Vector2 Location ) :
		Actor( Location ),
		mAcceleration( 0, 0 )
	{
		mWalkLeft = g_ResManager->GetAnimation( "Player.WalkLeft" );
		mWalkRight = g_ResManager->GetAnimation( "Player.WalkRight" );

		mCollision = CollisionComponent::CreateRectangle( b2_kinematicBody, Location, 52.0f, 58.0f );
		AttachComponent( mCollision );
		
		mWalkLeft->Play();
		mWalkRight->Play();

		mCurrentAnim = mWalkRight;
		
		mCurrentPhysics = PHYS_Falling;
	}

	Cat::~Cat()
	{
	}

	void Cat::Tick( float Dt )
	{
		mUpHold = g_Hge->Input_GetKeyState( HGEK_UP );
		mRightHold = g_Hge->Input_GetKeyState( HGEK_RIGHT );
		mDownHold = g_Hge->Input_GetKeyState( HGEK_DOWN );
		mLeftHold = g_Hge->Input_GetKeyState( HGEK_LEFT );

		mJumpPressed = g_Hge->Input_KeyDown( HGEK_SPACE );
	
		if( g_Hge->Input_KeyDown( HGEK_E ) )
		{
			mLocation = Vector2( 200, 50 );
			mVelocity = Vector2( 0, 0 );
		}

		Actor::Tick( Dt );

		mWalkLeft->Update( Dt );
		mWalkRight->Update( Dt );
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

		/*if( mJumpPressed )
		{
			mVelocity.Y -= 300.0f;
		}*/

		if( mRightHold )
		{
			mVelocity.X += 110.0f * Dt;
			mCurrentAnim = mWalkRight;
		}
		if( mLeftHold )
		{
			mVelocity.X -= 110.0f * Dt;
			mCurrentAnim = mWalkLeft;
		}

		Vector2 DesiredDestination = mLocation + mVelocity * Dt + 0.5f * mAcceleration * Dt * Dt;

		g_World->MoveActor( this, mLocation, DesiredDestination );
	}

	void Cat::Landed( const TraceHit& CollisionInfo )
	{
		if( CollisionInfo.HitNormal.Y < -0.8f )
		{
			mCurrentPhysics = PHYS_Walking;
		}
	}

	void Cat::PhysWalking( float Dt )
	{
		Vector2 MoveDirection( 0, 0 );
		mVelocity.X = 0;
		mVelocity.Y = 0;

		mAcceleration.X = 0;
		mAcceleration.Y = 0;

		if( mRightHold )
		{
			MoveDirection.X = 1;
			mCurrentAnim = mWalkRight;
		}
		if( mLeftHold )
		{
			MoveDirection.X = -1;
			mCurrentAnim = mWalkLeft;
		}

		if( mJumpPressed )
		{
			mVelocity.Y -= 450.0F;
			mVelocity.X = MoveDirection.X * 150.0f;

			mCurrentPhysics = PHYS_Falling;
			SimulatePhysics( Dt );

			return;
		}

		mVelocity = MoveDirection * 150.0f;

		if( mVelocity != Vector2::ZERO )
		{
			// Try step up 5 units
			bool HitSomething = g_World->MoveActor( this, mLocation + Vector2( 0, 1.0f ) * b2_polygonRadius, mLocation + Vector2( 0, -5.0f ) );

			// Move forward 300 units
			g_World->MoveActor( this, mLocation, mLocation + MoveDirection * 300.0f * Dt );

			// Try step down 5 units from original location
			if( !g_World->MoveActor( this, mLocation, mLocation + Vector2( 0, 10.0f ) ) )
			{
				mCurrentPhysics = PHYS_Falling;
			}
		}
	}

	void Cat::Render()
	{
		if( mCurrentAnim != NULL )
		{
			mCurrentAnim->RenderEx( mLocation.X, mLocation.Y, 0, 0.25f, 0.25f );
		}
	}
}