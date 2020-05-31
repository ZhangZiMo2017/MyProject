#include "stdafx.h"
#include "CIniFile.h"


tstring	GetModulePath()
{
	tstring strMoudlePath = _T("");
	TCHAR szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;//删除文件名，只获得路径
	strMoudlePath = szFilePath;
	return strMoudlePath;
}

long GetType(TCHAR* ptcPath)
{

	if (_taccess(ptcPath, 0) == -1)
	{
		return -1;
	}

	if(FILE_ATTRIBUTE_DIRECTORY & ::PathIsDirectory(ptcPath))
	{
		return 1;
	}

	if(TRUE == ::PathFileExists(ptcPath))
	{
		return 2;
	}

	return -1;
}


long CreateFileDir(TCHAR* ptcPath)
{

	if(NULL == ptcPath || _tcslen(ptcPath) <= 0)
	{
		return -1;
	}

	int iType = GetType(ptcPath);
	if(2 == iType)
	{
		return 2;
	}
	else if (1 == iType)
	{
		return 1;
	}

	__try
	{
		int iLen = _tcslen(ptcPath);
		TCHAR tmDir[MAX_PATH] = {0};
		for(int i=0;i<iLen;++i)
		{
			if(ptcPath[i] == _T('\\') || ptcPath[i] == _T('/') || i == (iLen-1))
			{
				_tcsncpy(tmDir, ptcPath, i+1);

				iType = GetType(tmDir);
				if(1 == iType || 2 == iType)
				{
					continue;
				}

				if(0 != _tmkdir(tmDir))
				{
					__leave;
				}
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return 0;
}

#define  BUFFER_SIZE     (1024*50)

tstring CIniFile::GetModulePath()
{
	TCHAR szModulPath[MAX_PATH] = {0};
	::GetModuleFileName(NULL, szModulPath, MAX_PATH);

	TCHAR *pT = _tcsrchr(szModulPath, _T('\\'));
	if (pT != NULL)
	{
		*(pT+1) = 0;
	}

	return szModulPath;
}

BOOL CIniFile::SetIniFile(LPCTSTR strIniPath)
{
	if (_taccess(strIniPath, 0) == 0)
	{
		m_strIniFile = strIniPath;
		return TRUE;
	}
	else
	{
		m_strIniFile = strIniPath;
		tstring		strFileDir = m_strIniFile;
		strFileDir = strFileDir.substr(0, strFileDir.rfind('\\') + 1);
		CreateFileDir((TCHAR* )strFileDir.c_str());
		return TRUE;
	}
}

tstring CIniFile::GetFolderPath()
{
	tstring strPath = m_strIniFile;
	if (strPath.size() > 0)
	{
		strPath = strPath.substr(0, strPath.rfind('\\') + 1);
	}

	return strPath;
}

tstring CIniFile::GetFileName()
{
	tstring strPath = m_strIniFile;
	if (strPath.size() > 0)
	{
		strPath = strPath.substr(strPath.rfind('\\') + 1);
	}

	return strPath;
}

int CIniFile::GetSectionCount()
{
	TCHAR* pBuf = new TCHAR[BUFFER_SIZE];
	ZeroMemory(pBuf, BUFFER_SIZE*sizeof(TCHAR));
	DWORD dwSize = GetPrivateProfileString(NULL, NULL, _T(""), pBuf, BUFFER_SIZE, m_strIniFile.c_str());
	int iCount = std::count(pBuf, pBuf + dwSize, _T('\0'));
	delete[] pBuf;
	return iCount;
}

int CIniFile::GetKeyCount(LPCTSTR strSection)
{
	TCHAR* pBuf = new TCHAR[BUFFER_SIZE];
	ZeroMemory(pBuf, BUFFER_SIZE*sizeof(TCHAR));
	DWORD dwSize = GetPrivateProfileString(strSection, NULL, _T(""), pBuf, BUFFER_SIZE, m_strIniFile.c_str());
	int iCount = std::count(pBuf, pBuf + dwSize, _T('\0'));
	delete[] pBuf;
	return iCount;
}

tstring CIniFile::GetSectionName(int iIndex)
{
	TCHAR* pBuf = new TCHAR[BUFFER_SIZE];
	ZeroMemory(pBuf, BUFFER_SIZE*sizeof(TCHAR));
	DWORD dwSize = GetPrivateProfileString(NULL, NULL, _T(""), pBuf, BUFFER_SIZE, m_strIniFile.c_str());
	int iCount = std::count(pBuf, pBuf + dwSize, _T('\0'));

	if (iIndex >= iCount || iIndex < 0)
	{	
		delete[] pBuf;
		return _T("");
	}
	else
	{
		tstring strReturnVal = _T("");
		TCHAR* pText = pBuf;
		for (int i = 0; i < iCount; i++)
		{
			if (i == iIndex)
			{
				break;
			}
			pText += _tcslen(pText) + 1;
		}

		strReturnVal = pText;
		delete[] pBuf;
		return strReturnVal;
	}
}

tstring CIniFile::GetKeyName(LPCTSTR strSection, int iIndex)
{
	TCHAR* pBuf = new TCHAR[BUFFER_SIZE];
	ZeroMemory(pBuf, BUFFER_SIZE*sizeof(TCHAR));
	DWORD dwSize = GetPrivateProfileString(strSection, NULL, _T(""), pBuf, BUFFER_SIZE, m_strIniFile.c_str());
	int iCount = std::count(pBuf, pBuf + dwSize, _T('\0'));

	if (iIndex >= iCount || iIndex < 0)
	{	
		delete[] pBuf;
		return _T("");
	}
	else
	{
		tstring strReturnVal = _T("");
		TCHAR* pText = pBuf;
		for (int i = 0; i < iCount; i++)
		{
			if (i == iIndex)
			{
				break;
			}
			pText += _tcslen(pText) + 1;
		}

		strReturnVal = pText;
		delete[] pBuf;
		return strReturnVal;
	}
}

int CIniFile::ReadInt(LPCTSTR strSection, LPCTSTR strKey, int iDefault)
{
	return GetPrivateProfileInt(strSection, strKey, iDefault, m_strIniFile.c_str());
}

double CIniFile::ReadDouble(LPCTSTR strSection, LPCTSTR strKey, double dDefault)
{
	double dValue = dDefault;
	TCHAR szText[128] = {0};

	GetPrivateProfileString(strSection, strKey, _T(""), szText, 128, m_strIniFile.c_str());
	if (_tcscmp(szText, _T("")) != 0)
	{
		dValue = _ttof(szText);
	}
	return dValue;
}

tstring CIniFile::ReadString(LPCTSTR strSection, LPCTSTR strKey, LPCTSTR strDefault)
{
	TCHAR szText[128] = {0};
	GetPrivateProfileString(strSection, strKey, strDefault, szText, 128, m_strIniFile.c_str());
	return szText;
}

BOOL CIniFile::WriteInt(LPCTSTR strSection, LPCTSTR strKey, int iValue)
{
	TCHAR szText[128] = {0};
	_itot_s(iValue, szText, 10);
	return WritePrivateProfileString(strSection, strKey, szText, m_strIniFile.c_str());
}

BOOL CIniFile::WriteDouble(LPCTSTR strSection, LPCTSTR strKey, double dValue)
{
	TCHAR szText[128] = {0};
	_stprintf_s(szText, _T("%f"), dValue);
	return WritePrivateProfileString(strSection, strKey, szText, m_strIniFile.c_str());
}

BOOL CIniFile::WriteString(LPCTSTR strSection, LPCTSTR strKey, LPCTSTR strValue)
{
	return WritePrivateProfileString(strSection, strKey, strValue, m_strIniFile.c_str());
}
