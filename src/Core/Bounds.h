#ifndef SCRIBBLE_BOUNDS_H
#define SCRIBBLE_BOUNDS_H

#include "MathTypes.h"

namespace Scribble
{
	namespace bounds
	{
		inline void from_rect( const Vector2& location, float width, float height, Bounds& out_b );

		inline bool contains( const Bounds& b, float x, float y );
	}

	namespace bounds
	{
		inline void from_rect( const Vector2& location, float width, float height, Bounds& out_b )
		{
			out_b.Min.X = location.X;
			out_b.Min.Y = location.Y;
			out_b.Max.X = location.X + width;
			out_b.Max.Y = location.Y + height;
		}

		inline bool contains( const Bounds& b, float x, float y )
		{
			return x > b.Min.X && y > b.Min.Y &&
				x < b.Max.X && y < b.Max.Y;
		}
	}
}

#endif