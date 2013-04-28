#ifndef SCRIBBLE_AARB_H
#define SCRIBBLE_AARB_H

#include "PhysicsTypes.h"

namespace Scribble
{
	namespace aarb
	{
		inline void CreateBox( const Vector2& FromLocation, float Width, AARB& out_AARB );

		inline bool Intersects( const AARB& A, const AARB& B );

		inline float Max( const AARB& A, int Idx );
		inline float Min( const AARB& A, int Idx );

		inline float MinX( const AARB& A );
		inline float MaxX( const AARB& A );

		inline float MinY( const AARB& A );
		inline float MaxY( const AARB& A );
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
			if( abs( A._Center.X - B._Center.X ) >= ( A._Extent.X + B._Extent.X ) )
			{
				return false;
			}
			if( abs( A._Center.Y - B._Center.Y ) >= ( A._Extent.Y + B._Extent.Y ) )
			{
				return false;
			}

			return true;
		}

		float Max( const AARB& A, int Idx )
		{
			assert( Idx >= 0 && Idx < 2 );
			return A._Center[ Idx ] + A._Extent[ Idx ];
		}

		float Min( const AARB& A, int Idx )
		{
			assert( Idx >= 0 && Idx < 2 );
			return A._Center[ Idx ] - A._Extent[ Idx ];
		}

		float MinX( const AARB& A )
		{
			return A._Center.X - A._Extent.X;
		}

		float MaxX( const AARB& A )
		{
			return A._Center.X + A._Extent.X;
		}

		float MinY( const AARB& A )
		{
			return A._Center.Y - A._Extent.Y;
		}

		float MaxY( const AARB& A )
		{
			return A._Center.Y + A._Extent.Y;
		}
	}
}

#endif