#ifndef SCRIBBLE_VECTOR2_H
#define SCRIBBLE_VECTOR2_H

#ifdef WIN32
#pragma once
#endif

#include <cmath>
#include <cassert>

namespace Scribble
{
	struct Vector2
	{
		inline Vector2()
		{
		}

		inline Vector2( float X_, float Y_ ) :
		X( X_ ),
			Y( Y_ )
		{
		}

		inline Vector2( const Vector2& Other ) :
		X( Other.X ),
			Y( Other.Y )
		{
		}

		inline const Vector2& operator=( const Vector2& Other )
		{
			X = Other.X;
			Y = Other.Y;

			return *this;
		}

		inline bool operator==( const Vector2& Other ) const
		{
			return X == Other.X && Y == Other.Y;
		}

		inline bool operator!=( const Vector2& Other ) const
		{
			return X != Other.X || Y != Other.Y;
		}

		Vector2 operator+( const Vector2& Rhs ) const
		{
			return Vector2( X + Rhs.X, Y + Rhs.Y );
		}

		Vector2 operator-( const Vector2& Rhs ) const
		{
			return Vector2( X - Rhs.X, Y - Rhs.Y );
		}

		Vector2 operator-() const
		{
			return Vector2( -X, -Y );
		}

		Vector2 operator*( float Scalar ) const
		{
			return Vector2( X * Scalar, Y * Scalar );
		}

		Vector2 operator/( float Divider ) const
		{
			return Vector2( X / Divider, Y / Divider );
		}

		Vector2& operator +=( const Vector2& Rhs )
		{
			X += Rhs.X;
			Y += Rhs.Y;

			return *this;
		}

		Vector2& operator -=( const Vector2& Rhs )
		{
			X -= Rhs.X;
			Y -= Rhs.Y;

			return *this;
		}

		Vector2& operator *=( float Scalar )
		{
			X *= Scalar;
			Y *= Scalar;

			return *this;
		}

		Vector2& operator /=( float Scalar )
		{
			X /= Scalar;
			Y /= Scalar;

			return *this;
		}

		float Dot( const Vector2& Rhs ) const
		{
			return X * Rhs.X + Y * Rhs.Y;
		}

		float Length() const
		{
			return sqrtf( X * X + Y * Y );
		}

		static Vector2 Normalize( const Vector2& V )
		{
			return V / sqrtf( V.X * V.X + V.Y * V.Y );
		}

		void Normalize()
		{
			*this /= sqrtf( X * X + Y * Y );
		}

		static float LengthSq( const Vector2& A, const Vector2& B )
		{
			const Vector2& Direction = A - B;

			return Direction.X * Direction.X + Direction.Y * Direction.Y;
		}

		float LengthSq() const
		{
			return X * X + Y * Y;
		}

		inline static Vector2 Lerp( const Vector2& A, const Vector2& B, float Alpha );

		void Rotate( float Theta )
		{
			X = cosf( Theta ) * X - sinf( Theta ) * Y;
			Y =	cosf( Theta ) * Y + sinf( Theta ) * X;
		}

		void Clamp( float DesiredLength )
		{
			float Length = sqrtf( X * X + Y * Y );

			if( Length != 0 )
			{
				X /= Length;
				Y /= Length;

				X *= DesiredLength;
				Y *= DesiredLength;
			}
		}

		inline float operator []( int Idx ) const
		{
			assert( Idx >= X_AXIS && Idx <= Y_AXIS );
			
			return Values[ Idx ];	
		}

		inline float& operator[]( int Idx )
		{
			assert( Idx >= X_AXIS && Idx <= Y_AXIS );

			return Values[ Idx ];	
		}

		static const Vector2 ZERO;
		static const Vector2 ONE;
		static const Vector2 UP;
		static const Vector2 DOWN;
		static const Vector2 LEFT;
		static const Vector2 RIGHT;

		static const int X_AXIS;
		static const int Y_AXIS;

		/// Members
		union
		{
			struct
			{
				float X, Y;
			};
			float Values[2];
		};
	};

	inline Vector2 operator*( float Scalar, const Vector2& Rhs )
	{
		return Vector2( Rhs.X * Scalar, Rhs.Y * Scalar );
	}

	Vector2 Vector2::Lerp( const Vector2& A, const Vector2& B, float Alpha )
	{
		return ( 1.0f - Alpha ) * A + Alpha * B;
	}
}

#endif