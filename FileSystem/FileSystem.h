/************************************************************************/
// 创建时间: 081014
// 最后确定: 081028

#ifndef _FileSystem_h_
#define _FileSystem_h_

#include "../AnyPlay.h"
#include "../FrameWork/Singleton.h"
#include "../Core/String.h"
#include "../FrameWork/Logger.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay名称空间
namespace AnyPlay
{
#define APFILE_SEEK_SET 1
#define APFILE_SEEK_CUR 2
#define APFILE_SEEK_END 3

	//////////////////////////////////////////////////////////////////////////
	// 单个文件对象
	class File
	{
	public:
		File() : m_pData(NULL), m_Size(0), m_Pointer(0), m_bCanDelete(true) {}
		File(void* pData, dword cbSize) : m_pData(pData), m_Size(cbSize), m_Pointer(0), m_bCanDelete(true) {}
		File(const File& file) { *this = file; }
		virtual ~File() { Close(); }

		// 设置文件操作指针位置
		void	Seek(long offset, ushort type);

		// 获得文件当前指针位置
		dword	Tell() const { return m_Pointer; }

		// 获得数据
		void*	GetData() const { return m_pData; }

		// 获得数据大小
		dword	GetSize() const { return m_Size; }

		// 读取数据
		dword	Read(void* pOut, dword cbSize);

		// 读取字符串数据
		dword	ReadString(String& strOut);

		// 关闭文件
		bool	Close();

		// 复制文件对象
		File&	operator = (const File& file);

	protected:
		void*	m_pData;		// 文件数据
		dword	m_Size;			// 文件大小
		dword	m_Pointer;		// 文件指针
		bool	m_bCanDelete;	// 是否有删除权
	};

	//////////////////////////////////////////////////////////////////////////
	// 文件系统
	class FileSystem : public Singleton<FileSystem>
	{
	public:
		FileSystem() : m_bFromDisk(true) {}

		// 打开文件
		File*	Open(const String& filename);

		// 关闭文件
		bool	Close(File* pFile);

	protected:
		bool	m_bFromDisk;
	};
}
#endif