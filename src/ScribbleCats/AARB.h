#ifndef SCRIBBLE_AARB_H
#define SCRIBBLE_AARB_H

#include "PhysicsTypes.h"

namespace Scribble
{
	namespace Physics
	{
		void CreateBox( const hgeVector& FromLocation, float Width, AARB& out_AARB );

		bool Intersects( const AARB& A, const AARB& B );
	}

	namespace Physics
	{
		void CreateBox( const hgeVector& FromLocation, float Width, AARB& out_AARB )
		{
			out_AARB._Center = FromLocation;
			out_AARB._Extent.x = Width / 2.0f;
			out_AARB._Extent.y = Width / 2.0f;
		}

		bool Intersects( const AARB& A, const AARB& B )
		{
			if( abs( A._Center.x - B._Center.x ) > ( A._Extent.x + B._Extent.x ) )
			{
				return false;
			}
			if( abs( A._Center.y - B._Center.y ) > ( A._Extent.y + B._Extent.y ) )
			{
				return false;
			}

			return true;
		}
	}
}

#endif