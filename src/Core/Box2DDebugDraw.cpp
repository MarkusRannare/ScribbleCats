#include "Box2DDebugDraw.h"

#include "World.h"
#include <hge/hge.h>

#define B2TOCOLOR( Color ) ARGB( 255, Color.r * 255, Color.g * 255, Color.b * 255 )

namespace Scribble
{
	void Box2DDebugDraw::DrawPolygon( const b2Vec2* Vertices, int32 VertexCount, const b2Color& Color )
	{
		extern HGE* g_Hge;
		for( int32 Idx = 0; Idx < VertexCount; ++Idx )
		{
			int32 ToIdx = ( Idx + 1 ) % VertexCount;
			g_Hge->Gfx_RenderLine( Vertices[Idx].x * TO_WORLD, Vertices[Idx].y* TO_WORLD, Vertices[ToIdx].x* TO_WORLD, Vertices[ToIdx].y * TO_WORLD, B2TOCOLOR( Color ) );
		}
	}

	void Box2DDebugDraw::DrawSolidPolygon( const b2Vec2* Vertices, int32 VertexCount, const b2Color& Color )
	{
		extern HGE* g_Hge;

		int MaxPrims;
		int NumPrims = 0;
		int NumVertices = 0;
		
		Vector2 Center( 0, 0 );
		for( int32 Idx = 0; Idx < VertexCount; ++Idx )
		{
			Center += Vector2( Vertices[Idx].x, Vertices[Idx].y );
		}
		Center /= (float)VertexCount;

		DWORD AlphaColor = ARGB( 127, Color.r * 255, Color.g * 255, Color.b * 255 );

		hgeVertex* RenderVertices = g_Hge->Gfx_StartBatch( HGEPRIM_TRIPLES, NULL, BLEND_ALPHABLEND, &MaxPrims );
		for( int32 Idx = 0; Idx < VertexCount; ++Idx )
		{
			int32 ToIdx = ( Idx + 1 ) % VertexCount;

			hgeVertex Vertice0 = { Vertices[Idx].x * TO_WORLD, Vertices[Idx].y * TO_WORLD, 0, AlphaColor, 0, 0 };
			hgeVertex Vertice1 = { Vertices[ToIdx].x * TO_WORLD, Vertices[ToIdx].y * TO_WORLD, 0, AlphaColor, 0, 0 };
			hgeVertex Vertice2 = { Center.X * TO_WORLD, Center.Y * TO_WORLD, 0, AlphaColor, 0, 0 };
			RenderVertices[NumVertices++] = Vertice0;
			RenderVertices[NumVertices++] = Vertice1;
			RenderVertices[NumVertices++] = Vertice2;
			++NumPrims;
		}
		g_Hge->Gfx_FinishBatch( NumPrims );

		DrawPolygon( Vertices, VertexCount, Color );
	}

	void Box2DDebugDraw::DrawCircle( const b2Vec2& Center, float32 Radius, const b2Color& Color )
	{
		float SEGMENTS = 16.0f;
		float INCREMENT = 2.0f * b2_pi / SEGMENTS;

		float Theta = 0.0f;
		b2Vec2 OldLocation = Center + Radius * b2Vec2( cosf(Theta), sinf(Theta) );
		OldLocation.x *= TO_WORLD;
		OldLocation.y *= TO_WORLD;
		for( int Idx = 0; Idx < SEGMENTS; ++Idx )
		{
			b2Vec2 NewLocation = Center + Radius * b2Vec2( cosf(Theta + INCREMENT), sinf(Theta + INCREMENT) );
			NewLocation.x *= TO_WORLD;
			NewLocation.y *= TO_WORLD;

			extern HGE* g_Hge;
			g_Hge->Gfx_RenderLine( OldLocation.x, OldLocation.y, NewLocation.x, NewLocation.y, B2TOCOLOR( Color ) );

			Theta += INCREMENT;

			OldLocation = NewLocation;
		}
	}

