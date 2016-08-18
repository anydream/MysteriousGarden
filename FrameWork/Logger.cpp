/************************************************************************/
// 创建时间: 081007
// 最后确定: 081017

#include "Logger.h"

namespace AnyPlay
{
	template<> Logger* Singleton<Logger>::m_Singleton = 0;

	void Logger::LogEvent(const String& message, LoggingLevel level)
	{
		String strLevel;
		if(level == Errors) strLevel = _T("错误消息");
		else if(level == Warnings) strLevel = _T("警告消息");
		else if(level == Standard) strLevel = _T("一般性问题");
		else if(level == Information) strLevel = _T("信息");
		else if(level == Insane) strLevel = _T("系统崩溃");
		_tprintf(_T("%s:\t%s\n"), strLevel(), message());
		
		FireEvent(_T("Log"), LogEventArgs(message, level));

		if(level == Insane) QUIT_PROGRAM;
	}
}