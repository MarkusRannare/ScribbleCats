#ifndef SCRIBBLE_TABBAR_H
#define SCRIBBLE_TABBAR_H

#include <Core/collection_types.h>

class hgeSprite;

namespace Scribble
{
	class Tab;
	class GUIHandler;

	class TabBar
	{
		public:
			TabBar( GUIHandler* Handler );
			virtual ~TabBar();

			virtual void Render();
			virtual void Update( float Dt );

			Tab* AddTab( const char* Text );
			void RemoveTab( Tab* Tab );
		private:
			hgeSprite* mSprite;
			foundation::Array<Tab*> mTabs;
			GUIHandler* mHandler;
	};
}

#endif