	void Box2DDebugDraw::DrawSolidCircle( const b2Vec2& Center, float32 Radius, const b2Vec2& Axis, const b2Color& Color )
	{
		extern HGE* g_Hge;
		float SEGMENTS = 16.0f;
		float INCREMENT = 2.0f * b2_pi / SEGMENTS;

		float Theta = 0.0f;
		b2Vec2 OldLocation = Center + Radius * b2Vec2( cosf(Theta), sinf(Theta) );
		OldLocation.x *= TO_WORLD;
		OldLocation.y *= TO_WORLD;

		int MaxPrims;
		int NumPrims = 0;
		int NumVertices = 0;
		hgeVertex* Vertices = g_Hge->Gfx_StartBatch( HGEPRIM_TRIPLES, NULL, BLEND_ALPHABLEND, &MaxPrims );

		DWORD AlphaColor = ARGB( 127, Color.r * 255, Color.g * 255, Color.b * 255 );
		for( int Idx = 0; Idx < SEGMENTS; ++Idx )
		{
			b2Vec2 NewLocation = Center + Radius * b2Vec2( cosf(Theta + INCREMENT), sinf(Theta + INCREMENT) );
			NewLocation.x *= TO_WORLD;
			NewLocation.y *= TO_WORLD;

			hgeVertex Vertice0 = { OldLocation.x, OldLocation.y, 0, AlphaColor, 0, 0 };
			hgeVertex Vertice1 = { NewLocation.x, NewLocation.y, 0, AlphaColor, 0, 0 };
			hgeVertex Vertice2 = { Center.x * TO_WORLD, Center.y * TO_WORLD, 0, AlphaColor, 0, 0 };
			Vertices[NumVertices++] = Vertice0;
			Vertices[NumVertices++] = Vertice1;
			Vertices[NumVertices++] = Vertice2;
			++NumPrims;

			Theta += INCREMENT;

			OldLocation = NewLocation;
			
		}
		g_Hge->Gfx_FinishBatch( NumPrims );

		DrawCircle( Center, Radius, Color );
		extern HGE* g_Hge;

		const Vector2 Location = Vector2( Center.x * TO_WORLD, Center.y * TO_WORLD );
		const Vector2 EndNormal = Location + B2ToVector( Axis ) * 15.0f;
		// Draw normal
		g_Hge->Gfx_RenderLine( Location.X, Location.Y, EndNormal.X, EndNormal.Y, B2TOCOLOR( Color ) );
		// Draw left part of arrow
		Vector2 Flerp = B2ToVector( Axis );
		Flerp.Rotate( 0.5f );
		g_Hge->Gfx_RenderLine( EndNormal.X, EndNormal.Y, EndNormal.X - Flerp.X * 10.0f, EndNormal.Y - Flerp.Y * 10.0f, B2TOCOLOR( Color ) );
		// Draw right part of arrow
		Flerp = B2ToVector( Axis );
		Flerp.Rotate( -0.5f );
		g_Hge->Gfx_RenderLine( EndNormal.X, EndNormal.Y, EndNormal.X - Flerp.X * 10.0f, EndNormal.Y - Flerp.Y * 10.0f, B2TOCOLOR( Color ) );
	}

	void Box2DDebugDraw::DrawSegment( const b2Vec2& P1, const b2Vec2& P2, const b2Color& Color )
	{
		extern HGE* g_Hge;
		g_Hge->Gfx_RenderLine( P1.x * TO_WORLD, P1.y* TO_WORLD, P2.x* TO_WORLD, P2.y * TO_WORLD, B2TOCOLOR( Color ) );
	}

	void Box2DDebugDraw::DrawTransform( const b2Transform& Xf )
	{
		// @TODO: Draw transform
/*		b2Vec2 p1 = xf.p, p2;
		const float32 k_axisScale = 0.4f;
		glBegin(GL_LINES);
	
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(p1.x, p1.y);
		p2 = p1 + k_axisScale * xf.q.GetXAxis();
		glVertex2f(p2.x, p2.y);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex2f(p1.x, p1.y);
		p2 = p1 + k_axisScale * xf.q.GetYAxis();
		glVertex2f(p2.x, p2.y);

		glEnd();*/
	}

	void Box2DDebugDraw::DrawPoint( const b2Vec2& P, float32 Size, const b2Color& Color )
	{
		// @TODO: Draw Point
/*		glPointSize(size);
		glBegin(GL_POINTS);
		glColor3f(color.r, color.g, color.b);
		glVertex2f(p.x, p.y);
		glEnd();
		glPointSize(1.0f);*/
	}

	void Box2DDebugDraw::DrawString( int X, int Y, const char* String, ... )
	{
		// @TODO: Draw string
/*		char buffer[128];

		va_list arg;
		va_start(arg, string);
		vsprintf(buffer, string, arg);
		va_end(arg);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		int w = glutGet(GLUT_WINDOW_WIDTH);
		int h = glutGet(GLUT_WINDOW_HEIGHT);
		gluOrtho2D(0, w, h, 0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glColor3f(0.9f, 0.6f, 0.6f);
		glRasterPos2i(x, y);
		int32 length = (int32)strlen(buffer);
		for (int32 i = 0; i < length; ++i)
		{
			glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buffer[i]);
		}

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);*/
	}

	void Box2DDebugDraw::DrawAABB( b2AABB* AABB, const b2Color& Color )
	{
		// @TODO: Draw AABB
		extern HGE* g_Hge;
		g_Hge->Gfx_RenderLine( AABB->lowerBound.x * TO_WORLD, AABB->lowerBound.y * TO_WORLD, AABB->upperBound.x * TO_WORLD, AABB->lowerBound.y * TO_WORLD, B2TOCOLOR( Color ) );
		g_Hge->Gfx_RenderLine( AABB->upperBound.x * TO_WORLD, AABB->lowerBound.y * TO_WORLD, AABB->upperBound.x * TO_WORLD, AABB->upperBound.y * TO_WORLD, B2TOCOLOR( Color ) );
		g_Hge->Gfx_RenderLine( AABB->upperBound.x * TO_WORLD, AABB->upperBound.y * TO_WORLD, AABB->lowerBound.x * TO_WORLD, AABB->upperBound.y * TO_WORLD, B2TOCOLOR( Color ) );
		g_Hge->Gfx_RenderLine( AABB->lowerBound.x * TO_WORLD, AABB->upperBound.y * TO_WORLD, AABB->lowerBound.x * TO_WORLD, AABB->lowerBound.y * TO_WORLD, B2TOCOLOR( Color ) );
	}
}