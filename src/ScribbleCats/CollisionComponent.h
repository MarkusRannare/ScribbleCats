#ifndef SCRIBBLE_COLLISIONCOMPONENT_H
#define SCRIBBLE_COLLISIONCOMPONENT_H

#include "Component.h"
#include "Vector2.h"

#include <box2d/Box2D.h>

namespace Scribble
{
	class CollisionComponent : public Component
	{
		public:
			CollisionComponent();
			~CollisionComponent();

			Vector2 GetLocation() const;
			Vector2 GetLinearVelocity() const;
			
			virtual void AttachedTo( Actor* NewOwner );
			virtual void DeattachedFrom( Actor* OldOwner );

			static CollisionComponent* CreateCircle( b2BodyType BodyType, float Radius );
			static CollisionComponent* CreateRectangle( b2BodyType BodyType, const Vector2& Location, float Width, float Height );
		public:
			b2Body* mPhysicsBody;
			b2Fixture* mFixture;
	};
}

#endif