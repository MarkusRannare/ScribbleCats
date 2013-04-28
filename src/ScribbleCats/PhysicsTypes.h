#ifndef SCRIBBLE_PHYSICSTYPES_H
#define SCRIBBLE_PHYSICSTYPES_H

#include "Vector2.h"

namespace Scribble
{
	struct AARB
	{
		inline AARB();
		inline AARB( const Vector2& Center, const Vector2& Extent );
		inline AARB& operator=( const AARB& Other );

		inline bool operator==( const AARB& Other ) const;
		inline bool operator!=( const AARB& Other ) const;		

		Vector2 _Center;
		Vector2 _Extent;
	};

	struct Body
	{
		AARB _Collision;
		void* _UserPointer;

		inline bool operator==( const Body& Other ) const;
		inline bool operator!=( const Body& Other ) const;
	};

	struct CollisionData
	{
		Vector2 _Normal;
		float FirstContact;
		float LastContact;
	};
}

namespace Scribble
{
	AARB::AARB()
	{
	}

	AARB::AARB( const Vector2& Center, const Vector2& Extent ) :
		_Center( Center ),
		_Extent( Extent )
	{
	}

	AARB& AARB::operator=( const AARB& Other )
	{
		_Center = Other._Center;
		_Extent = Other._Extent;

		return *this;
	}

	bool AARB::operator==( const AARB& Other ) const
	{
		return _Center == Other._Center && _Extent == Other._Extent;
	}

	bool AARB::operator!=( const AARB& Other ) const
	{
		return _Center != Other._Center || _Extent != Other._Extent;
	}

	bool Body::operator==( const Body& Other ) const
	{
		return _Collision == Other._Collision && _UserPointer == Other._UserPointer;
	}

	bool Body::operator!=( const Body& Other ) const
	{
		return _Collision != Other._Collision || _UserPointer != Other._UserPointer;
	}
}

#endif