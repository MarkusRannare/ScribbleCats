#ifndef SCRIBBLE_COMPONENT_H
#define SCRIBBLE_COMPONENT_H

namespace Scribble
{
	class Actor;

	class Component
	{
		public:
			Component();
			virtual ~Component();

			virtual void AttachedTo( Actor* NewOwner );
			virtual void DeattachedFrom( Actor* OldOwner );
		private:
			Actor* mOwner;
	};
}

#endif