#ifndef SCRIBBLE_BOX2DDEBUGDRAW_H
#define SCRIBBLE_BOX2DDEBUGDRAW_H

#include <box2d/Box2D.h>

namespace Scribble
{
	class Box2DDebugDraw : public b2Draw
	{
		public:
			void DrawPolygon( const b2Vec2* Vertices, int32 VertexCount, const b2Color& Color );

			void DrawSolidPolygon( const b2Vec2* Vertices, int32 VertexCount, const b2Color& Color );

			void DrawCircle( const b2Vec2& Center, float32 Radius, const b2Color& Color );

			void DrawSolidCircle( const b2Vec2& Center, float32 Radius, const b2Vec2& Axis, const b2Color& Color );

			void DrawSegment( const b2Vec2& P1, const b2Vec2& P2, const b2Color& Color );

			void DrawTransform( const b2Transform& Xf );

			void DrawPoint( const b2Vec2& P, float32 Size, const b2Color& Color );

			void DrawString( int X, int Y, const char* String, ... ); 

			void DrawAABB( b2AABB* AABB, const b2Color& Color );
	};
}

#endif