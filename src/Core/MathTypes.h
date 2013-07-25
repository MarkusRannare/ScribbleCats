#ifndef SCRIBBLE_MATHTYPES_H
#define SCRIBBLE_MATHTYPES_H

#include "Vector2.h"

namespace Scribble
{
	struct Bounds
	{
		inline Bounds( const Vector2& _Min, const Vector2& _Max );
		inline Bounds();

		Vector2 Min;
		Vector2 Max;
	};

	inline Bounds::Bounds( const Vector2& _Min, const Vector2& _Max ) :
		Min( _Min ),
		Max( _Max )
	{
	}

	inline Bounds::Bounds() :
		Min( Vector2( 0, 0 ) ),
		Max( Vector2( 0, 0 ) )
	{
	}
		
}

#endif