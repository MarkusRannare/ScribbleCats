#ifndef SCRIBBLE_ACTOR_H
#define SCRIBBLE_ACTOR_H

#include "foundation/collection_types.h"

namespace Scribble
{
	class World;
	struct TraceResult;
}

#include "PhysicsTypes.h"

extern Scribble::World* g_World;
class b2Body;

namespace Scribble
{
	class Component;
	class CollisionComponent;

	class Actor
	{
		public:
			enum EPhysics
			{
				PHYS_NONE,
				PHYS_Falling,
				PHYS_Walking
			};
		public:
			virtual ~Actor();

			virtual void Tick( float Dt );
			virtual void Render();

			bool SetLocation( const Vector2& Location );
			virtual void Landed( const TraceResult& CollisionInfo );

			void AttachComponent( Component* AComponent );
			void DeattachComponent( Component* AComponent );

			const Vector2& GetLocation() const;
		protected:
			virtual void SimulatePhysics( float Dt );
			/*virtual void PhysFalling( float Dt );*/
			Actor( Vector2 Location );
		private:
			Actor(); // - No default constructor
		private:
			friend class World;
		protected:
			Vector2 mLocation;
			Vector2 mVelocity;

			AARB mCollision;
			EPhysics mCurrentPhysics;
			
			foundation::Array<Component*> mAttachedComponents;
			CollisionComponent* mCollisionComponent;
	};
}

#endif