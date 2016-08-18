/************************************************************************/
// 创建时间: 081015
// 最后确定: 081017

#ifndef _Logger_h_
#define _Logger_h_

#include "../AnyPlay.h"
#include "Singleton.h"
#include "Event.h"
#include "../Core/String.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay名称空间
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 日志记录响应级别
	enum LoggingLevel
	{
		Errors,			// 错误
		Warnings,		// 警告
		Standard,		// 一般
		Information,	// 信息
		Insane			// 崩溃
	};

	//////////////////////////////////////////////////////////////////////////
	// 日志记录回调参数
	class LogEventArgs : public EventArgs
	{
	public:
		LogEventArgs(const String& message, LoggingLevel level) : m_Message(message), m_Level(level) {}

	public:
		String			m_Message;
		LoggingLevel	m_Level;
	};

	//////////////////////////////////////////////////////////////////////////
	// 日志记录器
	class Logger : public Singleton<Logger>, public EventSet
	{
	public:
		Logger() {}
		virtual ~Logger() {}

		virtual void LogEvent(const String& message, LoggingLevel level = Standard);
	};
}

#endif