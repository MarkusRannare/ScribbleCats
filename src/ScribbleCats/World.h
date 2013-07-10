#ifndef SCRIBBLE_WORLD_H
#define SCRIBBLE_WORLD_H

#include "Actor.h"
#include <assert.h>
#include "foundation/array.h"
#include <box2d/Box2D.h>

namespace Scribble
{
	class Component;

	const float TO_PHYSICS = 0.015625f;
	const float TO_WORLD = 64.0f;

	inline b2Vec2 VectorToB2( Vector2 V );
	inline Vector2 B2ToVector( b2Vec2 V );

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

			b2World& GetPhysicsWorld();

			bool SetActorLocation( Actor* Actor, const Vector2& ToLocation );
			bool MoveActor( Actor* Actor, const Vector2& ToLocation );

			void ComponentAttached( Component* TheComponent );
			void ComponentDeattached( Component* TheComponent );
		protected:
			bool ResolveCollision( Actor* Actor, 
				const Vector2& TargetLocation, 
				const CollisionData& CollisionInfo, 
				float TimesliceRemaining, 
				Vector2& out_NewWantedLocation );
		private:
			//PhysicsWorld* mPhysicsWorld;
			b2World mPhysicsWorld;
			b2Draw* mDebugPhysics;

			foundation::Array<Actor*> mActors;
			foundation::Array<Actor*> mNewActors;
			foundation::Array<Actor*> mDeadActors;

			foundation::Array<Component*> mComponents;
	};

	b2Vec2 VectorToB2( Vector2 V )
	{
		return b2Vec2( V.X, V.Y );
	}

	Vector2 B2ToVector( b2Vec2 V )
	{
		return Vector2( V.x, V.y );
	}
}

#endif