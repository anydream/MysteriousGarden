/************************************************************************/
// 创建时间: 081123
// 最后确定: 081125

#include "AudioSystem.h"

namespace AnyPlay
{
	template<> AudioSystem* Singleton<AudioSystem>::m_Singleton = 0;

	bool Buffer::LoadBuffer(const String& filename)
	{
		FileSystem* fs = FileSystem::GetSingletonPtr();
		if(fs)
		{
			File* file = fs->Open(filename);
			if(file)
			{
				SAFE_DELETE(m_Buffer);
				m_BufferLen = file->GetSize();
				m_Buffer = new byte[m_BufferLen];
				memmove(m_Buffer, file->GetData(), file->GetSize());
				fs->Close(file);
				m_FileName = filename;
				return true;
			}
		}
		return false;
	}

	bool AudioSystem::Play(const String& name)
	{
		if(Buffer* buf = MP3Buffers.Get(name))
		{
			WMp3* wmp3 = NULL;

			for(Manager<WMp3>::ManagedMap::iterator i = RenderingList.m_ManagedMap.begin(); i != RenderingList.m_ManagedMap.end(); i++)
			{
				if(!i->second->IsPlaying())
				{
					wmp3 = i->second;
					wmp3->Close();
					break;
				}
			}
			if(!wmp3)
			{
				wmp3 = new WMp3;
				RenderingList.Add(wmp3);
			}
			wmp3->SetName(name);
			wmp3->OpenMp3Stream((char*)buf->GetBuffer(), buf->GetBufferLen(), 2000);
			wmp3->Play();
			return true;
		}
		return false;
	}

	void AudioSystem::PauseAll()
	{
		for(Manager<WMp3>::ManagedMap::iterator i = RenderingList.m_ManagedMap.begin(); i != RenderingList.m_ManagedMap.end(); i++)
		{
			i->second->Pause();
		}
	}

	void AudioSystem::ResumeAll()
	{
		for(Manager<WMp3>::ManagedMap::iterator i = RenderingList.m_ManagedMap.begin(); i != RenderingList.m_ManagedMap.end(); i++)
		{
			i->second->Resume();
		}
	}

	void AudioSystem::SetVolume(dword lv, dword rv)
	{
		for(Manager<WMp3>::ManagedMap::iterator i = RenderingList.m_ManagedMap.begin(); i != RenderingList.m_ManagedMap.end(); i++)
		{
			i->second->SetVolume(lv, rv);
		}
	}
}