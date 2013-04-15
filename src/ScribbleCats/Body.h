#ifndef SCRIBBLE_BODY_H
#define SCRIBBLE_BODY_H

#include <hge/hgevector.h>

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

		hgeVector Location;
		hgeVector Velocity;

		float Mass;
		float Rotation;

		EBodyType BodyType;
	};
}

#endif