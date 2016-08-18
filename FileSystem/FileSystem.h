/************************************************************************/
// ����ʱ��: 081014
// ���ȷ��: 081028

#ifndef _FileSystem_h_
#define _FileSystem_h_

#include "../AnyPlay.h"
#include "../FrameWork/Singleton.h"
#include "../Core/String.h"
#include "../FrameWork/Logger.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay���ƿռ�
namespace AnyPlay
{
#define APFILE_SEEK_SET 1
#define APFILE_SEEK_CUR 2
#define APFILE_SEEK_END 3

	//////////////////////////////////////////////////////////////////////////
	// �����ļ�����
	class File
	{
	public:
		File() : m_pData(NULL), m_Size(0), m_Pointer(0), m_bCanDelete(true) {}
		File(void* pData, dword cbSize) : m_pData(pData), m_Size(cbSize), m_Pointer(0), m_bCanDelete(true) {}
		File(const File& file) { *this = file; }
		virtual ~File() { Close(); }

		// �����ļ�����ָ��λ��
		void	Seek(long offset, ushort type);

		// ����ļ���ǰָ��λ��
		dword	Tell() const { return m_Pointer; }

		// �������
		void*	GetData() const { return m_pData; }

		// ������ݴ�С
		dword	GetSize() const { return m_Size; }

		// ��ȡ����
		dword	Read(void* pOut, dword cbSize);

		// ��ȡ�ַ�������
		dword	ReadString(String& strOut);

		// �ر��ļ�
		bool	Close();

		// �����ļ�����
		File&	operator = (const File& file);

	protected:
		void*	m_pData;		// �ļ�����
		dword	m_Size;			// �ļ���С
		dword	m_Pointer;		// �ļ�ָ��
		bool	m_bCanDelete;	// �Ƿ���ɾ��Ȩ
	};

	//////////////////////////////////////////////////////////////////////////
	// �ļ�ϵͳ
	class FileSystem : public Singleton<FileSystem>
	{
	public:
		FileSystem() : m_bFromDisk(true) {}

		// ���ļ�
		File*	Open(const String& filename);

		// �ر��ļ�
		bool	Close(File* pFile);

	protected:
		bool	m_bFromDisk;
	};
}
#endif