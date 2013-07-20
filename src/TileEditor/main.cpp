#include "Windows.h"
#include "TestHeader.h"
#include <Core/memory.h>
#include "TileEditorExtension.h"

using namespace foundation;

Allocator* g_DefaultAllocator = NULL;
Allocator* g_DefaultScratchAllocator = NULL;

Scribble::EditorExtension* WINAPIV CreateExtension( Allocator* DefaultAllocator, Allocator* DefaultScratchAllocator )
{
	g_DefaultAllocator = DefaultAllocator;
	g_DefaultScratchAllocator = DefaultScratchAllocator;

	return MAKE_NEW( *g_DefaultAllocator, Scribble::TileEditorExtension );
}

BOOL InitDLL(){ return TRUE; }
BOOL ExitDLL(){ return TRUE; }

BOOL WINAPI DllMain(
  _In_  HINSTANCE hinstDLL,
  _In_  DWORD fdwReason,
  _In_  LPVOID lpvReserved
)
{
	BOOL ReturnCode = TRUE;
	switch( fdwReason )
	{
		case DLL_PROCESS_ATTACH:
			ReturnCode = InitDLL();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			ReturnCode = ExitDLL();
			break;
	}
	return ReturnCode;	
}