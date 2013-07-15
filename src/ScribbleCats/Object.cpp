#include "Object.h"

#include "stdlib.h"

namespace Scribble
{
	Object::Object() :
		mOwner( NULL )
	{
	}

	Object::~Object()
	{
		mOwner = NULL;
	}
}