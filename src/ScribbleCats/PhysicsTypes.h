#ifndef SCRIBBLE_PHYSICSTYPES_H
#define SCRIBBLE_PHYSICSTYPES_H

#include "Vector2.h"

namespace Scribble
{
	struct AABB
	{
		inline AABB();
		inline AABB( const Vector2& Center, const Vector2& Extent );
		inline AABB& operator=( const AABB& Other );

		inline bool operator==( const AABB& Other ) const;
		inline bool operator!=( const AABB& Other ) const;	

		inline AABB operator*( float S ) const;
		inline AABB& operator*=( float S );
		
		Vector2 _Center;
		Vector2 _Extent;
	};
}

namespace Scribble
{
	AABB::AABB()
	{
	}

	AABB::AABB( const Vector2& Center, const Vector2& Extent ) :
		_Center( Center ),
		_Extent( Extent )
	{
	}

	AABB& AABB::operator=( const AABB& Other )
	{
		_Center = Other._Center;
		_Extent = Other._Extent;

		return *this;
	}

	bool AABB::operator==( const AABB& Other ) const
	{
		return _Center == Other._Center && _Extent == Other._Extent;
	}

	bool AABB::operator!=( const AABB& Other ) const
	{
		return _Center != Other._Center || _Extent != Other._Extent;
	}

	inline AABB AABB::operator*( float S ) const
	{
		return AABB( _Center * S, _Extent * S );
	}

	inline AABB& AABB::operator*=( float S )
	{
		_Center *= S;
		_Extent *= S;

		return *this;
	}

}

#endif