

#include "AnyPlay.h"
#include <locale.h>
#include "FrameWork/Application.h"
#include "FrameWork/Interface.h"
#include <crtdbg.h>
using namespace AnyPlay;

//////////////////////////////////////////////////////////////////////////

int _tmain(int argc, TCHAR* argv[])
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	setlocale(LC_CTYPE, "chs");

	//Application mApp(NULL);
	return 0;//mApp.Run();

	_CrtDumpMemoryLeaks();
}