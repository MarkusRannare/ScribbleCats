#ifndef SCRIBBLE_AARB_H
#define SCRIBBLE_AARB_H

#include "PhysicsTypes.h"

namespace Scribble
{
	namespace aarb
	{
		void CreateBox( const Vector2& FromLocation, float Width, AARB& out_AARB );

		bool Intersects( const AARB& A, const AARB& B );

		inline float Max( const AARB& A, int Idx );
		inline float Min( const AARB& A, int Idx );

		float MinX( const AARB& A );
		float MaxX( const AARB& A );

		float MinY( const AARB& A );
		float MaxY( const AARB& A );
	}

	namespace aarb
	{
		void CreateBox( const Vector2& FromLocation, float Width, AARB& out_AARB )
		{
			out_AARB._Center = FromLocation;
			out_AARB._Extent.X = Width / 2.0f;
			out_AARB._Extent.Y = Width / 2.0f;
		}

		bool Intersects( const AARB& A, const AARB& B )
		{
			if( abs( A._Center.X - B._Center.X ) > ( A._Extent.X + B._Extent.X ) )
			{
				return false;
			}
			if( abs( A._Center.Y - B._Center.Y ) > ( A._Extent.Y + B._Extent.Y ) )
			{
				return false;
			}

			return true;
		}

		float Max( const AARB& A, int Idx )
		{
		}

		float Min( const AARB& A, int Idx )
		{
		}

		float MinX( const AARB& A );
		float MaxX( const AARB& A );

		float MinY( const AARB& A );
		float MaxY( const AARB& A );
	}
}

#endif