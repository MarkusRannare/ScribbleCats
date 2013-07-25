#ifndef SCRIBBLE_GUI_H
#define SCRIBBLE_GUI_H

#include "collection_types.h"
#include "hge/hge.h"

#define GNEW( Class, ... ) MAKE_NEW( memory_globals::default_allocator(), Class, __VA_ARGS__ )

namespace Scribble
{
	class GUIObject;

	class GUI
	{
		public:
			GUI();
			virtual ~GUI();

			GUIObject& AddObject( GUIObject* Object );

			void Tick( float Delta );
			void Render();

			void ObjectDirty( GUIObject* Object );
			void HandleEvent( hgeInputEvent InputEvent );
		private:
			void SetDirty( GUIObject* Object );
			void RelayoutBatch( GUIObject* Object );
			void FindFocus( float X, float Y );
		private:
			foundation::Array<GUIObject*> mObjects;
			GUIObject* mFocus;
	};
};

#endif