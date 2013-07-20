#ifndef SCRIBBLE_AABB_H
#define SCRIBBLE_AABB_H

#include "PhysicsTypes.h"

namespace Scribble
{
	namespace aabb
	{
		inline void CreateBox( const Vector2& FromLocation, float Width, AABB& out_AARB );
		inline void CreateRectangle( const Vector2& FromLocation, float Width, float Height, AABB& out_AARB );

		inline bool Intersects( const AABB& A, const AABB& B );

		inline float Max( const AABB& A, int Idx );
		inline float Min( const AABB& A, int Idx );

		inline float MinX( const AABB& A );
		inline float MaxX( const AABB& A );

		inline float MinY( const AABB& A );
		inline float MaxY( const AABB& A );
	}

	namespace aabb
	{
		void CreateBox( const Vector2& FromLocation, float Width, AABB& out_AARB )
		{
			out_AARB._Center = FromLocation;
			out_AARB._Extent.X = Width / 2.0f;
			out_AARB._Extent.Y = Width / 2.0f;
		}

		void CreateRectangle( const Vector2& FromLocation, float Width, float Height, AABB& out_AARB )
		{
			out_AARB._Center = FromLocation;
			out_AARB._Extent.X = Width / 2.0f;
			out_AARB._Extent.Y = Height / 2.0f;
		}

		bool Intersects( const AABB& A, const AABB& B )
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

		float Max( const AABB& A, int Idx )
		{
			assert( Idx >= 0 && Idx < 2 );
			return A._Center[ Idx ] + A._Extent[ Idx ];
		}

		float Min( const AABB& A, int Idx )
		{
			assert( Idx >= 0 && Idx < 2 );
			return A._Center[ Idx ] - A._Extent[ Idx ];
		}

		float MinX( const AABB& A )
		{
			return A._Center.X - A._Extent.X;
		}

		float MaxX( const AABB& A )
		{
			return A._Center.X + A._Extent.X;
		}

		float MinY( const AABB& A )
		{
			return A._Center.Y - A._Extent.Y;
		}

		float MaxY( const AABB& A )
		{
			return A._Center.Y + A._Extent.Y;
		}
	}
}

#endif