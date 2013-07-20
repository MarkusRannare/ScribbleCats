#ifndef SCRIBBLE_TAB_H
#define SCRIBBLE_TAB_H

#include <hge/hgeguictrls.h>


namespace Scribble
{
	class GUIHandler;

	class Tab : public hgeGUIObject
	{
		public:
			Tab( const char* Text, GUIHandler* Handler );
			virtual ~Tab();

			virtual void Update( float DeltaTime );
			virtual void Render();
		private:
			char* mText;
			GUIHandler* mHandler;

	};
}

#endif