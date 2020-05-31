
// ULoadMachine.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "ULoadMachine.h"
#include "ULoadMachineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CULoadMachineApp

BEGIN_MESSAGE_MAP(CULoadMachineApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CULoadMachineApp ����

CULoadMachineApp::CULoadMachineApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��

	if (!MyMutex())
	{
		exit(0);
	}
}


// Ψһ��һ�� CULoadMachineApp ����

CULoadMachineApp theApp;


// CULoadMachineApp ��ʼ��

BOOL CULoadMachineApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CULoadMachineDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

BOOL CULoadMachineApp::MyMutex()
{
	HANDLE hExistMutex = NULL;
	// Get the mutex_handle.
	hExistMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, _T("UnloadMachine"));
	if (NULL == hExistMutex)
	{
		// If the mutex_handle not exist, create it.
		hExistMutex = CreateMutex(NULL, FALSE, _T("UnloadMachine"));
	}
	else
	{
		// Program already running, Prompt and exit this program.
		MessageBox(NULL, _T("��ǰ����Ѿ�����!"), _T("��ʾ!"), MB_OK);
		::ReleaseMutex(hExistMutex);
		return FALSE;
	}

	return TRUE;
}
