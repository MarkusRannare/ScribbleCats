#ifndef SCRIBBLE_ACTOR_H
#define SCRIBBLE_ACTOR_H

#include "Vector2.h"

namespace Scribble
{
	class World;

	class Actor
	{
		public:
			virtual ~Actor();

			virtual void Tick( float Dt );
			virtual void Render();
		protected:
			Actor( Vector2 Location );
		private:
			Actor(); // - No default constructor
		private:
			friend class World;
		protected:
			Vector2 mLocation;
	};
}

#endif