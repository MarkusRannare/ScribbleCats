#include "Component.h"

#include "World.h"

// Remove when including something else that includes stdlib.h
#include <stdlib.h>

namespace Scribble
{
	Component::Component() :
		mOwner( NULL )
	{
	}

	Component::~Component()
	{
		mOwner = NULL;
	}

	void Component::AttachedTo( Actor* NewOwner )
	{
		mOwner = NewOwner;

		g_World->ComponentAttached( this );
	}

	void Component::DeattachedFrom( Actor* OldOwner )
	{
		mOwner = NULL;

		g_World->ComponentDeattached( this );
	}
}