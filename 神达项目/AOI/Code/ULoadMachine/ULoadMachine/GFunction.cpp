#include "stdafx.h"
#include "GFunction.h"

void AddComboItem(DuiLib::CComboUI *pComboUI, LPCTSTR lpstrText)
{
	if (pComboUI && lpstrText)
	{
		CListLabelElementUI *pListUI = new CListLabelElementUI;
		if (pListUI)
		{
			pListUI->SetText(lpstrText);
			pComboUI->Add(pListUI);
		}		
	}
}
void RemoveComboItem(DuiLib::CComboUI *pComboUI, LPCTSTR lpstrText)
{
	if (pComboUI && lpstrText)
	{
		int		iCount = pComboUI->GetCount();

		for (int iIndex = 0; iIndex < iCount; ++iIndex)
		{
			DuiLib::CDuiString	strText = _T("");
			CControlUI *pControlUI = pComboUI->GetItemAt(iIndex);

			if (pControlUI)
			{
				strText = pControlUI->GetText();

				if (strText == lpstrText)
				{
					pComboUI->Remove(pControlUI);
					break;
				}
			}
		}
	}
}

void RemoveComboAllItem(DuiLib::CComboUI *pComboUI)
{
	if (pComboUI)
	{
		pComboUI->RemoveAll();
	}
}

DuiLib::CContainerUI* AddListItem(DuiLib::CListUI *pListUI, LPCTSTR lpstrXML)
{
	if (pListUI)
	{
		DuiLib::CDialogBuilder builder;
		DuiLib::CDuiString strXML = lpstrXML;
		strXML += _T(".xml");
		DuiLib::CContainerUI* pSubUi = static_cast<DuiLib::CContainerUI*>(builder.Create(strXML.GetData(), (UINT)0, NULL, NULL, NULL));
		if (pSubUi)
		{
			pListUI->Add(pSubUi);
		}

		return pSubUi;
	}
	else
	{
		return NULL;
	}
}

int GetListCount(DuiLib::CListUI *pListUI)
{
	int iCount = 0;

	if (pListUI)
	{
		iCount = pListUI->GetCount();
	}

	return iCount;
}

DuiLib::CContainerUI* GetListItem(DuiLib::CListUI *pListUI, const int iIndex)
{
	if (pListUI)
	{
		int		iCount = pListUI->GetCount();

		if (iIndex > iCount || iIndex < 0)
		{
			return NULL;
		}
		else
		{
			return static_cast<DuiLib::CContainerUI* >(pListUI->GetItemAt(iIndex));
		}
	}
	else
	{
		return NULL;
	}
}

void RemoveListItem(DuiLib::CListUI *pListUI, const int iIndex)
{
	if (pListUI)
	{
		int		iCount = pListUI->GetCount();

		if (iIndex > iCount || iIndex < 1)
		{
			return;
		}

		pListUI->RemoveAt(iIndex);
	}
}

void RemoveListAllItem(DuiLib::CListUI *pListUI)
{
	if (pListUI)
	{
		int		iCount = pListUI->GetCount();

		for (int iIndex = iCount - 1; iIndex >= 0; --iIndex)
		{
			pListUI->RemoveAt(iIndex);
		}
	}
}

void OptionChanged(DuiLib::COptionUI *pOptionUI, DuiLib::CPaintManagerUI *pManager)
{
	if (pOptionUI && pManager)
	{
		DuiLib::CTabLayoutUI *pTabLayoutUI = static_cast<DuiLib::CTabLayoutUI *>(pManager->FindControl(pOptionUI->GetGroup()));

		if (pTabLayoutUI)
		{
			pTabLayoutUI->SelectItem(_ttoi(pOptionUI->GetUserData().GetData()));
		}
	}
}

void AddTreeView(DuiLib::CVerticalLayoutUI *pVerUI, LPCTSTR lpstrConfig, int iCount)
{
	if (pVerUI)
	{
		DuiLib::CTreeViewUI *pTreeView = new DuiLib::CTreeViewUI;

		if (pTreeView)
		{
			DuiLib::CDuiString strText = _T("");
			strText.Format(_T("TView_%s"), lpstrConfig);
			pTreeView->SetName(lpstrConfig);
			pTreeView->SetBkColor(RGB(0, 0, 0));
			
			DuiLib::CTreeNodeUI *pTreeNode = new DuiLib::CTreeNodeUI;

			if (pTreeNode)
			{
				strText.Format(_T("TNode_%s"), lpstrConfig);
				pTreeNode->SetName(lpstrConfig);
				strText.Format(_T("%s { %d }"), lpstrConfig, iCount);
				pTreeNode->SetItemTextColor(RGB(180, 180, 180));
				
				//pTreeNode->SetAttribute(_T("folderattr"), _T("font=\2\ normalimage=\file='1.png' source='0,0,23,23' dest='7,8,23,24'\ selectedimage=\file='2.png' source='0,0,23,23' dest='7,8,23,24'\"));
				pTreeNode->SetFixedHeight(30);
				pTreeView->Add(pTreeNode);
			}

			pVerUI->Add(pTreeView);
		}


	}
}

