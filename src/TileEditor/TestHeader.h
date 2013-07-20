#ifndef TILEEDITOR_TESTHEADER_H
#define TILEEDITOR_TESTHEADER_H

#include <Core/memory.h>
#include <Core/EditorExtension.h>
#include "Windows.h"

extern "C"
{
	__declspec(dllexport) Scribble::EditorExtension* WINAPIV CreateExtension( foundation::Allocator* DefaultAllocator, foundation::Allocator* DefaultScratchAllocator );
};

#endif