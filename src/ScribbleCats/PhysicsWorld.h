#ifndef SCRIBBLE_PHYSICS_WORLD_H
#define SCRIBBLE_PHYSICS_WORLD_H

#include "foundation/collection_types.h"

namespace Scribble
{
	class Shape;
	struct Body;

	class PhysicsWorld
	{
		public:
			PhysicsWorld();
			~PhysicsWorld();

			Body* CreateBody( Shape* Shape );
			void DestroyBody( Body* Body );
		private:
			foundation::Array<Body*> mBodies;
	};
}

#endif