void AddTreeNode(DuiLib::CTreeViewUI *pTreeViewUI, LPCTSTR lpstrSection, int iCount)
{
	if (pTreeViewUI)
	{
		DuiLib::CTreeNodeUI *pTreeNode = new DuiLib::CTreeNodeUI;
		DuiLib::CDuiString strText = _T("");
		if (pTreeNode)
		{
			pTreeNode->SetName(lpstrSection);
			strText.Format(_T("%s { %d }"), lpstrSection, iCount);
			pTreeNode->SetItemTextColor(RGB(180, 180, 180));
			//pTreeNode->SetAttribute(_T("folderattr"), _T("font=\2\ normalimage=\file='1.png' source='0,0,23,23' dest='7,8,23,24'\ selectedimage=\file='2.png' source='0,0,23,23' dest='7,8,23,24'\"));
			pTreeNode->SetAttribute(_T("folderattr"), _T("normalimage=\"file='1.png' source='0,0,23,23' dest='7,8,23,24'\" selectedimage=\"file='2.png' source='0,0,23,23' dest='7,8,23,24'\""));			
			pTreeNode->SetFixedHeight(30);
			pTreeNode->SetItemText(strText);
			//(static_cast<CListContainerElementUI *>(pTreeNode))->Add(pEditUI);
			pTreeViewUI->Add(pTreeNode);

		}
	}
}

void AddTreeNode(DuiLib::CTreeNodeUI *pTreeNodeUI, LPCTSTR lpstKey, LPCTSTR lpstrValue)
{
	if (pTreeNodeUI)
	{
		DuiLib::CTreeNodeUI *pTreeNode = new DuiLib::CTreeNodeUI;
		DuiLib::CDuiString strText = _T("");
		if (pTreeNode)
		{
			strText.Format(_T("TNode_%s"), lpstKey);
			pTreeNode->SetName(lpstKey);
			strText.Format(_T("%s :"), lpstKey);
			pTreeNode->SetItemTextColor(RGB(0, 0, 0));
			//pTreeNode->SetAttribute(_T("folderattr"), _T("font=\"2\" normalimage=\"file='1.png' source='0,0,23,23' dest='7,8,23,24'\ selectedimage=\file='2.png' source='0,0,23,23' dest='7,8,23,24'\""));
			pTreeNode->SetAttribute(_T("folderattr"), _T("itemfont=\"0\""));
			pTreeNode->SetFixedHeight(20);
			pTreeNode->SetFixedWidth(500);
			pTreeNode->SetItemText(strText);
			pTreeNodeUI->AddChildNode(pTreeNode);

		}
	}
}

void AddMessage(DuiLib::CVerticalLayoutUI *pVerUI, LPCTSTR lpstrTime, LPCTSTR lpstrMessage, const int iType)
{
	if (pVerUI)
	{
		if (pVerUI->GetCount() >= 30)
		{
			pVerUI->RemoveAt(29);
		}
		DuiLib::CDialogBuilder builder;
		DuiLib::CDuiString strXML = _T("");
		strXML += _T("Message.xml");
		DuiLib::CContainerUI* pSubUi = static_cast<DuiLib::CContainerUI*>(builder.Create(strXML.GetData(), (UINT)0, NULL, NULL, NULL));
		if (pSubUi)
		{
			if (pSubUi)
			{
				pSubUi->GetItemAt(0)->SetText(lpstrTime);
				switch (iType)
				{
				case WM_ULMSG_WARNING:
					{
						(static_cast<DuiLib::CLabelUI *>(pSubUi->GetItemAt(1)))->SetTextColor(0xFFFF0000);
					}
					break;

				case WM_ULMSG_INFORMATION:
					{
						(static_cast<DuiLib::CLabelUI *>(pSubUi->GetItemAt(1)))->SetTextColor(0xFF0000FF);
					}
					break;
				}
				pSubUi->GetItemAt(1)->SetText(lpstrMessage);
			}
			pVerUI->AddAt(pSubUi, 0);
		}
	}
}

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

int FMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType /* = MB_YESNO */)
{
 	int		iReturn = 0;
	DuiLib::CDuiString strXML = _T("");
	strXML= _T("MsgBox.xml");

	CMessageBox *pMessageBox = new CMessageBox(strXML.GetData(), lpCaption, lpText);

	if (pMessageBox)
	{
		pMessageBox->Create(hWnd, _T("MsgBox"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0);
		pMessageBox->CenterWindow();
		iReturn = pMessageBox->ShowModal();
		delete pMessageBox;
		pMessageBox = NULL;
	}

	return iReturn;
}

int Login(HWND hWnd)
{
	DuiLib::CDuiString strXML = _T("Login.xml");
	CLogIn *pLogin = new CLogIn(strXML.GetData());
	int iRet = 0;
	if (pLogin)
	{
		pLogin->Create(hWnd, _T("Login"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0);
		pLogin->CenterWindow();
		iRet = pLogin->ShowModal();
		delete pLogin;
		pLogin = NULL;
	}

	return iRet;
}

int ChangePsw(HWND hWnd)
{
	DuiLib::CDuiString strXML = _T("ChangePswd.xml");
	CChangePswd *pLogin = new CChangePswd(strXML.GetData());
	int iRet = 0;
	if (pLogin)
	{
		pLogin->Create(hWnd, _T("Login"), UI_WNDSTYLE_DIALOG, 0, 0, 0, 0, 0);
		pLogin->CenterWindow();
		iRet = pLogin->ShowModal();
		delete pLogin;
		pLogin = NULL;
	}

	return iRet;
}