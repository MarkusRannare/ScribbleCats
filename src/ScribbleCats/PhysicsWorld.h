#ifndef SCRIBBLE_PHYSICS_WORLD_H
#define SCRIBBLE_PHYSICS_WORLD_H

#include "foundation/collection_types.h"
#include "PhysicsTypes.h"
#include <windows.h>

namespace Scribble
{
	const float SMALL_NUMBER = 0.00001f;

	class Shape;
	struct Body;

	class PhysicsWorld
	{
		public:
			PhysicsWorld();
			~PhysicsWorld();

			void AddBody( const Body& AARB );
			void RemoveBody( const Body& AARB );

			void Tick( float Delta );
			void DebugRender();

			bool IsSpaceFree( const AARB& AARB );

			bool SweepAARB( const Vector2& FromLocation,
				const Vector2& ToLocation,
				const Vector2& Extent,
				CollisionData* out_CollisionData );
		private:
			bool AARBToAARBSweep( const Vector2& FromLocation,
				const Vector2& ToLocation,
				const Vector2& Extent,
				const AARB& StaticAARB,
				CollisionData* out_CollisionData );
		private:
			foundation::Array<Body> mBodies;
	};

	void DrawAARB( const AARB& A, DWORD Color );
}

#endif