#include "stdafx.h"
#include "CIniFile.h"

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
