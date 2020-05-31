#pragma once

	class CIniFile
	{
	public:
		CIniFile() {};
		virtual ~CIniFile() {};

		BOOL SetIniFile(LPCTSTR strIniPath);
		tstring GetModulePath();
		tstring GetFolderPath();
		tstring GetFileName();

		int GetSectionCount();
		int GetKeyCount(LPCTSTR strSection);
		tstring GetSectionName(int iIndex);
		tstring GetKeyName(LPCTSTR strSection, int iIndex);

		tstring ReadString(LPCTSTR strSection, LPCTSTR strKey, LPCTSTR strDefault);
		int ReadInt(LPCTSTR strSection, LPCTSTR strKey, int iDefault);
		double ReadDouble(LPCTSTR strSection, LPCTSTR strKey, double dDefault);
		BOOL WriteInt(LPCTSTR strSection, LPCTSTR strKey, int iValue);
		BOOL WriteDouble(LPCTSTR strSection, LPCTSTR strKey, double dValue);
		BOOL WriteString(LPCTSTR strSection, LPCTSTR strKey, LPCTSTR strValue);

	private:
		tstring m_strIniFile;
	};

