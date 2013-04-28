#include "PhysicsWorld.h"
#include "Body.h"

#include "foundation/memory.h"
#include "array_functions.h"
#include <hge/hge.h>
#include "AARB.h"

extern HGE* g_Hge;

using namespace foundation;

namespace Scribble
{
	PhysicsWorld::PhysicsWorld() :
		mBodies( memory_globals::default_allocator() )
	{
	}

	PhysicsWorld::~PhysicsWorld()
	{
		for( size_t Idx = 0; Idx < array::size( mBodies ); ++Idx )
		{
		}
	}

	void PhysicsWorld::AddBody( const Body& Body )
	{
		array::push_back( mBodies, Body );
	}

	void PhysicsWorld::RemoveBody( const Body& InBody )
	{
		array::remove_swap( mBodies, InBody );
	}

	void PhysicsWorld::Tick( float Delta )
	{
		for( size_t Idx = 0; Idx < array::size( mBodies ); ++Idx )
		{
			// Simulate
		}
	}

	AARB CollidingAARB;
	AARB Cont;

	void PhysicsWorld::DebugRender()
	{
		for( size_t Idx = 0; Idx < array::size( mBodies ); ++Idx )
		{
			DrawAARB( mBodies[Idx]._Collision, 0xffcccccc );
		}
	}

	bool PhysicsWorld::IsSpaceFree( const AARB& AARB )
	{
		for( size_t Idx = 0; Idx < array::size( mBodies ); ++Idx )
		{
			if( aarb::Intersects( AARB, mBodies[Idx]._Collision ) )
			{
				return false;
			}
		}
		
		return true;
	}	

	bool PhysicsWorld::SweepAARB( const Vector2& FromLocation, const Vector2& ToLocation, const Vector2& Extent, CollisionData* out_CollisionData )
	{
		out_CollisionData->FirstContact = 1.01f;
		bool DidCollide = false;

		for( size_t Idx = 0; Idx < array::size( mBodies ); ++Idx )
		{
			CollisionData TempCollisionData;
			if( AARBToAARBSweep( FromLocation, ToLocation, Extent, mBodies[Idx]._Collision, &TempCollisionData ) )
			{
				if( TempCollisionData.FirstContact < out_CollisionData->FirstContact )
				{
					*out_CollisionData = TempCollisionData;
					DidCollide = true;
				}
			}
		}

		return DidCollide;
	}



