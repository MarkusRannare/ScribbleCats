#ifndef SCRIBBLE_ACTOR_H
#define SCRIBBLE_ACTOR_H

#include "Object.h"

#include "foundation/collection_types.h"

namespace Scribble
{
	class World;
	struct TraceHit;
}

#include "PhysicsTypes.h"

extern Scribble::World* g_World;
class b2Body;

namespace Scribble
{
	class Component;
	class CollisionComponent;

	class Actor : public Object
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
			virtual void Landed( const TraceHit& CollisionInfo );

			void AttachComponent( Component* AComponent );
			void DeattachComponent( Component* AComponent );

			inline bool IsOwner( Object* Object );

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

			EPhysics mCurrentPhysics;
			
			foundation::Array<Component*> mComponents;
			CollisionComponent* mCollision;
	};

	inline bool Actor::IsOwner( Object* Object )
	{
		assert( Object );

		Actor* Owner = Object->GetOwner();

		while( Owner )
		{
			if( Owner == this )
			{
				return true;
			}

			Owner = Owner->GetOwner();
		}
		
		return false;
	}
}

#endif