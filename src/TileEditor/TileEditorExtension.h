#ifndef SCRIBBLE_TILEEDITOREXTENSION_H
#define SCRIBBLE_TILEEDITOREXTENSION_H

#include <Core/EditorExtension.h>

namespace Scribble
{
	class TileEditorExtension : public EditorExtension
	{
		public:
			virtual void Destroy();
			virtual const char* GetExtensionName() const;
		protected:
			virtual ~TileEditorExtension();
			
	};
}

#endif