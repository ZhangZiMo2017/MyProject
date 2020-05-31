#pragma once
class CViewBase : public DuiLib::WindowImplBase
{
public:
	explicit CViewBase(LPCTSTR pszXMLName);
	virtual ~CViewBase();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual DuiLib::CDuiString GetSkinFolder();

protected:
	DuiLib::CDuiString m_strXMLName;    // XMLµÄÃû×Ö
};

