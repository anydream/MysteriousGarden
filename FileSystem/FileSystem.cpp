/************************************************************************/
// 创建时间: 081014
// 最后确定: 081031

#include "FileSystem.h"

#pragma warning(disable: 4018)

namespace AnyPlay
{
	template<> FileSystem* Singleton<FileSystem>::m_Singleton = 0;

	void File::Seek(long offset, ushort type)
	{
		switch(type)
		{
		case APFILE_SEEK_SET:	// 从开头
			if((offset < m_Size) && (offset > 0)) m_Pointer = offset;
			break;
		case APFILE_SEEK_CUR:	// 从当前位置
			if((m_Pointer + offset < m_Size) && (m_Pointer + offset > 0)) m_Pointer += offset;
			break;
		case APFILE_SEEK_END:	// 从结尾
			if((m_Size + offset - 1 < m_Size) && (m_Size + offset - 1 > 0)) m_Pointer = m_Size + offset - 1;
			break;
		}
	}

	dword File::Read(void* pOut, dword cbSize)
	{
		dword dwReaded = 0;
		if(pOut && cbSize)
		{
			for(dword i = m_Pointer; (i < m_Size) && (i < m_Pointer + cbSize); i++)
			{
				((byte*)pOut)[dwReaded] = ((byte*)m_pData)[i];
				dwReaded++;
			}
			m_Pointer += dwReaded;
		}
		return dwReaded;
	}

	dword File::ReadString(String& strOut)
	{
		strOut.Clear();
		dword dwReaded = 0;
		TCHAR tmpCh = 0;
		while(Read(&tmpCh, sizeof(TCHAR)))
		{
			if(tmpCh)
			{
				strOut += tmpCh;
				dwReaded++;
			}
			else break;
		}
		return dwReaded * sizeof(TCHAR);
	}

	bool File::Close()
	{
		if(m_bCanDelete)
		{
			SAFE_DELETE(m_pData);
			m_Size = 0;
			m_Pointer = 0;
			return true;
		}
		return false;
	}

	File& File::operator = (const File& file)
	{
		if(&file == this) return *this;
		m_pData = file.m_pData;
		m_Size = file.m_Size;
		m_Pointer = file.m_Pointer;
		const_cast<File&>(file).m_bCanDelete = false;
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	File* FileSystem::Open(const String& filename)
	{
		File* file = NULL;
		
		if(filename.GetLength())
		{
			if(m_bFromDisk)
			{
				FILE* fp = _tfopen(filename(), _T("rb"));
				if(fp)
				{
					fseek(fp, 0L, SEEK_END);
					dword dwFileSize = ftell(fp);
					fseek(fp, 0L, SEEK_SET);
					
					byte* buf = new byte[dwFileSize];
					fread(buf, dwFileSize, 1, fp);
					
					file = new File(buf, dwFileSize);
					
					fclose(fp);
				}
				else
				{
					LOG(String(_T("无法打开文件!\t")) + filename, Insane);
				}
			}
			else
			{
				// 来自资源包
			}
		}
		return file;
	}

	bool FileSystem::Close(File* pFile)
	{
		if(pFile)
		{
			pFile->Close();
			SAFE_DELETE(pFile);
			return true;
		}
		return false;
	}

}