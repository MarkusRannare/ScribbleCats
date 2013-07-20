#ifndef SCRIBBLE_TOPBUTTON_H
#define SCRIBBLE_TOPBUTTON_H

#include <hge/hgeguictrls.h>
#include "Vector2.h"
#include <Core/collection_types.h>

class hgeSprite;

namespace Scribble
{
	class GUIHandler;

	class TopButton : public hgeGUIObject
	{
		public:
			TopButton( const char* Text, GUIHandler* Handler );
			virtual ~TopButton();

			void Update( float DeltaTime );
			void Render();

			void SetLocation( float X, float Y, float& Out_NextX );

			void AddSubText( const char* Text );
			
			virtual void MouseOver( bool Over );
			virtual bool MouseLButton( bool Down );
			virtual bool MouseMove( float X, float Y );
		private:
			char* mText;
			GUIHandler* mHandler;
			Vector2 mLocation;
			bool mExpanded;
			foundation::Array<char*> mSubTexts;
			hgeSprite* mSprite;
			float mExpandedHeight;
			float mWidth;
			Vector2 mMouseOffset;
			int mSelectedIndex;
	};
}

#endif