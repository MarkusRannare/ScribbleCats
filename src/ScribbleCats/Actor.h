#ifndef SCRIBBLE_ACTOR_H
#define SCRIBBLE_ACTOR_H

namespace Scribble
{
	class World;
}

#include "PhysicsTypes.h"

extern Scribble::World* g_World;

namespace Scribble
{
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
			virtual void Landed( const CollisionData& CollisionInfo );
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
	};
}

#endif