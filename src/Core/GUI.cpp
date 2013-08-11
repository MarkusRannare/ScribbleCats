#include "GUI.h"
#include "GUIObject.h"

#include "Array.h"
#include "Bounds.h"

using namespace foundation;

namespace Scribble
{
	GUI::GUI() :
		mObjects( memory_globals::default_allocator() ),
		mFocus( NULL )
	{
	}

	GUI::~GUI()
	{
		for( uint32_t Idx = 0; Idx < array::size( mObjects ); ++Idx )
		{
			MAKE_DELETE( memory_globals::default_allocator(), GUIObject, mObjects[Idx] );
		}
	}
	
	GUIObject& GUI::AddObject( GUIObject* Object )
	{
		assert( Object );
		assert( Object->mNextObject == NULL );
		assert( Object->mGUI == NULL );
		assert( Object->mBatch == -1 );

		array::push_back( mObjects, Object );

		Object->mGUI = this;
		Object->mBatch = array::size( mObjects ) - 1;

		return *Object;
	}

	void GUI::HandleEvent( hgeInputEvent InputEvent )
	{
		switch( InputEvent.type )
		{
			case INPUT_MOUSEMOVE:
			{
				FindFocus( InputEvent.x, InputEvent.y );
			}
			break;
		}
	}

	void GUI::FindFocus( float X, float Y )
	{
		GUIObject* NewFocus = NULL;
		for( uint32_t Idx = 0; Idx < array::size( mObjects ); ++Idx )
		{
			GUIObject* Object = mObjects[Idx];

			while( Object )
			{
				Bounds HitArea;
				bounds::from_rect( Object->mLocation, Object->mWidth, Object->mHeight, HitArea );
				if( bounds::contains( HitArea, X, Y ) )
				{
					NewFocus = Object;
					break;
				}
				Object = Object->mNextObject;
			}
		}

		if( mFocus != NewFocus )
		{
			if( mFocus )
			{
				mFocus->LoseFocus();
			}
			mFocus = NewFocus;
			if( mFocus )
			{
				mFocus->GainFocus();
			}
		}
	}

	void GUI::RelayoutBatch( GUIObject* Object )
	{
		Vector2 Location = Object->mParent ? Object->mParent->mLocation : Vector2( 0, 0 );
		
		GUIObject* O = Object;
		while( O )
		{
			O->mLocation = Location + Vector2( O->mMargin[GUIObject::M_Left], O->mMargin[GUIObject::M_Top] );

			Location += Vector2( O->mWidth + O->mMargin[GUIObject::M_Right], 0 );

			if( O->mSubObject )
			{
				RelayoutBatch( O->mSubObject );
			}
			
			O->mDirty = false;

			O = O->mNextObject;
		}
	}

	void GUI::SetDirty( GUIObject* Object )
	{
		while( Object )
		{
			Object->mDirty = true;

			GUIObject* ChildObject = Object->mSubObject;
			while( ChildObject )
			{
				SetDirty( ChildObject );
			}
			Object = Object->mNextObject;
		}
	}

	void GUI::ObjectDirty( GUIObject* Object )
	{
		assert( Object );
		assert( Object->mGUI == this );
		assert( Object->mBatch >= 0 && Object->mBatch < (int)array::size( mObjects ) );

		GUIObject* O = mObjects[Object->mBatch];

		SetDirty( O );
	}

	void GUI::Tick( float Delta )
	{
		for( uint32_t Idx = 0; Idx < array::size( mObjects ); ++Idx )
		{
			if( mObjects[Idx]->mDirty )
			{
				RelayoutBatch( mObjects[Idx] );
			}
			mObjects[Idx]->Tick( Delta );
		}
	}

	void GUI::Render()
	{
		for( uint32_t Idx = 0; Idx < array::size( mObjects ); ++Idx )
		{
			mObjects[Idx]->Render();
		}
	}
}