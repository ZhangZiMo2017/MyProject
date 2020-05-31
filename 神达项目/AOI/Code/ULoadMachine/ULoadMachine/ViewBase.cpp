#include "stdafx.h"
#include "ViewBase.h"


 CViewBase::~CViewBase()
{
}

 CViewBase::CViewBase(LPCTSTR pszXMLName)
 {
	 m_strXMLName = pszXMLName;
 }

 DuiLib::CDuiString CViewBase::GetSkinFile()
 {
	 return m_strXMLName;
 }

 DuiLib::CDuiString CViewBase::GetSkinFolder()
 {
	 return _T("");
 }