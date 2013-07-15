#ifndef SCRIBBLE_COLLISIONCOMPONENT_H
#define SCRIBBLE_COLLISIONCOMPONENT_H

#include "Component.h"
#include "Vector2.h"
#include "AABB.h"
#include "World.h"

#include <box2d/Box2D.h>

namespace Scribble
{
	// @TODO: Make options to let the component drive the Actor, or the other way around
	class CollisionComponent : public Component
	{
		public:
			enum ESpace
			{
				S_World,
				S_Physics
			};
		public:
			CollisionComponent();
			~CollisionComponent();

			Vector2 GetLocation() const;
			Vector2 GetLinearVelocity() const;
			
			virtual void AttachedTo( Actor* NewOwner );
			virtual void DeattachedFrom( Actor* OldOwner );

			inline void GetAABB( AABB& out_AABB, ESpace InSpace = S_World )
			{
				out_AABB = mAABB;

				if( InSpace == S_Physics )
				{
					out_AABB *= TO_PHYSICS;
				}
			}

			static CollisionComponent* CreateCircle( b2BodyType BodyType, const Vector2& Location, float Radius );
			static CollisionComponent* CreateRectangle( b2BodyType BodyType, const Vector2& Location, float Width, float Height );
		private:
			friend class World;
			b2Body* mPhysicsBody;
			b2Fixture* mFixture;
			AABB mAABB;
	};
}

#endif