
// ULoadMachineDlg.h : ͷ�ļ�
//

#pragma once


// CULoadMachineDlg �Ի���
class CULoadMachineDlg : public CDialogEx
{
// ����
public:
	CULoadMachineDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ULOADMACHINE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void ShowMainView();
	HWND m_hMainView;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
