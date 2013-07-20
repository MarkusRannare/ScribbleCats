#ifndef SCRIBBLE_GUIHANDLER_H
#define SCRIBBLE_GUIHANDLER_H

#include "Windows.h"

class hgeGUI;
class hgeFont;

namespace Scribble
{
	class TopBar;
	class TabBar;

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

			TopBar* ShowTopBar();
			void HideTopBar();

			TabBar* ShowTabBar();
			void HideTabBar();

			void Tick( float DeltaTime );
			void Render();

			hgeGUI* GetGUI() const;

			void SetFontSettings( const FontSettings& Settings );

			const hgeFont* GetFont() const;
			void Printf( float X, float Y, DWORD Align, const char* Format, ... );
		private:
			hgeGUI* mGUI;
			TopBar* mTopBar;
			TabBar* mTabBar;
			FontSettings mCurrentFontSettings;
			hgeFont* mFont;
	};
}

#endif