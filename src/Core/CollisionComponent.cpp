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

	CollisionComponent* CollisionComponent::CreateCircle( b2BodyType BodyType, const Vector2& Location, float Radius )
	{
		CollisionComponent* Component = MAKE_NEW( memory_globals::default_allocator(), CollisionComponent );

		b2BodyDef BodyDef;
		BodyDef.type = BodyType;
		BodyDef.fixedRotation = true;
		BodyDef.userData = Component;
		BodyDef.position.Set( Location.X * TO_PHYSICS, Location.Y * TO_PHYSICS );

		Component->mPhysicsBody = g_World->GetPhysicsWorld().CreateBody( &BodyDef );
		
		b2CircleShape CircleShape;
		CircleShape.m_radius = Radius * TO_PHYSICS;

		Component->mFixture = Component->mPhysicsBody->CreateFixture( &CircleShape, 1.0f );
		Component->mFixture->SetUserData( Component );

		aabb::CreateBox( Location, Radius, Component->mAABB );

		return Component;
	}

	CollisionComponent* CollisionComponent::CreateRectangle( b2BodyType BodyType, const Vector2& Location, float Width, float Height )
	{
		CollisionComponent* Component = MAKE_NEW( memory_globals::default_allocator(), CollisionComponent );

		b2BodyDef BodyDef;
		BodyDef.type = BodyType;
		BodyDef.fixedRotation = true;
		BodyDef.userData = Component;
		BodyDef.position.Set( Location.X * TO_PHYSICS, Location.Y * TO_PHYSICS );

		Component->mPhysicsBody = g_World->GetPhysicsWorld().CreateBody( &BodyDef );
		
		b2PolygonShape RectangleShape;
		RectangleShape.SetAsBox( Width / 2.0f * TO_PHYSICS, Height / 2.0f * TO_PHYSICS );

		Component->mFixture = Component->mPhysicsBody->CreateFixture( &RectangleShape, 1.0f );
		Component->mFixture->SetUserData( Component );

		aabb::CreateRectangle( Location, Width, Height, Component->mAABB );

		return Component;
	}

	void CollisionComponent::AttachedTo( Actor* NewOwner )
	{
		Component::AttachedTo( NewOwner );
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
	
	void CollisionComponent::Tick( float Dt )
	{
		if( mPhysicsBody->GetType() == b2_kinematicBody )
		{
			mPhysicsBody->SetTransform( VectorToB2( mOwner->GetLocation() * TO_PHYSICS ), 0.0f );
		}
	}
}