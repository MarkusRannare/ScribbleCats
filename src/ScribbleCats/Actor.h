#ifndef SCRIBBLE_ACTOR_H
#define SCRIBBLE_ACTOR_H

#include <hge/hgevector.h>

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
			Actor( hgeVector Location );
		private:
			Actor(); // - No default constructor
		private:
			friend class World;
		protected:
			hgeVector mLocation;
	};
}

#endif