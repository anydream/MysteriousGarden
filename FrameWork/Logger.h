/************************************************************************/
// ����ʱ��: 081015
// ���ȷ��: 081017

#ifndef _Logger_h_
#define _Logger_h_

#include "../AnyPlay.h"
#include "Singleton.h"
#include "Event.h"
#include "../Core/String.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay���ƿռ�
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// ��־��¼��Ӧ����
	enum LoggingLevel
	{
		Errors,			// ����
		Warnings,		// ����
		Standard,		// һ��
		Information,	// ��Ϣ
		Insane			// ����
	};

	//////////////////////////////////////////////////////////////////////////
	// ��־��¼�ص�����
	class LogEventArgs : public EventArgs
	{
	public:
		LogEventArgs(const String& message, LoggingLevel level) : m_Message(message), m_Level(level) {}

	public:
		String			m_Message;
		LoggingLevel	m_Level;
	};

	//////////////////////////////////////////////////////////////////////////
	// ��־��¼��
	class Logger : public Singleton<Logger>, public EventSet
	{
	public:
		Logger() {}
		virtual ~Logger() {}

		virtual void LogEvent(const String& message, LoggingLevel level = Standard);
	};
}

#endif