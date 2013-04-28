#ifndef SCRIBBLE_WORLD_H
#define SCRIBBLE_WORLD_H

#include "Actor.h"
#include <assert.h>
#include "foundation/array.h"

namespace Scribble
{
	class PhysicsWorld;

	class World
	{
		public:
			World();
			~World();

			// T is expecting a Entity
			template<class T>
			T* Spawn( Vector2 Location )
			{
				Actor* NewActor = MAKE_NEW( memory_globals::default_allocator(), T, Location );
				
				assert( dynamic_cast<Actor*>( NewActor ) != NULL && "Non actor called on Spawn" );

				foundation::array::push_back( mNewActors, NewActor );

				return (T*)NewActor;
			}

			void Destroy( Actor* Actor );

			void ClearActorList();

			void HandleNewAndOldActors();

			virtual void Tick( float Dt );
			virtual void Render();

			PhysicsWorld* GetPhysicsWorld() const;

			bool SetActorLocation( Actor* Actor, const Vector2& ToLocation );
			bool MoveActor( Actor* Actor, const Vector2& ToLocation );
		protected:
			bool ResolveCollision( Actor* Actor, 
				const Vector2& TargetLocation, 
				const CollisionData& CollisionInfo, 
				float TimesliceRemaining, 
				Vector2& out_NewWantedLocation );
		private:
			PhysicsWorld* mPhysicsWorld;

			foundation::Array<Actor*> mActors;
			foundation::Array<Actor*> mNewActors;
			foundation::Array<Actor*> mDeadActors;
	};
}

#endif