/************************************************************************/
// 创建时间: 081005
// 最后确定: 081117

#ifndef _String_h_
#define _String_h_

#include "../AnyPlay.h"

//////////////////////////////////////////////////////////////////////////
// AnyPlay名称空间
namespace AnyPlay
{
	//////////////////////////////////////////////////////////////////////////
	// 字符串模版类
	// 之所以要写成一个模版, 是因为以后做游戏输入框可能会用到
	template<typename T> class StringT
	{
	public:
		StringT() { m_pTmpStr = NULL; m_bChanged = false; }
		StringT(const StringT<T>& str) { m_pTmpStr = NULL; SetString(str); }
		StringT(const T* str) { m_pTmpStr = NULL; SetString(str); }

		//////////////////////////////////////////////////////////////////////////
		// 析构函数, 释放分配的内存
		virtual ~StringT() { SAFE_DELETE(m_pTmpStr); }

		//////////////////////////////////////////////////////////////////////////
		// 清空字符列表
		virtual void Clear() { if(m_String.size()) { m_String.clear(); m_bChanged = true; } }

		//////////////////////////////////////////////////////////////////////////
		// 获得字符串长度
		virtual dword GetLength() const { return (dword)m_String.size(); }

		//////////////////////////////////////////////////////////////////////////
		// 设置StringT字符串
		virtual void SetString(const StringT<T>& str)
		{
			if(&str == this) return;	// 如果是自己则返回
			m_bChanged = true;
			Clear();
			dword slen = str.GetLength();
			for(dword i = 0; i < slen; i++)
				m_String.push_back(str.m_String[i]);
		}

		//////////////////////////////////////////////////////////////////////////
		// 设置字符串指针
		virtual bool SetString(const T* str)
		{
			if(str)
			{
				m_bChanged = true;
				Clear();
				dword i = 0;
				while(str[i])
				{
					m_String.push_back(str[i]);
					i++;
				}
				return true;
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		// 寻找字符串
		virtual dword FindString(const StringT<T>& str, dword begin) const
		{
			dword localLen = GetLength();
			dword targetLen = str.GetLength();
			for(dword i = begin; i <= localLen - targetLen; i++)
			{
				for(dword j = 0; j < targetLen; j++)
				{
					if(m_String[i + j] != str.m_String[j]) break;
					else if(j == targetLen - 1) return i;
				}
			}
			return -1;
		}
		
		//////////////////////////////////////////////////////////////////////////
		// 逆向寻找字符串
		virtual dword InverseFindString(const StringT<T>& str, dword begin) const
		{
			dword localLen = GetLength();
			dword targetLen = str.GetLength();
			if(begin > localLen - targetLen) begin = localLen - targetLen;
			for(dword i = begin; i >= 0; i--)
			{
				for(dword j = 0; j < targetLen; j++)
				{
					if(m_String[i + j] != str.m_String[j]) break;
					else if(j == targetLen - 1) return i;
				}
			}
			return -1;
		}

		//////////////////////////////////////////////////////////////////////////
		// 从pos位置插入字符串
		virtual bool InsertString(const StringT<T>& str, dword pos)
		{
			if(IN_RANGE(m_String.begin() + pos, m_String) || (m_String.begin() + pos == m_String.end()))
			{
				m_bChanged = true;
				m_String.insert(m_String.begin() + pos, str.m_String.begin(), str.m_String.begin() + str.GetLength());
				return true;
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		// 从begin开始删除长度为len的字符串
		virtual bool RemoveString(dword begin, dword len)
		{
			if(len)
			{
				if( IN_RANGE(m_String.begin() + begin, m_String) &&
					IN_RANGE(m_String.begin() + begin + len - 1, m_String))
				{
					m_bChanged = true;
					m_String.erase(m_String.begin() + begin, m_String.begin() + begin + len);
					return true;
				}
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		// 获得字符串指针
		virtual T* GetBuffer() const
		{
			if(m_bChanged || !m_pTmpStr)
			{
				SAFE_DELETE(const_cast<T*>(m_pTmpStr));
				const_cast<T*>(m_pTmpStr) = new T[m_String.size() + 1];
				dword i = 0;
				while(m_String.begin() + i != m_String.end())
				{
					m_pTmpStr[i] = m_String[i];
					i++;
				}
				m_pTmpStr[i] = 0;	// 0结尾
				// 关闭已经改变的标记
				const_cast<StringT<T>*>(this)->m_bChanged = false;
			}
			return m_pTmpStr;
		}

		void MakeUpper()
		{
			dword localLen = GetLength();
			for(dword i = 0; i < localLen; i++)
			{
				if(m_String[i] >= _T('a') && m_String[i] <= _T('z')) m_String[i] -= 32;
			}
		}

		void MakeLower()
		{
			dword localLen = GetLength();
			for(dword i = 0; i < localLen; i++)
			{
				if(m_String[i] >= _T('A') && m_String[i] <= _T('Z')) m_String[i] += 32;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// 字符串相加, 就是从尾部插入
		virtual StringT<T> operator + (const StringT<T>& str) const
		{
			StringT<T> strTmp = *this;
			strTmp.InsertString(str, strTmp.GetLength());
			return strTmp;
		}

		//////////////////////////////////////////////////////////////////////////
		// 自身的字符串相加
		virtual void operator += (const StringT<T>& str)
		{
			InsertString(str, GetLength());
		}

		//////////////////////////////////////////////////////////////////////////
		// 从尾部添加字符
		virtual StringT<T> operator + (const T ch) const
		{
			StringT<T> strTmp = *this;
			strTmp.m_bChanged = true;
			strTmp.m_String.push_back(ch);
			return strTmp;
		}

		//////////////////////////////////////////////////////////////////////////
		// 自身从尾部添加字符
		virtual void operator += (const T ch)
		{
			m_bChanged = true;
			m_String.push_back(ch);
		}

		//////////////////////////////////////////////////////////////////////////
		// 比较两字符串是否相等
		virtual bool operator == (const StringT<T>& str) const
		{
			if(&str == this) return true;
			dword slen = GetLength();
			if(slen == str.GetLength())
			{
				for(dword i = 0; i < slen; i++)
				{
					if(str.m_String[i] != m_String[i]) return false;
				}
				return true;
			}
			return false;
		}

		//////////////////////////////////////////////////////////////////////////
		// 取字符串中间从start开始, 长度为len的字符
		virtual StringT<T> Mid(dword start, dword len) const
		{
			StringT<T> strTmp;
			if(len)		// 必须要有长度
			{
				dword localLen = GetLength();
				if(start < localLen && start + len <= localLen)
				{
					for(dword i = start; i < start + len; i++)
						strTmp.m_String.push_back(m_String[i]);
				}
			}
			return strTmp;
		}

		//////////////////////////////////////////////////////////////////////////
		// 取字符串左边len个字符
		virtual StringT<T> Left(dword len) const
		{
			StringT<T> strTmp;
			if(len)
			{
				dword localLen = GetLength();
				if(len > localLen) len = localLen;
				for(dword i = 0; i < len; i++)
					strTmp.m_String.push_back(m_String[i]);
			}
			return strTmp;
		}

		// 取字符串右边len个字符
		virtual StringT<T> Right(dword len) const
		{
			StringT<T> strTmp;
			if(len)
			{
				dword localLen = GetLength();
				if(len > localLen) len = localLen;
				for(dword i = localLen - len; i < localLen; i++)
					strTmp.m_String.push_back(m_String[i]);
			}
			return strTmp;
		};

		//////////////////////////////////////////////////////////////////////////
		// 格式化字符串
		virtual void Format(const T* format, ...)
		{
			T strBuf[1024];
			va_list vl;
			va_start(vl, format);
			_vstprintf(strBuf, format, vl);
			va_end(vl);

			SetString(strBuf);
		}

		//////////////////////////////////////////////////////////////////////////
		// 不相等判断
		virtual bool		operator != (const StringT<T>& str) const { return !(*this == str); }

		//////////////////////////////////////////////////////////////////////////
		// 赋值字符串指针
		virtual StringT<T>& operator = (const T* str) { SetString(str); return *this; }

		//////////////////////////////////////////////////////////////////////////
		// 赋值StringT
		virtual StringT<T>& operator = (const StringT<T>& str) { SetString(str); return *this; }

		//////////////////////////////////////////////////////////////////////////
		// 获得字符串指针
		virtual T*			operator () () const { return GetBuffer(); }

		//////////////////////////////////////////////////////////////////////////
		// 快速比较
		struct FastLessCompare
		{
			bool operator () (const StringT<T>& a, const StringT<T>& b) const
			{
				const dword la = a.GetLength();
				const dword lb = b.GetLength();
				if(la == lb) return (memcmp(a(), b(), la * sizeof(T)) < 0);
				return (la < lb);
			}
		};

	protected:
		std::vector<T>	m_String;	// 字符列表
		T*				m_pTmpStr;	// 字符串指针
		bool			m_bChanged;	// 字符串是否已被改变
	};

	typedef StringT<TCHAR> String;	// TCHAR型字符串(常用)
}

#endif