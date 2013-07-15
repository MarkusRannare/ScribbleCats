#ifndef SCRIBBLE_CAT_H
#define SCRIBBLE_CAT_H

#include "Actor.h"

#include <hge/hgeanim.h>

class b2Body;

namespace Scribble
{
	class Cat : public Actor
	{
		public:
			Cat( Vector2 Location );
			virtual ~Cat();

			virtual void Tick( float Dt );
			virtual void Render();

			virtual void Landed( const TraceHit& CollisionInfo );
		protected:
			virtual void SimulatePhysics( float Dt );
			virtual void PhysWalking( float Dt );
			virtual void PhysFalling( float Dt );
		private:
			Cat(); // -no default constructor
			friend class World;
		private:
			Vector2 mAcceleration;

			hgeAnimation* mWalkLeft;
			hgeAnimation* mWalkRight;

			hgeAnimation* mCurrentAnim;
			
			bool mUpHold;
			bool mRightHold;
			bool mDownHold;
			bool mLeftHold;
			bool mJumpPressed;
	};
}

#endif