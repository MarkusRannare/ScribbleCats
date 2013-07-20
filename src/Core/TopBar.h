#ifndef SCRIBBLE_TOPBAR_H
#define SCRIBBLE_TOPBAR_H

#include <Core/array.h>

class hgeSprite;

namespace Scribble
{
	class GUIHandler;
	class TopButton;

	class TopBar
	{
		public:
			TopBar( GUIHandler* Handler );
			virtual ~TopBar();

			TopButton* AddButton( const char* Text );
			void RemoveButton( TopButton* Button );

			virtual void Update( float DeltaTime );
			virtual void Render();
		private:
			hgeSprite* mSprite;
			GUIHandler* mHandler;

			foundation::Array<TopButton*> mButtons;
	};
}

#endif