#ifndef SCRIBBLE_GUIHANDLER_H
#define SCRIBBLE_GUIHANDLER_H

#include "Windows.h"

class hgeGUI;
class hgeFont;

namespace Scribble
{
	class TopBar;
	class TabBar;
	class GUIContainer;

	struct FontSettings
	{
		float Scale;
		DWORD Color;
	};

	class GUIHandler
	{
		public:
			GUIHandler();
			virtual ~GUIHandler();

			GUIContainer* CreateEditorContainer();

			void Tick( float DeltaTime );
			void Render();

			hgeGUI* GetGUI() const;

			void SetFontSettings( const FontSettings& Settings );

			const hgeFont* GetFont() const;
			void Printf( float X, float Y, DWORD Align, const char* Format, ... );

			void SetRootGUIContainer( GUIContainer* Container );
		private:
			hgeGUI* mGUI;
			FontSettings mCurrentFontSettings;
			hgeFont* mFont;
			GUIContainer* mRootContainer;
	};
}

#endif