#ifndef SCRIBBLE_COMPONENT_H
#define SCRIBBLE_COMPONENT_H

#include "Object.h"

namespace Scribble
{
	class Actor;

	class Component : public Object
	{
		public:
			Component();
			virtual ~Component();

			virtual void AttachedTo( Actor* NewOwner );
			virtual void DeattachedFrom( Actor* OldOwner );
	};
}

#endif