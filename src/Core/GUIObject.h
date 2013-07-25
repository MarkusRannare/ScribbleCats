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
			inline GUIObject& Margin( float MarginTop = 0, float MarginRight = 0, float MarginDown = 0, float MarignLeft = 0 );

			GUIObject& operator+( GUIObject& Other );
			GUIObject& operator[]( GUIObject& Other );
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

			bool mDirty;
			bool mFocused;

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

	inline GUIObject& GUIObject::Margin( float MarginTop, float MarginRight, float MarginDown, float MarignLeft )
	{
		mMargin[M_Top] = MarginTop;
		mMargin[M_Right] = MarginRight;
		mMargin[M_Down] = MarginDown;
		mMargin[M_Left] = MarignLeft;

		return *this;
	}


}

#endif