#ifndef SCRIBBLE_CAT_H
#define SCRIBBLE_CAT_H

#include "Actor.h"

#include <hge/hgesprite.h>

namespace Scribble
{
	class Cat : public Actor
	{
		public:
			enum EPhysics
			{
				PHYS_Falling,
				PHYS_Walking
			};
		public:
			Cat( hgeVector Location );
			virtual ~Cat();

			void SimulatePhysics( float Dt );

			virtual void Tick( float Dt );
			virtual void Render();
		protected:
			virtual void PhysFalling( float Dt );
			virtual void PhysWalking( float Dt );
		private:
			Cat(); // -no default constructor
		private:
			hgeVector mAcceleration;
			hgeVector mVelocity;

			hgeSprite* mSprite;

			EPhysics mCurrentPhysics;
			
			bool mUpHold;
			bool mRightHold;
			bool mDownHold;
			bool mLeftHold;
			bool mJumpPressed;
	};
}

#endif