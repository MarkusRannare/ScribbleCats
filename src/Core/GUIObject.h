#ifndef SCRIBBLE_GUIOBJECT_H
#define SCRIBBLE_GUIOBJECT_H

#include "Bounds.h"

namespace Scribble
{
	class GUI;

	class GUIObject
	{
 		public:
			enum EMargin
			{
				M_Top,
				M_Right,
				M_Down,
				M_Left
			};
		public:
			GUIObject();
			GUIObject( const GUIObject& Other );
			~GUIObject();

			inline GUIObject& Width( float Width );
			inline GUIObject& Height( float Height );
			inline GUIObject& AutoHeight();
			inline GUIObject& AutoWidth();
			inline GUIObject& Margin( float MarginTop = 0, float MarginRight = 0, float MarginDown = 0, float MarignLeft = 0 );

			template<class T>
			GUIObject& operator+( T& Other );

			template<class T>
			GUIObject& operator[]( T& Other );

			GUIObject& operator++(int);

			virtual void Tick( float Delta );
			virtual void Render();

			virtual void LoseFocus();
			virtual void GainFocus();
		protected:
			void Dirty();
		protected:
			GUI* mGUI;
			int mBatch;
			Vector2 mLocation;
			Bounds mHitArea;
			Bounds mBoundingBox;
			float mWidth;
			float mHeight;
			float mMargin[4];
			GUIObject* mNextObject;
			GUIObject* mSubObject;
			GUIObject* mParent;

			bool mDirty;
			bool mFocused;
			bool mAutoHeight;
			bool mAutoWidth;

			friend class GUI;
	};

	inline GUIObject& GUIObject::Width( float Width )
	{
		mWidth = Width;

		return *this;
	}

	inline GUIObject& GUIObject::Height( float Height )
	{
		mHeight = Height;

		return *this;
	}

	inline GUIObject& GUIObject::AutoHeight()
	{
		mAutoHeight = true;

		return *this;
	}

	inline GUIObject& GUIObject::AutoWidth()
	{
		mAutoWidth = true;

		return *this;
	}

	inline GUIObject& GUIObject::Margin( float MarginTop, float MarginRight, float MarginDown, float MarignLeft )
	{
		mMargin[M_Top] = MarginTop;
		mMargin[M_Right] = MarginRight;
		mMargin[M_Down] = MarginDown;
		mMargin[M_Left] = MarignLeft;

		return *this;
	}

	template<class T>
	GUIObject& GUIObject::operator[]( T& Other )
	{
		assert( mSubObject == NULL );
		assert( Other.mParent == NULL );

		mSubObject = &Other;
		Other.mParent = this;
		Other.mBatch = mBatch;

		return Other;
	}

	template<class T>
	GUIObject& GUIObject::operator+( T& Other )
	{
		assert( Other.mNextObject == NULL );

		//mNextObject = &Other;
		Other.mNextObject = this;
		Other.mBatch = mBatch;

		return Other;
	}
}

#endif