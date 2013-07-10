#include "CollisionComponent.h"

#include "Actor.h"
#include "World.h"

#include <cassert>

using namespace foundation;

namespace Scribble
{
	CollisionComponent::CollisionComponent()
	{
	}

	CollisionComponent::~CollisionComponent()
	{
		if( mPhysicsBody != NULL )
		{
			g_World->GetPhysicsWorld().DestroyBody( mPhysicsBody );
		}
	}

	CollisionComponent* CollisionComponent::CreateCircle( b2BodyType BodyType, float Radius )
	{
		assert( false && "Not implemented" );
		return NULL;
	}

	CollisionComponent* CollisionComponent::CreateRectangle( b2BodyType BodyType, float Width, float Height )
	{
		CollisionComponent* Component = MAKE_NEW( memory_globals::default_allocator(), CollisionComponent );

		b2BodyDef BodyDef;
		BodyDef.type = BodyType;
		BodyDef.gravityScale = BodyType == b2_kinematicBody ? 0.0f : 1.0f;
		BodyDef.fixedRotation = true;
		BodyDef.userData = Component;

		Component->mPhysicsBody = g_World->GetPhysicsWorld().CreateBody( &BodyDef );
		
		b2PolygonShape RectangleShape;
		RectangleShape.SetAsBox( Width * TO_PHYSICS, Height * TO_PHYSICS );

		Component->mFixture = Component->mPhysicsBody->CreateFixture( &RectangleShape, 1.0f );

		return Component;
	}

	void CollisionComponent::AttachedTo( Actor* NewOwner )
	{
		Component::AttachedTo( NewOwner );

		mPhysicsBody->SetTransform( VectorToB2( NewOwner->GetLocation() * TO_PHYSICS ), 0 );
	}

	void CollisionComponent::DeattachedFrom( Actor* OldOwner )
	{
		Component::DeattachedFrom( OldOwner );
	}

	Vector2 CollisionComponent::GetLocation() const
	{
		assert( mPhysicsBody != NULL && "Trying to get location from non initialized physics body" );

		b2Vec2 Location = mPhysicsBody->GetPosition();

		return B2ToVector( Location ) * TO_WORLD;
	}

	Vector2 CollisionComponent::GetLinearVelocity() const
	{
		assert( mPhysicsBody != NULL && "Trying to get linear velocity from non initialized physics body" );

		b2Vec2 LinearVelocity = mPhysicsBody->GetLinearVelocity();

		return B2ToVector( LinearVelocity ) * TO_WORLD;
	}
}