#include "Cat.h"
#include "foundation/memory.h"
#include <memory>
#include <hge/hge.h>
#include "Tileset.h"
#include "World.h"

#include "AARB.h"
#include "PhysicsWorld.h"
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

		mCollisionComponent = CollisionComponent::CreateRectangle( b2_kinematicBody, 28.0f, 31.0f );
		AttachComponent( mCollisionComponent );
		
		/*b2BodyDef BodyDef;
		BodyDef.position = VectorToB2( Location * TO_PHYSICS );
		BodyDef.type = b2_kinematicBody;
		BodyDef.fixedRotation = true;
		BodyDef.gravityScale = 0.00000f;
		
		mPhysicsBody = g_World->GetPhysicsWorld().CreateBody( &BodyDef );

		b2PolygonShape PolygonShape;
		PolygonShape.SetAsBox( 28 * TO_PHYSICS, 31 * TO_PHYSICS );

		mPhysicsBody->CreateFixture( &PolygonShape, 1.0f );*/

		mWalkLeft->Play();
		mWalkRight->Play();

		mCurrentAnim = mWalkRight;
		
		mCurrentPhysics = PHYS_Falling;
	}

	Cat::~Cat()
	{
		if( mCollisionComponent )
		{
			MAKE_DELETE( memory_globals::default_allocator(), CollisionComponent, mCollisionComponent ); 
		}
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

		if( mJumpPressed )
		{
			mVelocity.Y -= 300.0f;
		}

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
			mVelocity.Y -= 300.0f;
			mVelocity.X = MoveDirection.X * 150.0f;

			mCurrentPhysics = PHYS_Falling;
			SimulatePhysics( Dt );

			return;
		}

		mVelocity = MoveDirection * 150.0f;

 		g_World->MoveActor( this, mLocation + Vector2( 0, -5.0f ) );
		g_World->MoveActor( this, mLocation + MoveDirection * 300.0f * Dt );

		if( !g_World->MoveActor( this, mLocation + Vector2( 0, 5.1f ) ) )
		{
			mCurrentPhysics = PHYS_Falling;
		}
	}

	void Cat::Render()
	{
		if( mCurrentAnim != NULL )
		{
			mCurrentAnim->RenderEx( mLocation.X, mLocation.Y, 0, 0.25f, 0.25f );
		}
		/*extern bool g_DebugRenderPhysics;
		if( g_DebugRenderPhysics )
		{
			DrawAARB( mCollision, 0xffffff00 );
		}*/
	}
}