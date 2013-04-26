#ifndef SCRIBBLE_BODY_H
#define SCRIBBLE_BODY_H

#include "Vector2.h"

namespace Scribble
{
	class Shape;

	enum EBodyType
	{
		BT_Static,
		BT_Dynamic
	};

	struct Body
	{
		Shape* Shape;

		Vector2 Location;
		Vector2 Velocity;

		float Mass;
		float Rotation;

		EBodyType BodyType;
	};
}

#endif