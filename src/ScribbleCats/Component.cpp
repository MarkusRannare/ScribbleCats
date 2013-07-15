#include "Component.h"

#include "World.h"

// Remove when including something else that includes stdlib.h
#include <stdlib.h>

namespace Scribble
{
	Component::Component() :
		Object()
	{
	}

	Component::~Component()
	{
	}

	void Component::AttachedTo( Actor* NewOwner )
	{
		assert( mOwner == NULL && "Can't attach a component to a actor if it already is attached to another actor" );
		mOwner = NewOwner;

		g_World->ComponentAttached( this );
	}

	void Component::DeattachedFrom( Actor* OldOwner )
	{
		assert( mOwner != NULL && "Can't deattach a component from another actor than it's owner" );
		mOwner = NULL;

		g_World->ComponentDeattached( this );
	}
}