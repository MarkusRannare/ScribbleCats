#ifndef SCRIBBLE_TOPBAR_H
#define SCRIBBLE_TOPBAR_H

#include "GUIContainer.h"
#include "collection_types.h"

class hgeSprite;

namespace Scribble
{
	class GUIHandler;
	class TopButton;

	class TopBar : public GUIContainer
	{
		public:
			TopBar( GUIHandler* Handler );
			virtual ~TopBar();

			TopButton* AddButton( const char* Text );
			void RemoveButton( TopButton* Button );

			virtual void Tick( float DeltaTime );
			virtual void Render();
		private:
			hgeSprite* mSprite;

			foundation::Array<TopButton*> mButtons;
	};
}

#endif