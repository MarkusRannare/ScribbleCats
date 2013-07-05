#include "Box2DDebugDraw.h"

#include "World.h"
#include <hge/hge.h>

namespace Scribble
{
	void Box2DDebugDraw::DrawPolygon( const b2Vec2* Vertices, int32 VertexCount, const b2Color& Color )
	{
		extern HGE* g_Hge;
		for( int32 Idx = 0; Idx < VertexCount; ++Idx )
		{
			int32 ToIdx = ( Idx + 1 ) % VertexCount;
			g_Hge->Gfx_RenderLine( Vertices[Idx].x * TO_WORLD, Vertices[Idx].y* TO_WORLD, Vertices[ToIdx].x* TO_WORLD, Vertices[ToIdx].y * TO_WORLD, ARGB( 255, Color.r * 255, Color.g * 255, Color.b * 255 ) );
		}
		/*		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINE_LOOP);
		for (int32 i = 0; i < vertexCount; ++i)
		{
			glVertex2f(vertices[i].x, vertices[i].y);
		}
		glEnd();*/
	}

	void Box2DDebugDraw::DrawSolidPolygon( const b2Vec2* Vertices, int32 VertexCount, const b2Color& Color )
	{
		extern HGE* g_Hge;

		for( int32 Idx = 0; Idx < VertexCount; ++Idx )
		{
			int32 ToIdx = ( Idx + 1 ) % VertexCount;
			g_Hge->Gfx_RenderLine( Vertices[Idx].x * TO_WORLD, Vertices[Idx].y * TO_WORLD, Vertices[ToIdx].x * TO_WORLD, Vertices[ToIdx].y * TO_WORLD, ARGB( 255, Color.r * 255, Color.g * 255, Color.b * 255 ) );
		}
/*		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		for (int32 i = 0; i < vertexCount; ++i)
		{
			glVertex2f(vertices[i].x, vertices[i].y);
		}
		glEnd();
		glDisable(GL_BLEND);

		glColor4f(color.r, color.g, color.b, 1.0f);
		glBegin(GL_LINE_LOOP);
		for (int32 i = 0; i < vertexCount; ++i)
		{
			glVertex2f(vertices[i].x, vertices[i].y);
		}
		glEnd();*/
	}

	void Box2DDebugDraw::DrawCircle( const b2Vec2& Center, float32 Radius, const b2Color& Color )
	{
		/*const float32 k_segments = 16.0f;
		const float32 k_increment = 2.0f * b2_pi / k_segments;
		float32 theta = 0.0f;
		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINE_LOOP);
		for (int32 i = 0; i < k_segments; ++i)
		{
			b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
			glVertex2f(v.x, v.y);
			theta += k_increment;
		}
		glEnd();*/
	}

	void Box2DDebugDraw::DrawSolidCircle( const b2Vec2& Center, float32 Radius, const b2Vec2& Axis, const b2Color& Color )
	{
/*		const float32 k_segments = 16.0f;
		const float32 k_increment = 2.0f * b2_pi / k_segments;
		float32 theta = 0.0f;
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
		glBegin(GL_TRIANGLE_FAN);
		for (int32 i = 0; i < k_segments; ++i)
		{
			b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
			glVertex2f(v.x, v.y);
			theta += k_increment;
		}
		glEnd();
		glDisable(GL_BLEND);

		theta = 0.0f;
		glColor4f(color.r, color.g, color.b, 1.0f);
		glBegin(GL_LINE_LOOP);
		for (int32 i = 0; i < k_segments; ++i)
		{
			b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
			glVertex2f(v.x, v.y);
			theta += k_increment;
		}
		glEnd();

		b2Vec2 p = center + radius * axis;
		glBegin(GL_LINES);
		glVertex2f(center.x, center.y);
		glVertex2f(p.x, p.y);
		glEnd();*/
	}

	void Box2DDebugDraw::DrawSegment( const b2Vec2& P1, const b2Vec2& P2, const b2Color& Color )
	{
/*		glColor3f(color.r, color.g, color.b);
		glBegin(GL_LINES);
		glVertex2f(p1.x, p1.y);
		glVertex2f(p2.x, p2.y);
		glEnd();*/
	}

	void Box2DDebugDraw::DrawTransform( const b2Transform& Xf )
	{
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
/*		glPointSize(size);
		glBegin(GL_POINTS);
		glColor3f(color.r, color.g, color.b);
		glVertex2f(p.x, p.y);
		glEnd();
		glPointSize(1.0f);*/
	}

	void Box2DDebugDraw::DrawString( int X, int Y, const char* String, ... )
	{
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
/*		glColor3f(c.r, c.g, c.b);
		glBegin(GL_LINE_LOOP);
		glVertex2f(aabb->lowerBound.x, aabb->lowerBound.y);
		glVertex2f(aabb->upperBound.x, aabb->lowerBound.y);
		glVertex2f(aabb->upperBound.x, aabb->upperBound.y);
		glVertex2f(aabb->lowerBound.x, aabb->upperBound.y);
		glEnd();*/
	}
}