	bool PhysicsWorld::AARBToAARBSweep( const Vector2& FromLocation, 
		const Vector2& ToLocation, 
		const Vector2& Extent,
		const AARB& StaticAARB,
		CollisionData* out_CollisionData )
	{
		AARB SourceState( FromLocation, Extent );

		const Vector2 Velocity = ToLocation - FromLocation + Vector2( SMALL_NUMBER, -SMALL_NUMBER );

		// First overlap along each axis
		Vector2 U0( 0, 0 );
		// Last overlap along each axis
		Vector2 U1( 1, 1 );

		// If the overlap at initial location, no sweep needed
		if( aarb::Intersects( SourceState, StaticAARB ) )
		{
			out_CollisionData->FirstContact = out_CollisionData->LastContact = 0;
			out_CollisionData->_Normal = Vector2::UP;

			return true;
		}

		for( int Axis = Vector2::X_AXIS; Axis <= Vector2::Y_AXIS; ++Axis )
		{
			// Calculate first point of contact
			if( aarb::Max( SourceState, Axis ) <= aarb::Min( StaticAARB, Axis ) && Velocity[Axis] > 0 )
			{
				U0[Axis] = ( aarb::Min( StaticAARB, Axis ) - aarb::Max( SourceState, Axis ) ) / Velocity[Axis];
			}
			else if( ( aarb::Max( StaticAARB, Axis ) <= aarb::Min( SourceState, Axis ) ) && Velocity[Axis] < 0 )
			{
				U0[Axis] = ( aarb::Max( StaticAARB, Axis ) - aarb::Min( SourceState, Axis ) ) / Velocity[Axis];
			}

			if( aarb::Max( StaticAARB, Axis ) <= aarb::Min( SourceState, Axis ) && Velocity[Axis] > 0 )
			{
				U1[Axis] = ( aarb::Max( StaticAARB, Axis ) - aarb::Min( SourceState, Axis ) ) / Velocity[Axis];
			}
			else if( aarb::Max( SourceState, Axis ) <= aarb::Min( StaticAARB, Axis ) && Velocity[Axis] < 0 )
			{
				U1[Axis] = ( aarb::Min( StaticAARB, Axis ) - aarb::Max( SourceState, Axis ) ) / Velocity[Axis];
			}
		}

		// Possible first time of contact
		out_CollisionData->FirstContact = max( U0.X, U0.Y );
		// Possible last time of contact
		out_CollisionData->LastContact = min( U1.X, U1.Y );

		if( out_CollisionData->FirstContact < 1.0f && out_CollisionData->FirstContact <= out_CollisionData->LastContact )
		{
			// Calculate normal here too
			AARB Contact = AARB( FromLocation + ( FromLocation - ToLocation ) * out_CollisionData->FirstContact, Extent );
			Cont = Contact;
			// If the contact is to the left of the Static AARB
			if( aarb::MaxX( Contact ) <= aarb::MinX( StaticAARB ) )
			{
				// Does the collision lie in the top vertex voronoi region
				if( aarb::MaxY( Contact ) < aarb::MinY( StaticAARB ) )
				{
					out_CollisionData->_Normal = Vector2::Normalize( Vector2( -1, -1 ) );
				}
				// Does the collision lie in the bottom vertex voronoi region
				else if( aarb::MinY( Contact ) > aarb::MaxY( StaticAARB ) )
				{
					out_CollisionData->_Normal = Vector2::Normalize( Vector2( -1, 1 ) );
				}
				// Just from the left =)
				else
				{
					out_CollisionData->_Normal = Vector2::LEFT;
				}
			}
			// If the contact is to the right of the Static AARB
			else if( aarb::MinX( Contact ) >= aarb::MaxX( StaticAARB ) )
			{
				// Does the collision lie in the top vertex voronoi region
				if( aarb::MaxY( Contact ) < aarb::MinY( StaticAARB ) )
				{
					out_CollisionData->_Normal = Vector2::Normalize( Vector2( 1, -1 ) );
				}
				// Does the collision lie in the bottom vertex voronoi region
				else if( aarb::MinY( Contact ) > aarb::MaxY( StaticAARB ) )
				{
					out_CollisionData->_Normal = Vector2::Normalize( Vector2( 1, 1 ) );
				}
				// Just from the right =)
				else
				{
					out_CollisionData->_Normal = Vector2::RIGHT;
				}
			}
			// The contact lies above or below the AARB
			else
			{
				// Is it above the box 
				if( aarb::MaxY( Contact ) <= aarb::MinY( StaticAARB ) )
				{
					out_CollisionData->_Normal = Vector2::UP;
				}
				else
				{
					out_CollisionData->_Normal = Vector2::DOWN;
				}
			}

			return true;
		}

		out_CollisionData->FirstContact = 0;
		out_CollisionData->LastContact = 0;

		return false;
	}

	void DrawAARB( const AARB& A, DWORD Color )
	{
		// Top line
		g_Hge->Gfx_RenderLine( A._Center.X - A._Extent.X, A._Center.Y - A._Extent.Y, 
			A._Center.X + A._Extent.X, A._Center.Y - A._Extent.Y,
			Color );
		// Right line
		g_Hge->Gfx_RenderLine( A._Center.X + A._Extent.X, A._Center.Y - A._Extent.Y, 
			A._Center.X + A._Extent.X, A._Center.Y + A._Extent.Y,
			Color );

		// Bottom line
		g_Hge->Gfx_RenderLine( A._Center.X + A._Extent.X, A._Center.Y + A._Extent.Y, 
			A._Center.X - A._Extent.X, A._Center.Y + A._Extent.Y,
			Color );

		// Left line
		g_Hge->Gfx_RenderLine( A._Center.X - A._Extent.X, A._Center.Y + A._Extent.Y, 
			A._Center.X - A._Extent.X, A._Center.Y - A._Extent.Y,
			Color );
	}
}