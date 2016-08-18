/************************************************************************/
// 创建时间: 081123
// 最后确定: 081125

#ifndef _AudioSystem_h_
#define _AudioSystem_h_

#include "../AnyPlay.h"
#include "../Core/String.h"
#include "../FrameWork/Singleton.h"
#include "../FrameWork/Manager.h"
#include "../FileSystem/FileSystem.h"
#include "Wmp3.h"

namespace AnyPlay
{
	class Buffer : public Managed<Buffer>
	{
	public:
		Buffer(const String& name) : m_Buffer(NULL), m_BufferLen(0) { SetName(name); }
		Buffer(const String& name, byte* buf, dword buflen) : m_Buffer(buf), m_BufferLen(buflen) { SetName(name); }
		Buffer(const String& name, const String& filename) : m_Buffer(NULL), m_BufferLen(0) { SetName(name); LoadBuffer(filename); }
		~Buffer() { SAFE_DELETE(m_Buffer); }

		void	SetBuffer(byte* buf, dword buflen) { m_Buffer = buf; m_BufferLen = buflen; }
		byte*	GetBuffer() const { return m_Buffer; }
		dword	GetBufferLen() const { return m_BufferLen; }

		bool	LoadBuffer(const String& filename);

	private:
		byte*	m_Buffer;
		dword	m_BufferLen;
		String	m_FileName;
	};

	//////////////////////////////////////////////////////////////////////////
	class AudioSystem : public Singleton<AudioSystem>
	{
	public:
		AudioSystem() {}

		bool	Play(const String& name);
		void	PauseAll();
		void	ResumeAll();
		void	SetVolume(dword lv, dword rv);

	public:
		Manager<Buffer>	MP3Buffers;
		Manager<WMp3>	RenderingList;
	};
}

#endif