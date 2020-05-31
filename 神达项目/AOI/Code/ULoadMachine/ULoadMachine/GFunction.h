#pragma once

//MainView 

/*Combo 操作*/
//////////////////////////////////////////////////////////////////////////
void AddComboItem(DuiLib::CComboUI *pComboUI, LPCTSTR lpstrText);
void RemoveComboItem(DuiLib::CComboUI *pComboUI, LPCTSTR lpstrText);
void RemoveComboAllItem(DuiLib::CComboUI *pComboUI);
//////////////////////////////////////////////////////////////////////////

/*List 操作*/
//////////////////////////////////////////////////////////////////////////
DuiLib::CContainerUI* AddListItem(DuiLib::CListUI *pListUI, LPCTSTR lpstrXML);
int GetListCount(DuiLib::CListUI *pListUI);
DuiLib::CContainerUI* GetListItem(DuiLib::CListUI *pListUI, const int iIndex);
void RemoveListItem(DuiLib::CListUI *pListUI, const int iIndex);
void RemoveListAllItem(DuiLib::CListUI *pListUI);
//////////////////////////////////////////////////////////////////////////

/*Option 操作*/
//////////////////////////////////////////////////////////////////////////
void OptionChanged(DuiLib::COptionUI *pOptionUI, DuiLib::CPaintManagerUI *pManager);
//////////////////////////////////////////////////////////////////////////


/*Tree 操作*/
//////////////////////////////////////////////////////////////////////////
void AddTreeView(DuiLib::CVerticalLayoutUI *pVerUI, LPCTSTR lpstrConfig, int iCount);
void AddTreeNode(DuiLib::CTreeViewUI *pTreeViewUI, LPCTSTR lpstrSection, int iCount);
void AddTreeNode(DuiLib::CTreeNodeUI *pTreeNodeUI, LPCTSTR lpstKey, LPCTSTR lpstrValue);
//////////////////////////////////////////////////////////////////////////

/*报警消息显示*/
//////////////////////////////////////////////////////////////////////////
void AddMessage(DuiLib::CVerticalLayoutUI *pVerUI, LPCTSTR lpstrTime, LPCTSTR lpstrMessage, const int iType);
//////////////////////////////////////////////////////////////////////////

int FMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType = MB_YESNOCANCEL);


tstring GetModulePath();

long CreateFileDir(TCHAR* ptcPath);
long GetType(TCHAR* ptcPath);

#define CHECKRESULT(bResult, msg) \
		if (bResult)		 \
		{					 \
			msg.pSender->SetMouseEnabled(true);\
			return;			\
		}					
#define WM_ULMSG_BEGIN (WM_USER + 0x100)

#define WM_ULMSG_IOSTATUS (WM_ULMSG_BEGIN + 1)
#define WM_ULMSG_LOCALTIME (WM_ULMSG_BEGIN + 2)
#define WM_ULMSG_UPDATAUPH (WM_ULMSG_BEGIN + 3)
#define WM_ULMSG_PUTOKP0S (WM_ULMSG_BEGIN + 4)
#define WM_ULMSG_PUTNGP0S (WM_ULMSG_BEGIN + 5)
#define WM_ULMSG_UPDATARST (WM_ULMSG_BEGIN + 6)
#define WM_ULMSG_RESETPCB (WM_ULMSG_BEGIN + 7)
#define WM_ULMSG_PICKRSTPOS (WM_ULMSG_BEGIN + 8)
#define WM_ULMSG_WARNING (WM_ULMSG_BEGIN + 9)
#define WM_ULMSG_STOP (WM_ULMSG_BEGIN + 10)
#define WM_ULMSG_START (WM_ULMSG_BEGIN + 11)
#define WM_ULMSG_SUSPEND (WM_ULMSG_BEGIN + 12)
#define WM_ULMSG_RESET (WM_ULMSG_BEGIN + 13)
#define WM_ULMSG_INFORMATION (WM_ULMSG_BEGIN + 14)
#define WM_ULMSG_INFORMATION_V (WM_ULMSG_BEGIN + 15)
#define WM_ULMSG_CHANGEPORDUCT (WM_ULMSG_BEGIN + 16)

const int COL = 9;
const int ROW = 9;

typedef struct USERINFO
{
	tstring strName;
	int iLevel;

	USERINFO()
	{
		Init();
	}

	void Init()
	{
		iLevel = 5;
		strName = _T("Op");
	}

	void SetLevel(const int iLevel)
	{
		this->iLevel = iLevel;
	}

	int GetLevel()
	{
		return iLevel;
	}
}UserInfo, *PUserInfo;

int Login(HWND hWnd);
int ChangePsw(HWND hWnd);