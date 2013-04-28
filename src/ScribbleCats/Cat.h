#ifndef SCRIBBLE_CAT_H
#define SCRIBBLE_CAT_H

#include "Actor.h"

#include <hge/hgesprite.h>

namespace Scribble
{
	class Cat : public Actor
	{
		public:
			Cat( Vector2 Location );
			virtual ~Cat();

			virtual void Tick( float Dt );
			virtual void Render();

			virtual void Landed( const CollisionData& CollisionInfo );
		protected:
			virtual void SimulatePhysics( float Dt );
			virtual void PhysWalking( float Dt );
			virtual void PhysFalling( float Dt );
		private:
			Cat(); // -no default constructor
			friend class World;
		private:
			Vector2 mAcceleration;

			hgeSprite* mSprite;
			
			bool mUpHold;
			bool mRightHold;
			bool mDownHold;
			bool mLeftHold;
			bool mJumpPressed;
	};
}

#endif