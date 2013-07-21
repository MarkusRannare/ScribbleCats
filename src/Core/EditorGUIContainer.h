#ifndef SCRIBBLE_EDITORGUICONTAINER_H
#define SCRIBBLE_EDTIORGUICONTAINER_H

#include "GUIContainer.h"

namespace Scribble
{
	class EditorGUIContainer : public GUIContainer
	{
		public:
			EditorGUIContainer( GUIHandler* Handler );
			virtual ~EditorGUIContainer();

			virtual void Enter();
			virtual void Leave();
			
			virtual void Tick( float DeltaTime );
			virtual void Render();
	};
}

#endif