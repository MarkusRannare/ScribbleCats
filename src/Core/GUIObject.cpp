#include "GUIObject.h"

#include "memory.h"
#include "hge/hge.h"
#include "GUI.h"

using namespace foundation;

namespace Scribble
{
	GUIObject::GUIObject() :
		mNextObject( NULL ),
		mSubObject( NULL ),
		mGUI( NULL ),
		mBatch( -1 ),
		mLocation( Vector2( 0, 0 ) ),
		mDirty( true ),
		mFocused( false ),
		mAutoWidth( false ),
		mAutoHeight( false ),
		mParent( NULL )
	{
		mMargin[0] = 0;
		mMargin[1] = 0;
		mMargin[2] = 0;
		mMargin[3] = 0;
	}

	GUIObject::GUIObject( const GUIObject& Other ) :
		mNextObject( NULL ),
		mSubObject( NULL ),
		mLocation( Vector2( 0, 0 ) ),
		mWidth( Other.mWidth ),
		mHeight( Other.mHeight ),
		mGUI( Other.mGUI ),
		mBatch( Other.mBatch ),
		mFocused( false ),
		mAutoHeight( Other.mAutoHeight ),
		mAutoWidth( Other.mAutoWidth ),
		mParent( Other.mParent )
	{
		mMargin[0] = Other.mMargin[0];
		mMargin[1] = Other.mMargin[1];
		mMargin[2] = Other.mMargin[2];
		mMargin[3] = Other.mMargin[3];
	}

	GUIObject::~GUIObject()
	{
		if( mSubObject )
		{
			MAKE_DELETE( memory_globals::default_allocator(), GUIObject, mSubObject );
		}

		if( mNextObject )
		{
			MAKE_DELETE( memory_globals::default_allocator(), GUIObject, mNextObject );
		}
	}

	void GUIObject::Tick( float Delta )
	{
		GUIObject* Object = mSubObject;
		while( Object )
		{
			Object->Tick( Delta );
			Object = Object->mSubObject;
		}

		// @TODO: This doesn't need to be calculated every frame, I want my tick clean
		if( mAutoHeight )
		{
			extern int g_ScreenHeight;
			mHeight = mParent ? mParent->mHeight : g_ScreenHeight;
		}
		if( mAutoWidth )
		{
			extern int g_ScreenWidth;
			mWidth = mParent ? mParent->mWidth : g_ScreenWidth;
		}

		// @TODO: Add a SpecialTick here that objects can override

		Object = mNextObject;
		while( Object )
		{
			Object->Tick( Delta );
			Object = Object->mNextObject;
		}
	}

	void GUIObject::Render()
	{
		GUIObject* Object = mSubObject;
		while( Object )
		{
			Object->Render();
			Object = Object->mSubObject;
		}

		DWORD Color = mFocused ? ARGB( 255, 0, 255, 0 ) : ARGB( 255, 255, 255, 255 );

		extern HGE* g_Hge;
		g_Hge->Gfx_RenderLine( mLocation.X, mLocation.Y, mLocation.X + mWidth, mLocation.Y, Color );
		g_Hge->Gfx_RenderLine( mLocation.X + mWidth, mLocation.Y, mLocation.X + mWidth, mLocation.Y + mHeight, Color );
		g_Hge->Gfx_RenderLine( mLocation.X + mWidth, mLocation.Y + mHeight, mLocation.X, mLocation.Y + mHeight, Color );
		g_Hge->Gfx_RenderLine(mLocation.X, mLocation.Y + mHeight, mLocation.X, mLocation.Y, Color );

		Object = mNextObject;
		while( Object )
		{
			Object->Render();
			Object = Object->mNextObject;
		}
	}

	void GUIObject::Dirty()
	{
		mGUI->ObjectDirty( this );
	}

	GUIObject& GUIObject::operator++(int)
	{
		// @TODO: Create factory method in GUI
		mNextObject = MAKE_NEW( memory_globals::default_allocator(), GUIObject, *this );

		mNextObject->Dirty();

		return *mNextObject;
	}

	void GUIObject::LoseFocus()
	{
		mFocused = false;
	}

	void GUIObject::GainFocus()
	{
		mFocused = true;
	}
}