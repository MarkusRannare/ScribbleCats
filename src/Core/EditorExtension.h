#ifndef SCRIBBLE_EDITOREXTENSION_H
#define SCRIBBLE_EDITOREXTENSION_H

#include <Windows.h>

namespace Scribble
{
	class EditorExtension
	{
		public:
			virtual void Destroy() = 0;
			virtual const char* GetExtensionName() const = 0;
		protected:
			virtual ~EditorExtension();
	};
}

#endif