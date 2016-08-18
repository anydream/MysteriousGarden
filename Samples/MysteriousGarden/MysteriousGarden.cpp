

#include "../../AnyPlay.h"
#include "../SampleApp.h"
#include "MainInf.h"
#include <locale.h>
#include <crtdbg.h>

using namespace AnyPlay;

//////////////////////////////////////////////////////////////////////////

int _tmain(int argc, TCHAR* argv[])
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	setlocale(LC_CTYPE, "chs");

	SetCurrentDirectory(_T("..\\..\\bin"));// 这里要注意

	SampleApp mApp(new MainInf);
	return mApp.Run();

	_CrtDumpMemoryLeaks();
}