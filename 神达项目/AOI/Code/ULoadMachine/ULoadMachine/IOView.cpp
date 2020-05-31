#include "stdafx.h"
#include "IOView.h"

void CIOView::InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow)
{
	m_pPaintManagerUI = pPaintManagerUI;
	m_pIWorkFlow = pIWorkFlow;
	if (m_pPaintManagerUI)
	{
		CListUI *pListUI = NULL;
		pListUI = static_cast<CListUI *>(m_pPaintManagerUI->FindControl(_T("LstOutPort")));
		
		if (pListUI)
		{
			RemoveListAllItem(pListUI);
			if (m_pIWorkFlow)
			{
				for (int i = 0; i < m_pIWorkFlow->GetIOCount(_T("OutPort")); ++i)
				{
					CContainerUI *pSubUi = AddListItem(pListUI, _T("OutPort"));
					TCHAR ptcName[128] = {0};
					m_pIWorkFlow->GetIOName(i, _T("OutPort"), ptcName);
					pSubUi->GetItemAt(0)->SetText(ptcName);
					static_cast<CButtonUI *>(pSubUi->GetItemAt(1))->SetNormalImage(_T("file='GrayLed.png' dest='10,3,30,23'"));
				}
			}
		}

		pListUI = static_cast<CListUI *>(m_pPaintManagerUI->FindControl(_T("LstInPort")));

		if (pListUI)
		{
			RemoveListAllItem(pListUI);
			if (m_pIWorkFlow)
			{
				for (int i = 0; i < m_pIWorkFlow->GetIOCount(_T("InPort")); ++i)
				{
					CContainerUI *pSubUi = AddListItem(pListUI, _T("OutPort"));
					TCHAR ptcName[128] = {0};
					m_pIWorkFlow->GetIOName(i, _T("InPort"), ptcName);
					pSubUi->GetItemAt(0)->SetText(ptcName);
					pSubUi->GetItemAt(1)->SetMouseEnabled(false);
					static_cast<CButtonUI *>(pSubUi->GetItemAt(1))->SetNormalImage(_T("file='GrayLed.png' dest='10,3,30,23'"));
				}
			}
		}
	}
}

BOOL CIOView::OnClick(DuiLib::TNotifyUI& msg)
{
	BOOL bRet = FALSE;
	CContainerUI *pContainerUI = NULL;

	if (msg.pSender->GetName() == _T("BtnStatus"))
	{
		pContainerUI = static_cast<CContainerUI *>(msg.pSender->GetParent());
		CDuiString strName = pContainerUI->GetItemAt(0)->GetText();

		if (m_pIWorkFlow)
		{
			switch (m_pIWorkFlow->GetIOStatus(strName, _T("OutPort")))
			{
			case 0:
				{
					m_pIWorkFlow->OpenOutPort(strName);
				}
				break;

			case 1:
				{
					m_pIWorkFlow->CloseOutPort(strName);
				}
				break;
			}
		}

		bRet = TRUE;
	}

	return bRet;
}

void CIOView::UpdataStatus()
{
	int iCount = 0;
	int iIndex = 0;
	tstring strName = _T("");
	tstring strNorImage = _T("");
	CListUI *pListUi = NULL;
	CContainerUI *pSubUi = NULL;
	
	pListUi = static_cast<CListUI *>(m_pPaintManagerUI->FindControl(_T("LstOutPort")));
	iCount = GetListCount(pListUi);

	for (iIndex  = 0; iIndex < iCount; ++iIndex)
	{
		pSubUi = GetListItem(pListUi, iIndex);
		if (pSubUi && m_pIWorkFlow)
		{
			strName = pSubUi->GetItemAt(0)->GetText();
	
			switch (m_pIWorkFlow->GetIOStatus(strName.c_str(), _T("OutPort")))
			{
			case 0:
				{
					strNorImage = _T("file='RedLed.png' dest='10,3,30,23'");
				}
				break;

			case 1:
				{
					strNorImage = _T("file='GreenLed.png' dest='10,3,30,23'");
				}
				break;

			default:
				{
					strNorImage = _T("file='GrayLed.png' dest='10,3,30,23'");
				}
				break;
			}

			static_cast<CButtonUI *>(pSubUi->GetItemAt(1))->SetNormalImage(strNorImage.c_str());
		}		
	}

	pListUi = static_cast<CListUI *>(m_pPaintManagerUI->FindControl(_T("LstInPort")));
	iCount = GetListCount(pListUi);

	for (iIndex  = 0; iIndex < iCount; ++iIndex)
	{
		pSubUi = GetListItem(pListUi, iIndex);
		if (pSubUi && m_pIWorkFlow)
		{
			strName = pSubUi->GetItemAt(0)->GetText();

			switch (m_pIWorkFlow->GetIOStatus(strName.c_str(), _T("InPort")))
			{
			case 0:
				{
					strNorImage = _T("file='RedLed.png' dest='10,3,30,23'");
				}
				break;

			case 1:
				{
					strNorImage = _T("file='GreenLed.png' dest='10,3,30,23'");
				}
				break;

			default:
				{
					strNorImage = _T("file='GrayLed.png' dest='10,3,30,23'");
				}
				break;
			}

			static_cast<CButtonUI *>(pSubUi->GetItemAt(1))->SetNormalImage(strNorImage.c_str());
		}		
	}
}