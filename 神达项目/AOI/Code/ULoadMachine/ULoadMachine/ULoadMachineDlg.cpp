
// ULoadMachineDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ULoadMachine.h"
#include "ULoadMachineDlg.h"
#include "afxdialogex.h"

#include "MainView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CULoadMachineDlg �Ի���




CULoadMachineDlg::CULoadMachineDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CULoadMachineDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CULoadMachineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CULoadMachineDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CULoadMachineDlg ��Ϣ�������

BOOL CULoadMachineDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	ShowMainView();
	CDialogEx::OnOK();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CULoadMachineDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CULoadMachineDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CULoadMachineDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CULoadMachineDlg::ShowMainView()
{
	::CoInitialize(NULL);
	CIniFile cIniFile;
	DuiLib::CDuiString	strPath = cIniFile.GetModulePath().c_str();
//	DuiLib::CPaintManagerUI::SetResourcePath(strPath + _T("Skin\\"));
	CString strAddress = _T("");
#ifdef _DEBUG
	DuiLib::CPaintManagerUI::SetResourcePath(strPath + _T("Skin\\"));
#else
	DuiLib::CPaintManagerUI::SetResourceZip(strPath + _T("Skin\\Skin.zip"));
#endif
	



	CMainView *pMainView = new CMainView;
	pMainView->Create(NULL, _T("ULoad"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES);
	pMainView->CenterWindow();
	pMainView->SetParent(m_hWnd);
	m_hMainView = pMainView->GetHWND();
	SetTimer(1, 200, 0);
	SetTimer(2, 1000, 0);
	pMainView->ShowModal();
	if (pMainView)
	{
		delete pMainView;
	}
	pMainView = NULL;
	::CoUninitialize();
}

void CULoadMachineDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case 1:
		{
			::PostMessage(m_hMainView, WM_ULMSG_IOSTATUS, NULL, NULL);
		}
		break;

	case 2:
		{
			::PostMessage(m_hMainView, WM_ULMSG_LOCALTIME, NULL, NULL);
		}
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
