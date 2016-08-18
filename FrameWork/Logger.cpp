/************************************************************************/
// ����ʱ��: 081007
// ���ȷ��: 081017

#include "Logger.h"

namespace AnyPlay
{
	template<> Logger* Singleton<Logger>::m_Singleton = 0;

	void Logger::LogEvent(const String& message, LoggingLevel level)
	{
		String strLevel;
		if(level == Errors) strLevel = _T("������Ϣ");
		else if(level == Warnings) strLevel = _T("������Ϣ");
		else if(level == Standard) strLevel = _T("һ��������");
		else if(level == Information) strLevel = _T("��Ϣ");
		else if(level == Insane) strLevel = _T("ϵͳ����");
		_tprintf(_T("%s:\t%s\n"), strLevel(), message());
		
		FireEvent(_T("Log"), LogEventArgs(message, level));

		if(level == Insane) QUIT_PROGRAM;
	}
}