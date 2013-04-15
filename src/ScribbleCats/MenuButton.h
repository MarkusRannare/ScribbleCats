/*#ifndef SCRIBBLE_MENU_BUTTON_H
#define SCRIBBLE_MENU_BUTTON_H

#include <hge\hgeguictrls.h>

namespace Scribble
{
	class MenuButton : public hgeGUIObject
	{
		public:
			MenuButton( int Id, hgeFont* Font, float X, float Y, char* Text );

			virtual void Render();
			virtual void Update( float Dt );

			virtual void Enter();
			virtual void Leave();
			virtual bool IsDone();

			virtual void Focus( bool bFocused );
			virtual void MouseOver( bool bOver );
			
			virtual bool MouseLButton( bool bDown );
			virtual bool KeyClick( int key, int char );
		private:
			hgeFont* mFont;
			char* mText;
	};
}

#endif*/