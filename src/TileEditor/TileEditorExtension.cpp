#include "TileEditorExtension.h"
#include <Core/memory.h>

using namespace foundation;

namespace Scribble
{
	void TileEditorExtension::Destroy()
	{
		extern foundation::Allocator* g_DefaultAllocator;
		MAKE_DELETE( *g_DefaultAllocator, TileEditorExtension, this );
	}

	TileEditorExtension::~TileEditorExtension()
	{
	}

	const char* TileEditorExtension::GetExtensionName() const
	{
		return "Tile Editor";
	}
}