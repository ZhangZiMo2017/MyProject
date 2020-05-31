#include "stdafx.h"
#include "SD_WorkFlow.h"

namespace SD_WORKFLOW
{
	UINT __stdcall InTrackThread(LPVOID lpParam)
	{
		CWorkFlow *pWorkFlow = (CWorkFlow *)lpParam;
		pWorkFlow->InTrack();
		return 0;
	}

	UINT __stdcall FailTrayThread(LPVOID lpParam)
	{
		CWorkFlow *pWorkFlow = (CWorkFlow *)lpParam;
		pWorkFlow->FailTray();
		return 0;
	}

	UINT __stdcall PassTrayThread(LPVOID lpParam)
	{
		CWorkFlow *pWorkFlow = (CWorkFlow *)lpParam;
		pWorkFlow->PassTray();
		return 0;
	}

	UINT __stdcall PickPutThread(LPVOID lpParam)
	{
		CWorkFlow *pWorkFlow = (CWorkFlow *)lpParam;
		pWorkFlow->PickAndPut();
		return 0;
	}

	UINT __stdcall AlarmThread(LPVOID lpParam)
	{
		CWorkFlow *pWorkFlow = (CWorkFlow *)lpParam;
		pWorkFlow->Alarm();
		return 0;
	}

	UINT __stdcall WorkFlowThread(LPVOID lpParam)
	{
		CWorkFlow *pWorkFlow = (CWorkFlow *)lpParam;
		pWorkFlow->WorkFlow();
		return 0;
	}

	UINT __stdcall SignalThread(LPVOID lpParam)
	{
		CWorkFlow *pWorkFlow = (CWorkFlow *)lpParam;
		pWorkFlow->Signal();
		return 0;
	}


	CWorkFlow::CWorkFlow()
	{
		m_strModuleDir = m_classIniConfig.GetModulePath();
		m_strConfigDir = m_strModuleDir + _T("Config\\");
		m_strProductDir = m_strModuleDir + _T("Product\\");
		m_strConfigIni = m_strConfigDir + _T("Config.ini");
		m_strIOIni =  m_strConfigDir + _T("IO.dat");
		m_classIniConfig.SetIniFile(m_strConfigIni.c_str());
		m_bStart = FALSE;
		m_bReset = FALSE;
		m_bSuspend = FALSE;
		m_bThread = FALSE;
		m_bAllowPutPass = FALSE;
		m_bAllowPutFail = FALSE;
		m_bTrayNRun = FALSE;

		m_bAllowMoveToEmpty = FALSE;
		m_bAllowMoveToFull = FALSE;
		m_pIVision = NULL;
		m_pIVision = NULL;
		m_pIRobot = NULL;
		m_pIUphRecord = NULL;
		m_pIFile = NULL;
		m_bAllowPutFail = FALSE;
		m_bSkipVision = FALSE;
		m_lETrayPos = 300000;
		m_iPutFailPos = 0;
		m_iPutPassPos = 0;
		m_iSuspendCount = 0;
		m_iAddDis = 0;
		m_bNULLRun = FALSE;
		CreateRobot(&m_pIRobot, 0);

		for (int i = 0; i < 50; ++i)
		{
			if (i == 10)
			{
				m_aiTestResult[i] = 0;
			}
			else if (i == 5)
			{
				m_aiTestResult[i] = 2;
			}
			else
			{
				m_aiTestResult[i] = 1;
			}
		}
		LoadConfig();
		LoadIOInfomaion();
		LoadPosInfo();
		LoadProduct();
		InitClassInfo();
		m_dPassPosHigh = -20.0;
		m_dFailPosHigh = -18.0;

		m_dPassPosHigh = m_classIniConfig.ReadDouble(ROBOT, _T("PutOKHigh"), -20.0);
		m_dFailPosHigh = m_classIniConfig.ReadDouble(ROBOT, _T("PutNgHigh"), -18.0);

		
		AM_CreateClient(&m_pIVision);
		AM_CreateFile(&m_pIFile);
//		m_pICalibClient->ConnectServer(_T("127.0.0.1"), 6666);
	

		CreateUph(&m_pIUphRecord);
		m_pIUphRecord->SetProductName(m_strProductName.c_str());
		unsigned utid;
		m_bThread = TRUE;
		m_ahThread[7] = (HANDLE)_beginthreadex(NULL, NULL, AlarmThread, this, 0, &utid);
		m_ahThread[6] = (HANDLE)_beginthreadex(NULL, NULL, SignalThread, this, 0, &utid);

		for (int i = 0; i < 6; ++i)
		{
			m_ahThread[i] = (HANDLE)_beginthreadex(NULL, NULL, WorkFlowThread, this, 0, &utid);
		}
		
	}

	CWorkFlow::~CWorkFlow()
	{
		m_bThread = FALSE;
		WaitForMultipleObjects(8, m_ahThread, TRUE, INFINITE);
		m_pIVision->DisConnect();
		m_pIVision->DisConnect();
		m_pIRobot->CloseConnect();
		AM_ReleaseClient(m_pIVision);
		AM_ReleaseFile(m_pIFile);
		ReleaseRobot(m_pIRobot);
		ReleaseUph(m_pIUphRecord);
	}

	void CWorkFlow::InitClassInfo()
	{
		m_iClassType = m_classIniConfig.ReadInt(_T("Class"), _T("ClassType"), 0);
		m_iClassPsCnt = m_classIniConfig.ReadInt(_T("Class"), _T("ClassPsCnt"), 0);
		m_iClassNgCnt = m_classIniConfig.ReadInt(_T("Class"), _T("ClassNgCnt"), 0);
		m_iClassNcCnt = m_classIniConfig.ReadInt(_T("Class"), _T("ClassNcCnt"), 0);
	}

	void CWorkFlow::LoadIOInfomaion()
	{
		CIniFile cIniFile;
		cIniFile.SetIniFile(m_strIOIni.c_str());
		tstring strKey = _T("");
		tstring strSection = _T("");
		tstring strVal = _T("");
		m_mvIOName.clear();
		m_mapPlcInfo.clear();
		for (int i = 0; i < cIniFile.GetSectionCount(); ++i)
		{
			strSection = cIniFile.GetSectionName(i);

			for (int j = 0; j < cIniFile.GetKeyCount(strSection.c_str()); ++j)
			{
				PlcInfo sPlcInfo;
				strKey = cIniFile.GetKeyName(strSection.c_str(), j);
				strVal = cIniFile.ReadString(strSection.c_str(), strKey.c_str(), _T("0,0,1,0"));
				swscanf(strVal.c_str(), _T("%d,%d,%d,%d"), &sPlcInfo.iPortNo, &sPlcInfo.iHexNo, &sPlcInfo.iStation, &sPlcInfo.iCardNo);
				m_mapPlcInfo[strSection][strKey] = sPlcInfo;
				m_mvIOName[strSection].push_back(strKey);
			}
		}
	}

	int CWorkFlow::HexToInt(TCHAR tcHex)
	{
		int iVal = 0;

		if ((tcHex >= '0') && ('9' >= tcHex))
		{
			iVal = tcHex - '0';
		}
		else if ((tcHex >= 'A') && ('F' >= tcHex))
		{
			iVal = tcHex - 'A' + 10;
		}

		return iVal;
	}

	void CWorkFlow::TransResult(const BYTE *pByData, const int iCaptureIndex)
	{
		TCHAR atcResult[2] = {0};
		tstring strData = _T("");
		int iIndex = 0 ;
		USES_CONVERSION;
		strData = A2T((char *)pByData);
		memset(m_aiResultTemp, 0, sizeof(int) * 50);

		for (int j = 0; j < m_iCaptrueCount; ++j)
		{
			iIndex = strData.find(':', iIndex);
			iIndex += 1;

			for (int i = 0; i < m_mapCaptureInfo[j].iProductCount; ++i)
			{
				if (iIndex >=  strData.size())
				{
					m_aiResultTemp[m_mapCaptureInfo[j].aiProductNo[i] - 1] = 3;
				}
				else
				{
					m_aiResultTemp[m_mapCaptureInfo[j].aiProductNo[i] - 1] = strData.at(iIndex) - '0';
				}
				
				iIndex += 2;
			}
		}
		

	}

	BOOL CWorkFlow::OpenOutPort(LPCTSTR lpstrName)
	{
		BOOL bRet = FALSE;
		
		if ((m_mapPlcInfo.find(_T("OutPort")) != m_mapPlcInfo.end())
			&& (m_mapPlcInfo[_T("OutPort")].find(lpstrName) != m_mapPlcInfo[_T("OutPort")].end()))
		{
			PlcInfo sPlcInfo;
			sPlcInfo = m_mapPlcInfo[_T("OutPort")][lpstrName];
			if (m_classGT.OpenOutPort(sPlcInfo.iPortNo, sPlcInfo.iHexNo, sPlcInfo.iStation) == 1)
			{
				bRet = TRUE;
			}
		}

		return bRet;

	}

	BOOL CWorkFlow::CloseOutPort(LPCTSTR lpstrName)
	{
		BOOL bRet = FALSE;

		if ((m_mapPlcInfo.find(_T("OutPort")) != m_mapPlcInfo.end())
			&& (m_mapPlcInfo[_T("OutPort")].find(lpstrName) != m_mapPlcInfo[_T("OutPort")].end()))
		{
			PlcInfo sPlcInfo;
			sPlcInfo = m_mapPlcInfo[_T("OutPort")][lpstrName];
			if (m_classGT.CloseOutPort(sPlcInfo.iPortNo, sPlcInfo.iHexNo, sPlcInfo.iStation) == 1)
			{
				bRet = TRUE;
			}
		}

		return bRet;

	}

	int CWorkFlow::CheckOutPort(LPCTSTR lpstrName)
	{
		int iRet = -1;

		if ((m_mapPlcInfo.find(_T("OutPort")) != m_mapPlcInfo.end())
			&& (m_mapPlcInfo[_T("OutPort")].find(lpstrName) != m_mapPlcInfo[_T("OutPort")].end()))
		{
// 			PlcInfo sPlcInfo;
// 			sPlcInfo = m_mapPlcInfo[_T("OutPort")][lpstrName];
// 			iRet = m_classGT.CheckOutPort(sPlcInfo.iPortNo, sPlcInfo.iHexNo, sPlcInfo.iStation);	
			iRet = GetIOStatus(lpstrName, _T("OutPort"));
		}

		return iRet;
	}

	int CWorkFlow::ReadInPort(LPCTSTR lpstrName)
	{
		int iRet = -1;

		if ((m_mapPlcInfo.find(_T("InPort")) != m_mapPlcInfo.end())
			&& (m_mapPlcInfo[_T("InPort")].find(lpstrName) != m_mapPlcInfo[_T("InPort")].end()))
		{
// 			PlcInfo sPlcInfo;
// 			sPlcInfo = m_mapPlcInfo[_T("InPort")][lpstrName];
// 			iRet = m_classGT.ReadInPort(sPlcInfo.iPortNo, sPlcInfo.iHexNo, sPlcInfo.iStation);	
			iRet = GetIOStatus(lpstrName, _T("InPort"));
		}

		return iRet;
	}

	void CWorkFlow::LoadPosInfo()
	{
		m_mapRobotPosInfo.clear();
		m_mapProductPosInfo.clear();
		m_vecPosIndex.clear();
		int		iPosCount = 0;
		int		iLength = 0;
		tstring strName = _T("");
		tstring strTemp = _T("");
		CIniFile classIni;

		classIni.SetIniFile((m_strConfigDir + _T("Position.dat")).c_str());

		iPosCount = classIni.GetKeyCount(RPOSITION);

		for (int i = 0; i < iPosCount; ++i)
		{
			iLength = 0;
			strName = classIni.ReadString(RPOSITION, classIni.GetKeyName(RPOSITION, i).c_str(), _T(""));
			strTemp = strName;
			RobotPosInfo sRobotInfo;
			sRobotInfo.dXPos = (float)_ttof(strTemp.substr(iLength, strTemp.find(_T(","), iLength) - iLength).c_str());
			iLength =  strTemp.find(_T(","), iLength) + 1;
			sRobotInfo.dYPos = (float)_ttof(strTemp.substr(iLength, strTemp.find(_T(","), iLength) - iLength).c_str());
			iLength =  strTemp.find(_T(","), iLength) + 1;
			sRobotInfo.dZPos = (float)_ttof(strTemp.substr(iLength, strTemp.find(_T(","), iLength) - iLength).c_str());
			iLength =  strTemp.find(_T(","), iLength) + 1;
			sRobotInfo.dUPos = (float)_ttof(strTemp.substr(iLength, strTemp.find(_T(","), iLength) - iLength).c_str());
			m_mapRobotPosInfo[classIni.GetKeyName(RPOSITION, i)] = sRobotInfo;	
			m_vecPosIndex.push_back(classIni.GetKeyName(RPOSITION, i));
		}

		
		classIni.SetIniFile((m_strProductDir + m_strProductName + _T("\\Position.dat")).c_str());

		iPosCount = classIni.GetKeyCount(RPOSITION);

		for (int i = 0; i < iPosCount; ++i)
		{
			iLength = 0;
			strName = classIni.ReadString(RPOSITION, classIni.GetKeyName(RPOSITION, i).c_str(), _T(""));
			strTemp = strName;
			RobotPosInfo sRobotInfo;
			sRobotInfo.dXPos = (float)_ttof(strTemp.substr(iLength, strTemp.find(_T(","), iLength) - iLength).c_str());
			iLength =  strTemp.find(_T(","), iLength) + 1;
			sRobotInfo.dYPos = (float)_ttof(strTemp.substr(iLength, strTemp.find(_T(","), iLength) - iLength).c_str());
			iLength =  strTemp.find(_T(","), iLength) + 1;
			sRobotInfo.dZPos = (float)_ttof(strTemp.substr(iLength, strTemp.find(_T(","), iLength) - iLength).c_str());
			iLength =  strTemp.find(_T(","), iLength) + 1;
			sRobotInfo.dUPos = (float)_ttof(strTemp.substr(iLength, strTemp.find(_T(","), iLength) - iLength).c_str());
			m_mapProductPosInfo[classIni.GetKeyName(RPOSITION, i)] = sRobotInfo;	
			m_vecPosIndex.push_back(classIni.GetKeyName(RPOSITION, i));
		}

	}

	void CWorkFlow::LoadConfig()
	{
		if (m_classIniConfig.GetKeyCount(_T("ProductList")) < 0)
		{
			m_classIniConfig.WriteString(_T("ProductList"), _T("SD"), _T("1"));
		}

		for (int i = 0; i < m_classIniConfig.GetKeyCount(_T("ProductList")); ++i)
		{
			if (m_classIniConfig.ReadInt(_T("ProductList"), m_classIniConfig.GetKeyName(_T("ProductList"), i).c_str(), 0) == 1)
			{
				m_strProductName = m_classIniConfig.GetKeyName(_T("ProductList"), i);
				break;
			}
		}

		if (m_pIUphRecord)
		{
			m_pIUphRecord->SetProductName(m_strProductName.c_str());
		}
		

		m_strRobotIp = m_classIniConfig.ReadString(ROBOT, _T("Ip"), _T("127.0.0.1"));
		m_iInitPort = m_classIniConfig.ReadInt(ROBOT, _T("InitPort"), 5000);
		m_iCtrlPort = m_classIniConfig.ReadInt(ROBOT, _T("CtrlPort"), 2001);
		m_iMovePort = m_classIniConfig.ReadInt(ROBOT, _T("MovePort"), 2000);
		m_iVisionPort = m_classIniConfig.ReadInt(ROBOT, _T("VisionPort"), 2010);
		m_dYeildTarget = m_classIniConfig.ReadDouble(_T("Yeild"), _T("Target"), 90.0);
		m_dYeildSpec = m_classIniConfig.ReadDouble(_T("Yeild"), _T("Spec"), 85.0);
		m_dSafeHigh = m_classIniConfig.ReadDouble(ROBOT, _T("SafeHigh"), 0.0);
		m_dPutOKHigh = m_classIniConfig.ReadDouble(ROBOT, _T("PutOKHigh"), -23);
		m_dPutNgHigh = m_classIniConfig.ReadDouble(ROBOT, _T("PutNgHigh"), -10);

		if (m_pIRobot)
		{
			m_pIRobot->SetRobotIp(m_strRobotIp.c_str());
			m_pIRobot->SetInitPort(m_iInitPort);
			m_pIRobot->SetMovePort(m_iMovePort);
			m_pIRobot->SetCtrlPort(m_iCtrlPort);
			m_pIRobot->SetVisionPort(m_iVisionPort);
			TCHAR atcMsg[512] = {0};
//			m_pIRobot->InitRobot(atcMsg);
		}

		m_sPickAndPutFlow.SetMainSubStep(STEP_PICKANDPUT, m_classIniConfig.ReadInt(_T("StepInfo"), _T("PickAndPut"), 0));
		m_sFailTrayFlow.SetMainSubStep(STEP_PICKANDPUT, m_classIniConfig.ReadInt(_T("StepInfo"), _T("FailTray"), 0));
		m_sInTrackFlow.SetMainSubStep(STEP_INTRACK, m_classIniConfig.ReadInt(_T("StepInfo"), _T("InTrack"), 0));

		m_iCaptrueIndex = m_classIniConfig.ReadInt(_T("StepInfo"), _T("CaptrueIndex"), 0);
		m_iPutFailPos = m_classIniConfig.ReadInt(_T("StepInfo"), _T("PutFailPos"), 0);
		m_iPutPassPos = m_classIniConfig.ReadInt(_T("StepInfo"), _T("PutPassPos"), 0);
		m_iSuckerIndex = m_classIniConfig.ReadInt(_T("StepInfo"), _T("SuckerIndex"), 0);
		m_iPickPosIndex = m_classIniConfig.ReadInt(_T("StepInfo"), _T("PickPosIndex"), 0);		
		m_bAllowPutFail = m_classIniConfig.ReadInt(_T("StepInfo"), _T("AllowPutFail"), 0);
	}

	void CWorkFlow::LoadProduct()
	{
		int		iPosCount = 0;
		int		iLength = 0;
		int		iSectionCount = 0;
		TCHAR	atcTemp[128] = {0};
		TCHAR	atcKey[128] = {0};
		tstring strName = _T("");
		tstring strTemp = _T("");
		CIniFile classIni;
		classIni.SetIniFile((m_strProductDir + m_strProductName + _T("\\Product.inf")).c_str());
		m_classIniProduct.SetIniFile((m_strProductDir + m_strProductName + _T("\\Product.inf")).c_str());

		m_lFTrayPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("FTrayPos"), 0);
		m_lETrayPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("ETrayPos"), 300000);
		m_lFTrayOffset = m_classIniProduct.ReadInt(_T("GTPos"), _T("FTrayOffset"), 0);
		m_lETrayOffset = m_classIniProduct.ReadInt(_T("GTPos"), _T("ETrayOffset"), 0);
		m_lFSensorPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("FSensorPos"), 0);
		m_lESensorPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("ESensorPos"), 0);

		m_iProductRow = classIni.ReadInt(_T("Product"), _T("Row"), 4);
		m_iProductCol = classIni.ReadInt(_T("Product"), _T("Col"), 3);
		m_iTrayRow = classIni.ReadInt(_T("Tray"), _T("Row"), 4);
		m_iTrayCol = classIni.ReadInt(_T("Tray"), _T("Col"), 10);
		m_iNGTrayRow = classIni.ReadInt(_T("Tray"), _T("NgRow"), m_iTrayRow);
		m_iNGTrayCol = classIni.ReadInt(_T("Tray"), _T("NgCol"), m_iTrayCol);
		m_iCaptrueCount = classIni.ReadInt(_T("Product"), _T("CaptrueCount"), 1);
		m_iPickCircle = classIni.ReadInt(_T("Product"), _T("PickCircle"), 3);

		m_mapTKPosInfo.clear();
		// 用于加载每次吸头取得位置上每个吸头对应的实际产品位置编号
		classIni.SetIniFile((m_strProductDir + m_strProductName + _T("\\Sucker.inf")).c_str());
		iSectionCount = classIni.GetSectionCount();

		for (int i = 0; i < m_iPickCircle; ++i)
		{
			TKPosInfo sTKPosInfo;
			int iIndex = 0;
			swprintf(atcTemp, _T("Sucker_%d"), i);
			sTKPosInfo.iTakeMode = classIni.ReadInt(atcTemp, _T("PickMode"), 0);
			sTKPosInfo.iTkCount = classIni.ReadInt(atcTemp, _T("PickCount"), 1);
			sTKPosInfo.aiSuckerToPos[0] = classIni.ReadInt(atcTemp, _T("Sucker_0_Pos"), 0 + i * 4);
			sTKPosInfo.aiSuckerToPos[1] = classIni.ReadInt(atcTemp, _T("Sucker_1_Pos"), 1 + i * 4);
			sTKPosInfo.aiSuckerToPos[2] = classIni.ReadInt(atcTemp, _T("Sucker_2_Pos"), 2 + i * 4);
			sTKPosInfo.aiSuckerToPos[3] = classIni.ReadInt(atcTemp, _T("Sucker_3_Pos"), 3 + i * 4);
			strTemp = classIni.ReadString(atcTemp, _T("Tool1"), _T("0.0, 0.0, 0.0"));
			sTKPosInfo.asToolSet[0].dXPos = _ttof(strTemp.substr(iIndex, strTemp.find(',', iIndex) - iIndex).c_str());
			iIndex = strTemp.find(',', iIndex) + 1;
			sTKPosInfo.asToolSet[0].dYPos = _ttof(strTemp.substr(iIndex, strTemp.find(',', iIndex) - iIndex).c_str());
			iIndex = strTemp.find(',', iIndex) + 1;
			sTKPosInfo.asToolSet[0].dUPos = _ttof(strTemp.substr(iIndex).c_str());
			iIndex = 0;
			strTemp = classIni.ReadString(atcTemp, _T("Tool2"), _T("0.0, 0.0, 0.0"));
			sTKPosInfo.asToolSet[1].dXPos = _ttof(strTemp.substr(iIndex, strTemp.find(',', iIndex) - iIndex).c_str());
			iIndex = strTemp.find(',', iIndex) + 1;
			sTKPosInfo.asToolSet[1].dYPos = _ttof(strTemp.substr(iIndex, strTemp.find(',', iIndex) - iIndex).c_str());
			iIndex = strTemp.find(',', iIndex) + 1;
			sTKPosInfo.asToolSet[1].dUPos = _ttof(strTemp.substr(iIndex).c_str());
			iIndex = 0;
			strTemp = classIni.ReadString(atcTemp, _T("Tool3"), _T("0.0, 0.0, 0.0"));
			sTKPosInfo.asToolSet[2].dXPos = _ttof(strTemp.substr(iIndex, strTemp.find(',', iIndex) - iIndex).c_str());
			iIndex = strTemp.find(',', iIndex) + 1;
			sTKPosInfo.asToolSet[2].dYPos = _ttof(strTemp.substr(iIndex, strTemp.find(',', iIndex) - iIndex).c_str());
			iIndex = strTemp.find(',', iIndex) + 1;
			sTKPosInfo.asToolSet[2].dUPos = _ttof(strTemp.substr(iIndex).c_str());
			iIndex = 0;
			strTemp = classIni.ReadString(atcTemp, _T("Tool4"), _T("0.0, 0.0, 0.0"));
			sTKPosInfo.asToolSet[3].dXPos = _ttof(strTemp.substr(iIndex, strTemp.find(',', iIndex) - iIndex).c_str());
			iIndex = strTemp.find(',', iIndex) + 1;
			sTKPosInfo.asToolSet[3].dYPos = _ttof(strTemp.substr(iIndex, strTemp.find(',', iIndex) - iIndex).c_str());
			iIndex = strTemp.find(',', iIndex) + 1;
			sTKPosInfo.asToolSet[3].dUPos = _ttof(strTemp.substr(iIndex).c_str());
			iIndex = 0;

			m_mapTKPosInfo[i] = sTKPosInfo;
		}
		m_mapCaptureInfo.clear();
		//用于加载每次的拍照位置对应的实际的取放位置的编号
		classIni.SetIniFile((m_strProductDir + m_strProductName + _T("\\Capture.inf")).c_str());
		
		for (int i = 0; i < m_iCaptrueCount; ++i)
		{
			CaptrueInfo sCaptrueInfo;
			swprintf(atcTemp, _T("Capture_%d"), i);
			sCaptrueInfo.iProductCount = classIni.ReadInt(atcTemp, _T("ProductCount"), 1);
			sCaptrueInfo.iRow = classIni.ReadInt(atcTemp, _T("Row"), 1);
			sCaptrueInfo.iCol = classIni.ReadInt(atcTemp, _T("Col"), 1);
			
			for (int j = 0; j < sCaptrueInfo.iProductCount; ++j)
			{
				swprintf(atcKey, _T("Product_%d"), j);
				sCaptrueInfo.aiProductNo[j] = classIni.ReadInt(atcTemp, atcKey, j);
			}

			m_mapCaptureInfo[i] = sCaptrueInfo;
		}
	}

	BOOL CWorkFlow::ChangeProduct(LPCTSTR lpstrName)
	{
		BOOL bRet = FALSE;
		USES_CONVERSION;
		if (!m_bStart && (m_strProductName != lpstrName))
		{
			if (m_pIVision && m_pIVision->IsConnect())
			{
				TCHAR atcMsg[128] = {0};
				BYTE  abyRecv[128] = {0};
				swprintf(atcMsg, _T("Product:%s"), m_strProductName.c_str());
				
				m_pIVision->SendData((BYTE *)T2A(atcMsg), _tcslen(atcMsg));
				Sleep(500);
				m_pIVision->RecvData(abyRecv, 128);

				tstring strRecv = _T("");

				strRecv = A2T((char *)abyRecv);

				if (strRecv == _T("Product:OK"))
				{
					bRet = TRUE;
				}
			}
			else
			{
				bRet = TRUE;
			}
			
			
			if (bRet)
			{
				m_classIniConfig.WriteString(_T("ProductList"), m_strProductName.c_str(), _T("0"));
				m_strProductName = lpstrName;
				m_classIniConfig.WriteString(_T("ProductList"), m_strProductName.c_str(), _T("1"));
				LoadConfig();
				//			LoadIOInfomaion();
				LoadPosInfo();
				LoadProduct();

				PostMsg(WM_ULMSG_CHANGEPORDUCT, NULL, NULL);
			}
			
			
		
		}

		return bRet;
	}

	SIZE CWorkFlow::GetProductSize()
	{
		SIZE sSize;
		sSize.cx = m_iProductRow;
		sSize.cy = m_iProductCol;
		return sSize;
	}

	SIZE CWorkFlow::GetTraySize()
	{
		SIZE sSize;
		sSize.cx = m_iTrayRow;
		sSize.cy = m_iTrayCol;
		return sSize;
	}

	SIZE CWorkFlow::GetNgTraySize()
	{
		SIZE sSize;
		sSize.cx = m_iNGTrayRow;
		sSize.cy = m_iNGTrayCol;
		return sSize;
	}

	void CWorkFlow::F_ResetTray(StepInfo &sStepInfo)
	{
		switch (sStepInfo.GetSubStep())
		{
		case 0:
			{
				//进Tray电机回零
			}
			break;

		case 1:
			{
				//收Tray电机回零
			}
			break;

		case 2:
			{
				//检测是否运动完成 清零位置
			}
			break;

		case 3:
			{
				//已在气缸原位
			}
			break;

		case 4:
			{
				//检测已在气缸达到原位
			}
			break;
		}
	}

	void CWorkFlow::F_ResetRobot(StepInfo &sStepInfo)
	{
		switch (sStepInfo.GetSubStep())
		{
		case 0:
			{
				//机械手移动到等待位置
			}
			break;

		case 1:
			{
				//检测吸头是否有板
			}
			break;

		case 2:
			{
				//如果有板丢弃板

			}
			break;

		case 3:
			{
				//检测是否运动停止
			}
			break;

		case 4:
			{
				//机械手复位完成
			}
			break;
		}
	}

	void CWorkFlow::F_ResetFail(StepInfo &sStepInfo)
	{
		
	}

	void CWorkFlow::F_CalibCamera(StepInfo &sStepInfo)
	{
		switch (sStepInfo.GetSubStep())
		{
			

		case 0:
			{
				//检测吸头真空
				m_iCalibPosIndex = 0;
				if (m_pIRobot && (m_pIRobot->ChangeTool(1) == 1))
				{
					sStepInfo.SetSubStep(1);
				}
			}
			break;

		case 1:
			{
				//检测m_iCalibPosIndex 是否已经超过9点标定的点数
				TCHAR ptcName[128] = {0};

				swprintf(ptcName, SD_ROBOT_CAMERA_CAL_POS, m_iCalibPosIndex);

				if (m_pIRobot->JumpPosMod(m_mapRobotPosInfo[ptcName], ROBOT_SAFEHIGH))
				{
					sStepInfo.SetSubStep(2);
				}
			}
			break;

		case 2:
			{
				//移动到标定点位置

				if (!m_pIRobot->IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(3);
				}
			}
			break;

		case 3:
			{
				//检测一定是否停止
				sStepInfo.SetSubStep(4);
			}
			break;

		case 4:
			{
				//发送拍照信号
				if (m_pIVision->IsConnect() && m_pIVision->SendData((BYTE *)("Model:Camera"), strlen("Model:Camera")))
				{
					sStepInfo.SetSubStep(5);
				}
			}
			break;

		case 5:
			{
				//等待模板匹配结果
				BYTE abyData[256] = {0};

				if (m_pIVision->RecvData(abyData, 256) > 0)
				{
					TCHAR atcName[128] = {0};
					TCHAR atcData[128] = {0};
					double dXVal = 0.0;
					double dYVal = 0.0;
					double dUVal = 0.0;
					swprintf(atcName, SD_ROBOT_CAMERA_CAL_VAL, m_iCalibPosIndex);
					tstring strValue = _T("");
					tstring strOKFlag = _T("");
					USES_CONVERSION;
					strValue = A2T((char *)abyData);

					
					strOKFlag = strValue.substr(0, 3);
					if (strOKFlag == _T("OK:"))
					{
						int iIndex = 3;
						dXVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
						iIndex = strValue.find(',', iIndex) + 1;
						dYVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
						iIndex = strValue.find(',', iIndex) + 1;
						dUVal = _ttof(strValue.substr(iIndex).c_str());
						m_dAngle = dUVal;

					
						m_mapRobotPosInfo[atcName].dXPos = (float)dXVal;
						m_mapRobotPosInfo[atcName].dYPos = (float)dYVal;
						m_mapRobotPosInfo[atcName].dUPos = 0.0;
						m_mapRobotPosInfo[atcName].dZPos = 0.0;

						CIniFile classIni;

						classIni.SetIniFile((m_strConfigDir + _T("Position.dat")).c_str());
						swprintf(atcData, _T("%.3f,%.3f,0.0,0.0,"), dXVal, dYVal);
						classIni.WriteString(RPOSITION, atcName, atcData);
						sStepInfo.SetSubStep(6);
					}

// 					int iIndex = 0;
// 					dXVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
// 					iIndex = strValue.find(',', iIndex) + 1;
// 					dYVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
// 					iIndex = strValue.find(',', iIndex) + 1;
// 					dUVal = _ttof(strValue.substr(iIndex).c_str());

// 					m_mapRobotPosInfo[atcName].dXPos = (float)dXVal;
// 					m_mapRobotPosInfo[atcName].dYPos = (float)dYVal;
// 					m_mapRobotPosInfo[atcName].dUPos = 0.0;
// 					m_mapRobotPosInfo[atcName].dZPos = 0.0;
// 
// 					CIniFile classIni;
// 
// 					classIni.SetIniFile((m_strConfigDir + _T("Position.dat")).c_str());
// 					swprintf(atcData, _T("%.3f,%.3f,0.0,0.0,"), dXVal, dYVal);
// 					classIni.WriteString(RPOSITION, atcName, atcData);
// 					sStepInfo.SetSubStep(6);
				}
			}
			break;

		case 6:
			{
				++m_iCalibPosIndex;

				if (m_iCalibPosIndex < 9)
				{
					sStepInfo.SetSubStep(1);
				}
				else
				{
					sStepInfo.SetSubStep(7);
				}
			}
			break;

		case 7:
			{
				TCHAR	atcPosName[256] = {0};
				for (int i = 0; i < 9; ++i)
				{		
					swprintf(atcPosName, SD_ROBOT_CAMERA_CAL_POS, i);
					// 发送坐标
					m_pIRobot->SetPosition(910 + i, m_mapRobotPosInfo[atcPosName]);
				}

				sStepInfo.SetSubStep(8);
			}
			break;

		case 8:
			{
				TCHAR	ptcPosName[256] = {0};
				for (int i = 0; i < 9; ++i)
				{		
					swprintf(ptcPosName, SD_ROBOT_CAMERA_CAL_VAL, i);
					// 发送坐标
					m_pIRobot->SetPosition(900 + i, m_mapRobotPosInfo[ptcPosName]);
				}

				sStepInfo.SetSubStep(9);
			}
			break;

		case 9:
			{
				//发送开始计算数据 保存计算数据
				TCHAR	ptcReturn[512] = {0};
				m_pIRobot->CameraCal(1, 1, ptcReturn);
				sStepInfo.SetSubStep(10);
			}
			break;

		case 10:
			{
				//发送完成
				m_pIRobot->ChangeTool(0);
				sStepInfo.SetMainSubStep(-1, 0);
			}
			break;
		}
	}

	void CWorkFlow::F_CreatePickTool(StepInfo &sStepInfo)
	{
		TCHAR atcName[128] = {0};
		BOOL bRet = TRUE;
		switch (sStepInfo.GetSubStep())
		{
		case 0:
			{
				m_iPickPosIndex = 0;
				sStepInfo.SetSubStep(1);
			}
			break;

		case 1:
			{
				//移动到等待位置
				if (JumpPos(SD_ROBOT_WAIT_POS))
				{
					sStepInfo.SetSubStep(2);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 2:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					m_iSuckerIndex = 0;
					sStepInfo.SetSubStep(3);
				}
			}
			break;

		case 3:
			{

				if ((m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0))
				{
					if (m_iSuckerIndex < m_mapTKPosInfo[m_iPickPosIndex].iTkCount)
					{
						//移动到第m_iSuckerIndex位置取板
						swprintf(atcName, SD_ROBOT_PICK_POS, m_iPickPosIndex + 1, m_iSuckerIndex + 1);

						if (JumpPos(atcName))
						{
							sStepInfo.SetSubStep(5);
						}
					}
					else
					{

					}
				}
				else
				{
					//移动到第0位置取板
					swprintf(atcName, SD_ROBOT_PICK_POS, m_iPickPosIndex + 1, 1);
					if (JumpPos(atcName))
					{
						sStepInfo.SetSubStep(5);
					}
				}
			}
			break;

		case 4:
			{
				//开启所有真空
				
				bRet = TRUE;
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					swprintf(atcName, SD_GT_O_SUCKERVAC_OPEN, m_iSuckerIndex + 1);
					bRet &= OpenOutPort(atcName);
					swprintf(atcName, SD_GT_O_SUCKERVAC_CLOSE, m_iSuckerIndex + 1);
					bRet &= CloseOutPort(atcName);
				}
				else
				{
					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_O_SUCKERVAC_OPEN, i + 1);
						bRet &= OpenOutPort(atcName);
						swprintf(atcName, SD_GT_O_SUCKERVAC_CLOSE, i + 1);
						bRet &= CloseOutPort(atcName);
					}
				}
			

				if (bRet)
				{
					sStepInfo.SetSubStep(8);
				}

			}
			break;



		case 5:
			{
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
					if (OpenOutPort(atcName))
					{
						sStepInfo.SetSubStep(6);
						sStepInfo.UpdataStTime();
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, i + 1);
						bRet &=OpenOutPort(atcName);
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(6);
						sStepInfo.UpdataStTime();
					}
				}
			}
			break;

		case 6:
			{
				//气缸是否到位
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
					if (ReadInPort(atcName) == 1)
					{
						sStepInfo.SetSubStep(7);
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, i + 1);
						if (ReadInPort(atcName) != 1)
						{
							bRet = FALSE;
						}
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(7);
					}
				}
			}
			break;

		case 7:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(4);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 8:
			{
				if (sStepInfo.IsTimeOut(100))
				{
					sStepInfo.SetSubStep(9);
				}
			}
			break;

		case 9:
			{
				//气缸上升
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
					if (CloseOutPort(atcName))
					{
						sStepInfo.SetSubStep(10);
						sStepInfo.UpdataStTime();
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, i + 1);
						bRet &=CloseOutPort(atcName);
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(10);
						sStepInfo.UpdataStTime();
					}
				}
			}
			break;

		case 10:
			{
				//检测气缸上升到位
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_I_SUCKERCYL_UP, m_iSuckerIndex + 1);
					if (ReadInPort(atcName) == 1)
					{
						sStepInfo.SetSubStep(11);
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_I_SUCKERCYL_UP, i + 1);
						if (ReadInPort(atcName) != 1)
						{
							bRet = FALSE;
						}
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(11);
					}
				}
			}
			break;

		case 11:
			{
				//检测真空
				//检测气缸上升到位
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_I_SUCKERVAC, m_iSuckerIndex + 1);
					if (ReadInPort(atcName) == 1)
					{
						sStepInfo.SetSubStep(12);
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_I_SUCKERVAC, i + 1);
						if (ReadInPort(atcName) != 1)
						{
							bRet = FALSE;
						}
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(12);
					}
				}

			}
			break;

		case 12:
			{
				//移动到等待位置
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					++m_iSuckerIndex;
					if (m_iSuckerIndex < m_mapTKPosInfo[m_iPickPosIndex].iTkCount)
					{
						sStepInfo.SetSubStep(3);
					}
					else
					{
						if (JumpPos(SD_ROBOT_TRANSOK_POS))
						{
							m_iSuckerIndex = 0;
							sStepInfo.SetSubStep(13);
							sStepInfo.UpdataStTime();
						}
					}
				}
				else
				{
					if (JumpPos(SD_ROBOT_TRANSOK_POS))
					{
						m_iSuckerIndex = 0;
						sStepInfo.SetSubStep(13);
						sStepInfo.UpdataStTime();
					}
				}
				
				
			}
			break;

		case 13:
			{
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(14);
				}				
			}
			break;

		case 14:
			{
				if ((m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0))
				{
					if (m_iSuckerIndex < m_mapTKPosInfo[m_iPickPosIndex].iTkCount)
					{
						//移动到第m_iSuckerIndex位标定位置
						swprintf(atcName, SD_ROBOT_SUCKER_TOOL, m_iSuckerIndex + 1);

						if (JumpPos(atcName))
						{
							sStepInfo.SetSubStep(15);
						}
					}
					else
					{
						m_iSuckerIndex = 0;
						sStepInfo.SetSubStep(30);
					}
				}
				else
				{
					//移动到第0位置取板
					if (m_iSuckerIndex < 4)
					{
						swprintf(atcName, SD_ROBOT_SUCKER_TOOL, m_iSuckerIndex + 1);
						if (JumpPos(atcName))
						{
							sStepInfo.SetSubStep(15);
						}
					}
					else
					{
						m_iSuckerIndex = 0;
						sStepInfo.SetSubStep(30);
					}
					
				}
			}
			break;

		case 15:
			{
				swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
				if (OpenOutPort(atcName))
				{
					sStepInfo.SetSubStep(16);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 16:
			{
				swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
				if (ReadInPort(atcName) == 1)
				{
					sStepInfo.SetSubStep(17);
				}
			}
			break;

		case 17:
			{
				if (!IsRobotBusy())
				{
					sStepInfo.SetSubStep(18);
				}
			}
			break;

		case 18:
			{
				//发送拍照模板匹配信号
				if (m_pIVision->IsConnect() && m_pIVision->SendData((BYTE *)("Model:PCB"), strlen("Model:PCB")))
				{
					sStepInfo.SetSubStep(19);
				}

			}
			break;
			
		case 19:
			{
				//
				BYTE abyData[256] = {0};

				if (m_pIVision->RecvData(abyData, 256) > 0)
				{
					double dXVal = 0.0;
					double dYVal = 0.0;
					double dUVal = 0.0;
					tstring strValue = _T("");
					tstring strOKFlag = _T("");
					USES_CONVERSION;
					strValue = A2T((char *)abyData);
					strOKFlag = strValue.substr(0, 3);
					if (strOKFlag == _T("OK:"))
					{
						int iIndex = 3;
						dXVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
						iIndex = strValue.find(',', iIndex) + 1;
						dYVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
						iIndex = strValue.find(',', iIndex) + 1;
						dUVal = _ttof(strValue.substr(iIndex).c_str());
						m_dAngle = dUVal;

						if (m_pIRobot->CreateTool(1, m_iSuckerIndex + 1, dXVal, dYVal))
						{
							sStepInfo.SetSubStep(20);
						}
					}
					
				}
			}
			break;

		case 20:
			{
				double dXVal = 0.0;
				double dYVal = 0.0;
				double dZVal = 0.0;
				double dUVal = 0.0;
				int iFlag = m_classIniConfig.ReadInt(_T("Config"), _T("AngleDir"), 0);
				if (m_pIRobot->GetTool(m_iSuckerIndex + 1, dXVal, dYVal, dZVal, dUVal) == 1)
				{
					m_mapTKPosInfo[m_iPickPosIndex].asToolSet[m_iSuckerIndex].dXPos = dXVal;
					m_mapTKPosInfo[m_iPickPosIndex].asToolSet[m_iSuckerIndex].dYPos = dYVal;
					if (iFlag)
					{
						m_mapTKPosInfo[m_iPickPosIndex].asToolSet[m_iSuckerIndex].dUPos = m_dAngle;
					}
					else
					{
						m_mapTKPosInfo[m_iPickPosIndex].asToolSet[m_iSuckerIndex].dUPos = -m_dAngle;
					}
					
					CIniFile classIni;
					classIni.SetIniFile((m_strProductDir + m_strProductName + _T("\\Sucker.inf")).c_str());
					swprintf(atcName, _T("Tool%d"), m_iSuckerIndex + 1);
					TCHAR atcData[128] = {0};
					TCHAR atcKey[128] = {0};
					swprintf(atcData, _T("%.3f,%.3f,%.3f"), dXVal, dYVal, m_dAngle);
					swprintf(atcKey, _T("Sucker_%d"), m_iPickPosIndex);
					classIni.WriteString(atcKey, atcName, atcData);
					sStepInfo.SetSubStep(21);
				}
			}
			break;

		case 21:
			{
				swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
				if (CloseOutPort(atcName))
				{
					sStepInfo.SetSubStep(22);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 22:
			{
				swprintf(atcName, SD_GT_I_SUCKERCYL_UP, m_iSuckerIndex + 1);
				if (ReadInPort(atcName) == 1)
				{
					++m_iSuckerIndex;
					sStepInfo.SetSubStep(14);
				}
			}
			break;

		case 30:
			{
				if ((m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0))
				{
					if (m_iSuckerIndex < m_mapTKPosInfo[m_iPickPosIndex].iTkCount)
					{
						//移动到第m_iSuckerIndex位置取板
						swprintf(atcName, SD_ROBOT_PICK_POS, m_iPickPosIndex + 1, m_iSuckerIndex + 1);

						if (JumpPos(atcName))
						{
							sStepInfo.SetSubStep(31);
						}
					}
					else
					{
						sStepInfo.SetSubStep(40);
					}
				}
				else
				{
					//移动到第0位置取板
					swprintf(atcName, SD_ROBOT_PICK_POS, m_iPickPosIndex + 1, 1);
					if (JumpPos(atcName))
					{
						sStepInfo.SetSubStep(31);
					}
				}
			}
			break;

		case 31:
			{
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
					if (OpenOutPort(atcName))
					{
						sStepInfo.SetSubStep(32);
						sStepInfo.UpdataStTime();
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, i + 1);
						bRet &=OpenOutPort(atcName);
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(32);
						sStepInfo.UpdataStTime();
					}
				}
			}
			break;

		case 32:
			{
				//气缸是否到位
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
					if (ReadInPort(atcName) == 1)
					{
						sStepInfo.SetSubStep(33);
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, i + 1);
						if (ReadInPort(atcName) != 1)
						{
							bRet = FALSE;
						}
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(33006);
					}
				}
			}
			break;

		case 33:
			{
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(33001);
				}
			}
			break;

		case 33001:
			{
				if (ChangeTool(m_iSuckerIndex + 1) == 1)
				{
					sStepInfo.SetSubStep(33002);
				}
			}
			break;

		case 33002:
			{
				if (GotoDistance(_T("UAxis"), m_mapTKPosInfo[m_iPickPosIndex].asToolSet[m_iSuckerIndex].dUPos))
				{
					sStepInfo.SetSubStep(33003);
				}
			}
			break;
			
		case 33003:
			{
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(33004);
				}
				
			}
			break;

		case 33004:
			{
				if (ChangeTool(0) == 1)
				{
					sStepInfo.SetSubStep(33005);
				}
			}
			break;

		case 33005:
			{
				swprintf(atcName, SD_ROBOT_PICK_POS, m_iPickPosIndex + 1, m_iSuckerIndex + 1);
				if (StorePos(atcName))
				{
					sStepInfo.SetSubStep(34);
				}
			}
			break;

		case 33006:
			{
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(33007);
				}
			}
			break;

		case 33007:
			{
				if (ChangeTool(0) == 1)
				{
					sStepInfo.SetSubStep(34);
				}
			}
			break;

		case 34:
			{
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					
					bRet = TRUE;
					swprintf(atcName, SD_GT_O_SUCKERVAC_OPEN, m_iSuckerIndex + 1);
					bRet &= CloseOutPort(atcName);
					swprintf(atcName, SD_GT_O_SUCKERVAC_CLOSE, m_iSuckerIndex + 1);
					bRet &= OpenOutPort(atcName);

					if (bRet)
					{
						sStepInfo.SetSubStep(35);
						sStepInfo.UpdataStTime();
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_O_SUCKERVAC_OPEN, i + 1);
						bRet &= CloseOutPort(atcName);
						swprintf(atcName, SD_GT_O_SUCKERVAC_CLOSE, i + 1);
						bRet &= OpenOutPort(atcName);
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(35);
						sStepInfo.UpdataStTime();
					}
				}
			}
			break;

		case 35:
			{
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
					if (CloseOutPort(atcName))
					{
						sStepInfo.SetSubStep(36);
						sStepInfo.UpdataStTime();
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, i + 1);
						bRet &=CloseOutPort(atcName);
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(36);
						sStepInfo.UpdataStTime();
					}
				}
			}
			break;

		case 36:
			{
				//气缸是否到位
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_I_SUCKERCYL_UP, m_iSuckerIndex + 1);
					if (ReadInPort(atcName) == 1)
					{
						++m_iSuckerIndex;
						sStepInfo.SetSubStep(30);
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_I_SUCKERCYL_UP, i + 1);
						if (ReadInPort(atcName) != 1)
						{
							bRet = FALSE;
						}
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(40);
					}
				}
			}
			break;

		case 40:
			{
				m_iSuckerIndex = 0;
				++m_iPickPosIndex;
				sStepInfo.SetSubStep(41);
			}
			break;

		case 41:
			{
				if (m_iPickPosIndex < m_mapTKPosInfo.size())
				{
					sStepInfo.SetSubStep(3);
				}
				else
				{
					sStepInfo.SetMainSubStep(-1, 0);
				}
			}
			break;

		}
	}

	void CWorkFlow::F_CreateTool(StepInfo &sStepInfo)
	{
		switch (sStepInfo.GetSubStep())
		{

		case 0:
			{

				//移动到指定取板位置
				if (JumpPos(SD_ROBOT_WAIT_POS))
				{
					sStepInfo.SetSubStep(10000);
					m_dAngle = 0.0;
				}

			}
			break;

		case 10001:
			{
				//移动到指定取板位置
				TCHAR atcPosName[128] = {0};

				if (JumpPos(_T("取板位置")))
				{
					sStepInfo.SetSubStep(3);
				}
			}
			break;

		case 10000:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(10001);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 1:
			{
				//打开吸头真空
				if (sStepInfo.IsTimeOut(500) && CloseOutPort(_T("吸盘1关真空")) && OpenOutPort(_T("吸盘1开真空")))
				{
					if (GotoDistance(_T("ZAxis"), 5))
					{
						sStepInfo.SetSubStep(5);
					}

				}
			}
			break;

		case 2:
			{
				//吸头气缸下降
				if (OpenOutPort(_T("吸盘1下降")))
				{
					sStepInfo.SetSubStep(4);
				}
			}
			break;

		case 3:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(2);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 4:
			{
				//Z轴上升5mm
				if (ReadInPort(_T("吸盘1下降到位")) == 1)
				{

					{
						sStepInfo.SetSubStep(1);
						sStepInfo.UpdataStTime();
					}
				}

			}
			break;

		case 5:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(6);
				}
			}
			break;

		case 6:
			{
				//发送拍照信号
				if ((ReadInPort(_T("吸盘1有料")) == 1) && JumpPos(_T("Board标定位置")))
				{
					sStepInfo.SetSubStep(7);
				}
			}
			break;

		case 7:
			{
				//等待图像检测结果
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(8);
				}
			}
			break;

		case 8:
			{
				//生成TOOL 1
				if (m_pIVision->IsConnect() && m_pIVision->SendData((BYTE *)("PCB"), sizeof("PCB")))
				{
					sStepInfo.SetSubStep(9);
				}
			}
			break;

		case 9:
			{
				//移动到取料位置
				BYTE abyData[256] = {0};

				if (m_pIVision->RecvData(abyData, 256) > 0)
				{
					TCHAR atcName[128] = {0};
					TCHAR atcData[128] = {0};
					double dXVal = 0.0;
					double dYVal = 0.0;
					double dUVal = 0.0;
					swprintf(atcName, SD_ROBOT_CAMERA_CAL_VAL, m_iCalibPosIndex);
					tstring strValue = _T("");
					USES_CONVERSION;
					strValue = A2T((char *)abyData);
					int iIndex = 0;
					dXVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
					iIndex = strValue.find(',', iIndex) + 1;
					dYVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
					iIndex = strValue.find(',', iIndex) + 1;
					dUVal = _ttof(strValue.substr(iIndex).c_str());
					m_dAngle = dUVal;

					if (m_pIRobot->CreateTool(1, 1, dXVal, dYVal))
					{
						sStepInfo.SetSubStep(10);
					}
				}
			}
			break;

		case 10:
			{
				//检测移动完成
				if (JumpPos(_T("取板位置")))
				{
					sStepInfo.SetSubStep(11);
				}
			}
			break;

		case 11:
			{
				//关闭真空
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(14);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 12:
			{
				//气缸上升
				if (CloseOutPort(_T("吸盘1下降")))
				{
					sStepInfo.SetSubStep(13);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 13:
			{
				//检测气缸上升到位
				if ((ReadInPort(_T("吸盘1上升到位")) == 1))
				{

					sStepInfo.SetSubStep(15);					
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 14:
			{
				//切换到TOOL 1
				if (sStepInfo.IsTimeOut(500) && OpenOutPort(_T("吸盘1关真空")) && CloseOutPort(_T("吸盘1开真空")))
				{
					if (ChangeTool(1) == 1)
					{
						sStepInfo.SetSubStep(12);
					}
				}
			}
			break;

		case 15:
			{
				//保存当前坐标为校准后坐标
				GotoDistance(_T("UAxis"), -m_dAngle);
				IsRobotBusy(1000);
				if (StorePos(_T("取板位置")))
				{
					sStepInfo.SetSubStep(16);
				}


			}
			break;

		case 16:
			{
				//切换到 TOOL 0
				if (ChangeTool(0) == 1)
				{
					sStepInfo.SetSubStep(17);
				}
			}
			break;

		case 17:
			{
				//校准结束
				sStepInfo.SetMainSubStep(-1, 0);
			}
			break;
		}
		
	}

	void CWorkFlow::F_CalibPutPos(const int iModel)
	{
		StepInfo sStep;
		
		for (int i = 0; i < 3; ++i)
		{
			sStep.Init();
			while (sStep.GetMainStep() == 0)
			{
				F_CalibPutPos(iModel, i, sStep);

				if (GetIOStatus(_T("停止"), _T("InPort")) == 1)
				{
					MessageBox(m_hParaent, _T("检测到停止信号自动标定位置已停止"), _T("报警"), MB_OK);
					ChangeTool(0);
					return;
				}
			}	
		}


		if (m_pIRobot->CreatePallet(1, m_iTrayRow, m_iTrayCol) == 1)
		{
			int iCount = 0;
			if (iModel == 0)
			{
				iCount = m_iTrayRow * m_iTrayCol;
			}
			else
			{
				iCount  = m_iNGTrayRow * m_iNGTrayCol;
			}

			for (int i = 0; i < m_iTrayRow * m_iTrayCol; ++i)
			{
				TCHAR atcName[128] = {0};
				swprintf(atcName, (iModel == 0) ? SD_ROBOT_PASSTRAY_POS : SD_ROBOT_NGTRAY_POS, i + 1);
				RobotPosInfo sRobotPos;
				double dPassPosHigh = m_classIniConfig.ReadDouble(ROBOT, _T("PutOKHigh"), -20.0);
				double dFailPosHigh = m_classIniConfig.ReadDouble(ROBOT, _T("PutNgHigh"), -18.0);
				if (m_pIRobot->GetPalletPos(1, i + 1, sRobotPos) == 1)
				{
					if (iModel == 0)
					{
						if (dPassPosHigh > sRobotPos.dZPos)
						{
							sRobotPos.dZPos = dPassPosHigh;
						}
					}
					else
					{
						if (dFailPosHigh > sRobotPos.dZPos)
						{
							sRobotPos.dZPos = dFailPosHigh;
						}
					}

					m_mapProductPosInfo[atcName] = sRobotPos;
					CIniFile classIni;
					TCHAR atcPosData[128] = {0};
					classIni.SetIniFile((m_strProductDir + m_strProductName + _T("\\Position.dat")).c_str());
					swprintf(atcPosData, _T("%.3f,%.3f,%.3f,%.3f,"), sRobotPos.dXPos, sRobotPos.dYPos, sRobotPos.dZPos, sRobotPos.dUPos);
					classIni.WriteString(RPOSITION, atcName, atcPosData);

				}
			}
		}

	
	}

	void CWorkFlow::F_CalibPutPos(const int iModel, const int iPosIndex, StepInfo &sStepInfo)
	{
		switch (sStepInfo.GetSubStep())
		{

		case 0:
			{
			
					//移动到指定取板位置
					TCHAR atcPosName[128] = {0};
					if (iModel == 0)
					{
						if (JumpPos(SD_ROBOT_TRANSOK_POS))
						{
							sStepInfo.SetSubStep(10000);
							m_dAngle = 0.0;
						}
					}
					else
					{
						if (JumpPos(SD_ROBOT_TRANSNG_POS))
						{
							sStepInfo.SetSubStep(10000);
							m_dAngle = 0.0;
						}
					}
		
		
			}
			break;

		case 10001:
			{
				//移动到指定取板位置
				TCHAR atcPosName[128] = {0};
				swprintf(atcPosName, (iModel == 0) ? SD_ROBOT_CAMERA_PTRAY_POS : SD_ROBOT_CAMERA_NTRAY_POS, iPosIndex + 1);
				if (JumpPos(atcPosName))
				{
					sStepInfo.SetSubStep(3);
				}
			}
			break;

		case 10000:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(10001);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 1:
			{
				//打开吸头真空
				if (sStepInfo.IsTimeOut(500) && CloseOutPort(_T("吸盘1关真空")) && OpenOutPort(_T("吸盘1开真空")))
				{
					if (GotoDistance(_T("ZAxis"), 5))
					{
						sStepInfo.SetSubStep(5);
					}
					
				}
			}
			break;

		case 2:
			{
				//吸头气缸下降
				if (OpenOutPort(_T("吸盘1下降")))
				{
					sStepInfo.SetSubStep(4);
				}
			}
			break;

		case 3:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(2);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 4:
			{
				//Z轴上升5mm
				if (ReadInPort(_T("吸盘1下降到位")) == 1)
				{
					
					{
						sStepInfo.SetSubStep(1);
						sStepInfo.UpdataStTime();
					}
				}
				
			}
			break;

		case 5:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(6);
				}
			}
			break;

		case 6:
			{
				//发送拍照信号
				if ((ReadInPort(_T("吸盘1有料")) == 1))
				{
					sStepInfo.SetSubStep(6001);
				}
			}
			break;

		case 6001:
			{
				TCHAR atcPosName[128] = {0};
				if (iModel == 0)
				{
					if (JumpPos(SD_ROBOT_TRANSOK_POS))
					{
						sStepInfo.SetSubStep(6002);
						m_dAngle = 0.0;
					}
				}
				else
				{
					if (JumpPos(SD_ROBOT_TRANSNG_POS))
					{
						sStepInfo.SetSubStep(6002);
						m_dAngle = 0.0;
					}
				}
			}
			break;

		case 6002:
			{
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(6003);
				}
			}
			break;

		case 6003:
			{
				if (JumpPos(_T("1号吸头标定位置")))
				{
					sStepInfo.SetSubStep(7);
				}
			}
			break;


		case 7:
			{
				//等待图像检测结果
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(8);
				}
			}
			break;

		case 8:
			{
				//生成TOOL 1
				if (m_pIVision->IsConnect() && m_pIVision->SendData((BYTE *)("Model:PCB"), strlen("Model:PCB")))
				{
					sStepInfo.SetSubStep(9);
				}
			}
			break;

		case 9:
			{
				//移动到取料位置
				BYTE abyData[256] = {0};

				if (m_pIVision->RecvData(abyData, 256) > 0)
				{
					TCHAR atcName[128] = {0};
					TCHAR atcData[128] = {0};
					double dXVal = 0.0;
					double dYVal = 0.0;
					double dUVal = 0.0;
					swprintf(atcName, SD_ROBOT_CAMERA_CAL_VAL, m_iCalibPosIndex);
					tstring strValue = _T("");
					tstring strOKFlag = _T("");
					USES_CONVERSION;
					strValue = A2T((char *)abyData);
					strOKFlag = strValue.substr(0, 3);
					if (strOKFlag == _T("OK:"))
					{

						int iIndex = 3;
						dXVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
						iIndex = strValue.find(',', iIndex) + 1;
						dYVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
						iIndex = strValue.find(',', iIndex) + 1;
						dUVal = _ttof(strValue.substr(iIndex).c_str());
						m_dAngle = dUVal;
		
						if (m_pIRobot->CreateTool(1, 1 + iPosIndex, dXVal, dYVal))
						{
							sStepInfo.SetSubStep(9001);
						}
					}
				}
			}
			break;

		case 9001:
			{
				TCHAR atcPosName[128] = {0};
				if (iModel == 0)
				{
					if (JumpPos(SD_ROBOT_TRANSOK_POS))
					{
						sStepInfo.SetSubStep(9002);
						m_dAngle = 0.0;
					}
				}
				else
				{
					if (JumpPos(SD_ROBOT_TRANSNG_POS))
					{
						sStepInfo.SetSubStep(9002);
						m_dAngle = 0.0;
					}
				}
			}
			break;

		case 9002:
			{
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(10);
				}
			}
			break;

		case 10:
			{
				//检测移动完成
				TCHAR atcPosName[128] = {0};
				swprintf(atcPosName, (iModel == 0) ? SD_ROBOT_CAMERA_PTRAY_POS : SD_ROBOT_CAMERA_NTRAY_POS, iPosIndex + 1);
				if (JumpPos(atcPosName))
				{
					sStepInfo.SetSubStep(11);
				}
			}
			break;

		case 11:
			{
				//关闭真空
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(14);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 12:
			{
				//气缸上升
				if (CloseOutPort(_T("吸盘1下降")))
				{
					sStepInfo.SetSubStep(13);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 13:
			{
				//检测气缸上升到位
				if ((ReadInPort(_T("吸盘1上升到位")) == 1))
				{					
					sStepInfo.SetSubStep(15);					
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 14:
			{
				//切换到TOOL 1
				if (sStepInfo.IsTimeOut(500) && OpenOutPort(_T("吸盘1关真空")) && CloseOutPort(_T("吸盘1开真空")))
				{
					if (ChangeTool(1 + iPosIndex) == 1)
					{
						sStepInfo.SetSubStep(12);
					}
				}
			}
			break;

		case 15:
			{
				//保存当前坐标为校准后坐标
				TCHAR atcPosName[128] = {0};
				int iFlag = m_classIniConfig.ReadInt(_T("Config"), _T("AngleDir"), 0);
				if (iFlag)
				{
					GotoDistance(_T("UAxis"), m_dAngle);
				}
				else
				{
					GotoDistance(_T("UAxis"), -m_dAngle);
				}
				
				IsRobotBusy(1000);
				swprintf(atcPosName, (iModel == 0) ? SD_ROBOT_CAMERA_PTOOL_POS : SD_ROBOT_CAMERA_NTOOL_POS, iPosIndex + 1);
				if (StorePos(atcPosName) && m_pIRobot->SetPosition(iPosIndex + 1, m_mapProductPosInfo[atcPosName]))
				{
					sStepInfo.SetSubStep(16);
				}


			}
			break;

		case 16:
			{
				//切换到 TOOL 0
				if (ChangeTool(0) == 1)
				{
					sStepInfo.SetSubStep(17);
				}
			}
			break;

		case 17:
			{
				//校准结束
				sStepInfo.SetMainSubStep(-1, 0);
			}
			break;
		}
	}

	void CWorkFlow::F_CalibPos(LPCTSTR lpstrNameFormat, const int iPosIndex, StepInfo &sStepInfo)
	{
		switch (sStepInfo.GetSubStep())
		{

		case 0:
			{

				//移动到指定取板位置
				TCHAR atcPosName[128] = {0};
				swprintf(atcPosName, lpstrNameFormat, iPosIndex);
				if (JumpPos(_T("过渡位")))
				{
					sStepInfo.SetSubStep(10000);
					m_dAngle = 0.0;
				}

			}
			break;

		case 10001:
			{
				//移动到指定取板位置
				TCHAR atcPosName[128] = {0};
				swprintf(atcPosName, lpstrNameFormat, iPosIndex);
				if (JumpPos(atcPosName))
				{
					sStepInfo.SetSubStep(3);
				}
			}
			break;

		case 10000:
			{
				//检测移动完成
				if (!IsRobotBusy(10) && (ChangeTool(1) == 1))
				{
					sStepInfo.SetSubStep(10001);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 1:
			{
				//打开吸头真空
				if (sStepInfo.IsTimeOut(500) && CloseOutPort(_T("吸盘1关真空")) && OpenOutPort(_T("吸盘1开真空")))
				{
					if (GotoDistance(_T("ZAxis"), 5))
					{
						sStepInfo.SetSubStep(5);
					}

				}
			}
			break;

		case 2:
			{
				//吸头气缸下降
				if (OpenOutPort(_T("吸盘1下降")))
				{
					sStepInfo.SetSubStep(4);
				}
			}
			break;

		case 3:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(2);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 4:
			{
				//Z轴上升5mm
				if (ReadInPort(_T("吸盘1下降到位")) == 1)
				{

					{
						sStepInfo.SetSubStep(1);
						sStepInfo.UpdataStTime();
					}
				}

			}
			break;

		case 5:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(6);
				}
			}
			break;

		case 6:
			{
				//发送拍照信号
				if ((ReadInPort(_T("吸盘1有料")) == 1) && (ChangeTool(0) == 1) && JumpPos(_T("Board标定位置")))
				{
					sStepInfo.SetSubStep(7);
				}
			}
			break;

		case 7:
			{
				//等待图像检测结果
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(8);
				}
			}
			break;

		case 8:
			{
				//生成TOOL 1
				if (m_pIVision->IsConnect() && m_pIVision->SendData((BYTE *)("PCB"), sizeof("PCB")))
				{
					sStepInfo.SetSubStep(9);
				}
			}
			break;

		case 9:
			{
				//移动到取料位置
				BYTE abyData[256] = {0};

				if (m_pIVision->RecvData(abyData, 256) > 0)
				{
					TCHAR atcName[128] = {0};
					TCHAR atcData[128] = {0};
					double dXVal = 0.0;
					double dYVal = 0.0;
					double dUVal = 0.0;
					swprintf(atcName, SD_ROBOT_CAMERA_CAL_VAL, m_iCalibPosIndex);
					tstring strValue = _T("");
					USES_CONVERSION;
					strValue = A2T((char *)abyData);
					int iIndex = 0;
					dXVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
					iIndex = strValue.find(',', iIndex) + 1;
					dYVal = _ttof(strValue.substr(iIndex, strValue.find(',') - iIndex).c_str());
					iIndex = strValue.find(',', iIndex) + 1;
					dUVal = _ttof(strValue.substr(iIndex).c_str());
					m_dAngle = dUVal;

					if (m_pIRobot->CreateTool(1, 15, dXVal, dYVal))
					{
						sStepInfo.SetSubStep(10);
					}
				}
			}
			break;

		case 10:
			{
				//检测移动完成
				TCHAR atcPosName[128] = {0};
				swprintf(atcPosName, lpstrNameFormat, iPosIndex);
				if ((ChangeTool(1) == 1) &&JumpPos(atcPosName))
				{
					sStepInfo.SetSubStep(11);
				}
			}
			break;

		case 11:
			{
				//关闭真空
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(14);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 12:
			{
				//气缸上升
				if (CloseOutPort(_T("吸盘1下降")))
				{
					sStepInfo.SetSubStep(13);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 13:
			{
				//检测气缸上升到位
				if ((ReadInPort(_T("吸盘1上升到位")) == 1))
				{

					sStepInfo.SetSubStep(15);					
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 14:
			{
				//切换到TOOL 1
				if (sStepInfo.IsTimeOut(500) && OpenOutPort(_T("吸盘1关真空")) && CloseOutPort(_T("吸盘1开真空")))
				{
					if (ChangeTool(15) == 1)
					{
						sStepInfo.SetSubStep(12);
					}
				}
			}
			break;

		case 15:
			{
				//保存当前坐标为校准后坐标
				TCHAR atcPosName[128] = {0};
				swprintf(atcPosName, lpstrNameFormat, iPosIndex);
				GotoDistance(_T("UAxis"), -m_dAngle);
				IsRobotBusy(1000);
				if (StorePos(atcPosName))
				{
					sStepInfo.SetSubStep(16);
				}


			}
			break;

		case 16:
			{
				//切换到 TOOL 0
				if (ChangeTool(0) == 1)
				{
					sStepInfo.SetSubStep(17);
				}
			}
			break;

		case 17:
			{
				//校准结束
				sStepInfo.SetMainSubStep(-1, 0);
			}
			break;
		}
	}

	void CWorkFlow::F_PickAndPut(StepInfo &sStepInfo)
	{
		TCHAR atcName[128] = {0};
		BOOL bRet = TRUE;
		switch (sStepInfo.GetSubStep())
		{
		case 0:
			{
				//移动到等待位置
				if (JumpPos(SD_ROBOT_WAIT_POS))
				{
					sStepInfo.SetSubStep(1);
					sStepInfo.UpdataStTime();
				}
				else
				{
					AlarmSetStep(sStepInfo, 1, sStepInfo.GetSubStep());
				}
			}
			break;

		case 1:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(2);
				}
				else
				{
					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, 2, sStepInfo.GetSubStep(), 0);
					}
				}
			}
			break;

		case 2:
			{
				//等待允许抓取信号
				if (m_bNULLRun || m_bAllPick)
				{
					sStepInfo.SetSubStep(3);

					for (int i = 0; i < 50; ++i)
					{
						m_aiTestResult[i] = m_aiResultTemp[i];
					}
				}
			}
			break;

		case 3:
			{
				//开启所有真空
				m_iSuckerIndex = 0;
				bRet = TRUE;
// 				for (int i = 0; i < 4; ++i)
// 				{
// 					swprintf(atcName, SD_GT_O_SUCKERVAC_OPEN, i + 1);
// 					bRet &= OpenOutPort(atcName);
// 					swprintf(atcName, SD_GT_O_SUCKERVAC_CLOSE, i + 1);
// 					bRet &= CloseOutPort(atcName);
// 				}

				if (bRet)
				{
					sStepInfo.SetSubStep(4);
				}
				else
				{
					AlarmSetStep(sStepInfo, 3, sStepInfo.GetSubStep(), 4);
				}
			}
			break;

		case 4:
			{
				
				if ((m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0))
				{
					if (m_iSuckerIndex < m_mapTKPosInfo[m_iPickPosIndex].iTkCount)
					{
						//移动到第m_iSuckerIndex位置取板
						swprintf(atcName, SD_GT_O_SUCKERVAC_OPEN, m_iSuckerIndex + 1);
						bRet &= OpenOutPort(atcName);
						swprintf(atcName, SD_GT_O_SUCKERVAC_CLOSE, m_iSuckerIndex + 1);
						bRet &= CloseOutPort(atcName);

						swprintf(atcName, SD_ROBOT_PICK_POS, m_iPickPosIndex + 1, m_iSuckerIndex + 1);
						if (m_iSuckerIndex == 0)
						{
							if (bRet && JumpPos(atcName))
							{
								sStepInfo.SetSubStep(7);
								sStepInfo.UpdataStTime();
							}
							else
							{
								AlarmSetStep(sStepInfo, 1, sStepInfo.GetSubStep());
							}
						}
						else
						{
							if (bRet && GotoPos(atcName))
							{
								sStepInfo.SetSubStep(7);
								sStepInfo.UpdataStTime();
							}
							else
							{
								AlarmSetStep(sStepInfo, 1, sStepInfo.GetSubStep());
							}
						}
					}
					else
					{
						sStepInfo.SetSubStep(12);
					}
				}
				else
				{
					//移动到第0位置取板
					bRet = TRUE;
					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_O_SUCKERVAC_OPEN, i + 1);
						bRet &= OpenOutPort(atcName);
						swprintf(atcName, SD_GT_O_SUCKERVAC_CLOSE, i + 1);
						bRet &= CloseOutPort(atcName);
					}

					swprintf(atcName, SD_ROBOT_PICK_POS, m_iPickPosIndex + 1, 1);
					if (bRet && JumpPos(atcName))
					{
						sStepInfo.SetSubStep(7);
						sStepInfo.UpdataStTime();
					}
					else
					{
						AlarmSetStep(sStepInfo, 1, sStepInfo.GetSubStep());
					}
				}
			}
			break;

		case 5:
			{
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
					if (OpenOutPort(atcName))
					{
						sStepInfo.SetSubStep(6);
						sStepInfo.UpdataStTime();
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, i + 1);
						bRet &=OpenOutPort(atcName);
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(6);
						sStepInfo.UpdataStTime();
					}
				}

				if (!bRet)
				{
					AlarmSetStep(sStepInfo, 4, sStepInfo.GetSubStep(), 6);
				}
			}
			break;

		case 6:
			{
				//气缸是否到位
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
					if (ReadInPort(atcName) == 1)
					{
						sStepInfo.UpdataStTime();
						sStepInfo.SetSubStep(8);
					}
					else
					{
						bRet = FALSE;
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, i + 1);
						if (ReadInPort(atcName) != 1)
						{
							bRet = FALSE;
						}
					}

					if (bRet)
					{
						sStepInfo.UpdataStTime();
						sStepInfo.SetSubStep(8);
					}
				}

				if (!bRet)
				{
					if (sStepInfo.IsTimeOut(2000))
					{
						AlarmSetStep(sStepInfo, 5, sStepInfo.GetSubStep(), 8);
					}		
				}
			}
			break;

		case 7:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(5);
					sStepInfo.UpdataStTime();
				}
				else
				{
					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, 1, sStepInfo.GetSubStep());
					}
				}
			}
			break;

		case 8:
			{
				if (sStepInfo.IsTimeOut(100))
				{
					sStepInfo.SetSubStep(9);
				}
			}
			break;

		case 9:
			{
				//气缸上升
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
					if (CloseOutPort(atcName))
					{
						sStepInfo.SetSubStep(10);
						sStepInfo.UpdataStTime();
					}
					else
					{
						bRet = FALSE;
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, i + 1);
						bRet &=CloseOutPort(atcName);
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(10);
						sStepInfo.UpdataStTime();
					}
				}

				if (!bRet)
				{
					AlarmSetStep(sStepInfo, 7, sStepInfo.GetSubStep(), 10);
				}
			}
			break;

		case 10:
			{
				//检测气缸上升到位
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					//下降指定气缸
					//打开真空
					swprintf(atcName, SD_GT_I_SUCKERCYL_UP, m_iSuckerIndex + 1);
					if (ReadInPort(atcName) == 1)
					{
						sStepInfo.SetSubStep(11);
					}
					else
					{
						bRet = FALSE;									
					}
				}
				else
				{
					//所有气缸全部下降
					//打开真空	
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_I_SUCKERCYL_UP, i + 1);
						if (ReadInPort(atcName) != 1)
						{
							bRet = FALSE;
						}
					}

					if (bRet)
					{
						sStepInfo.SetSubStep(11);
					}
				}

				if (!bRet)
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, 6, sStepInfo.GetSubStep(), 11);
					}						
				}

			}
			break;

		case 11:
			{
				//检测真空
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					swprintf(atcName, SD_GT_I_SUCKERVAC, m_iSuckerIndex + 1);
					if (m_bNULLRun || ReadInPort(atcName) == 1)
					{
						SendMsg(WM_ULMSG_PICKRSTPOS, m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1, NULL);
						++m_iSuckerIndex;
						sStepInfo.SetSubStep(4);
					}
					else
					{
						AlarmSetStep(sStepInfo, 8, sStepInfo.GetSubStep(), 4);

						if (sStepInfo.GetSubStep() == 4)
						{
							SendMsg(WM_ULMSG_PICKRSTPOS, m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1, NULL);
							++m_iSuckerIndex;
						}
					}
				}
				else
				{
					bRet = TRUE;

					for (int i = 0; i < 4; ++i)
					{
						swprintf(atcName, SD_GT_I_SUCKERVAC, i + 1);
						if (m_bNULLRun || ReadInPort(atcName) != 1)
						{
							bRet = FALSE;
						}

						Sleep(100);
					}

					if (bRet)
					{
						SendMsg(WM_ULMSG_PICKRSTPOS, m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[0] - 1, NULL);
						SendMsg(WM_ULMSG_PICKRSTPOS, m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[1] - 1, NULL);
						SendMsg(WM_ULMSG_PICKRSTPOS, m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[2] - 1, NULL);
						SendMsg(WM_ULMSG_PICKRSTPOS, m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[3] - 1, NULL);
						sStepInfo.SetSubStep(12);
					}
					else
					{
						AlarmSetStep(sStepInfo, 8, sStepInfo.GetSubStep(), 12);

						if (sStepInfo.GetSubStep() == 12)
						{
							SendMsg(WM_ULMSG_PICKRSTPOS, m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[0] - 1, NULL);
							SendMsg(WM_ULMSG_PICKRSTPOS, m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[1] - 1, NULL);
							SendMsg(WM_ULMSG_PICKRSTPOS, m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[2] - 1, NULL);
							SendMsg(WM_ULMSG_PICKRSTPOS, m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[3] - 1, NULL);
						}
					}
				}
				
			}
			break;

		case 12:
			{
				m_iSuckerIndex = 0;
				if (m_iPickPosIndex == (m_mapTKPosInfo.size() - 1))
				{
					//发送取料完成信号
					m_bAllPick = FALSE;				
				}

				sStepInfo.SetSubStep(13);
			}
			break;

		case 13:
			{
				//移动到等待位置
				if (JumpPos(SD_ROBOT_TRANSOK_POS))
				{
					sStepInfo.SetSubStep(15);
					sStepInfo.UpdataStTime();
				}
				else
				{
					AlarmSetStep(sStepInfo, 1, sStepInfo.GetSubStep());
				}
			}
			break;

		case 14:
			{
				//发送Tool 1 - 4 创建吸头Tool
				bRet = TRUE;
				for (int i = 0; i < 4; ++i)
				{
					double dX = 0.0;
					double dY = 0.0;
					double dU = 0.0;
					dX = m_mapTKPosInfo[m_iPickPosIndex].asToolSet[i].dXPos;
					dY = m_mapTKPosInfo[m_iPickPosIndex].asToolSet[i].dYPos;
					dU = m_mapTKPosInfo[m_iPickPosIndex].asToolSet[i].dUPos;
					if (m_pIRobot->SetTool(i + 1, dX, dY) != 1)
					{
						bRet = FALSE;
					}
				}

				if (bRet)
				{
					sStepInfo.SetSubStep(16);
				}
				else
				{
					AlarmSetStep(sStepInfo, 9, sStepInfo.GetSubStep());
				}
			}
			break;

		case 15:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(14);
				}
				else
				{
					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, 2, sStepInfo.GetSubStep(), 13);
					}
				}
			}
			break;

		case 16:
			{
				//来判断是否应该换吹盘了检测取起来的板是否是OK

				if (m_bAllowPutPass && (m_iPutPassPos < m_iTrayRow * m_iTrayCol))
				{
					if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
					{
						if (m_iSuckerIndex < m_mapTKPosInfo[m_iPickPosIndex].iTkCount)
						{
							if (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] == 1)
							{
								sStepInfo.SetSubStep(17);
							}
							else
							{
								++m_iSuckerIndex;
							}
						}
						else
						{
							sStepInfo.SetSubStep(24);
						}
					}
					else
					{
						if (m_iSuckerIndex < 4)
						{
							if (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] == 1)
							{
								sStepInfo.SetSubStep(17);
							}
							else
							{
								++m_iSuckerIndex;
							}
						}
						else
						{
							sStepInfo.SetSubStep(24);
						}
					}
				}
				else
				{
// 					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
// 					{
// 						//发送消息说等待
// // 						m_bAllowPutPass = TRUE;
// // 						m_iPutPassPos = 0;
// // 						SendMsg(WM_ULMSG_RESETPCB, 1, 0);
// 						//AlarmSetStep(sStepInfo, 10, sStepInfo.GetSubStep());
// 					}

					if (m_iSuckerIndex < 4)
					{
// 						RobotPosInfo sRobot;
// 						if (m_pIRobot->QryCurPos(sRobot) == 1)
// 						{
// 							if ((fabs(sRobot.dZPos) > 0.5))
// 							{
// 								sRobot.dZPos = 0.0;
// 								if (m_pIRobot->GotoPosMod(sRobot, ROBOT_SAFEHIGH))
// 								{
									sStepInfo.SetSubStep(16001);
// 								}
// 							}
// 							
// 						}
// 
// 						if (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] != 1)
// 						{
// 							++m_iSuckerIndex;
// 						}
					}
					else
					{
						sStepInfo.SetSubStep(24);
					}
				}
		
			}
			break;

		case 16001:
			{
				if ((ChangeTool(0) == 1))
				{
					if (JumpPos(SD_ROBOT_TRANSOK_POS))
					{
						sStepInfo.SetSubStep(16002);
						sStepInfo.UpdataStTime();
					}
					else
					{
						AlarmSetStep(sStepInfo, 16001, sStepInfo.GetSubStep());
					}
				}
			}
			break;

		case 16002:
			{
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(16003);
				}
				else
				{
					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, 2, sStepInfo.GetSubStep(), 16001);
					}
				}
			}
			break;

		case 16003:
			{
				if (m_iSuckerIndex < 4)
				{
					if (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] != 1)
					{
						++m_iSuckerIndex;
					}
					else
					{
						if (m_bAllowPutPass)
						{
							sStepInfo.SetSubStep(16);
						}
					}
				}
				else
				{
					sStepInfo.SetSubStep(24);
				}
			}
			break;

		case 17:
			{
				//切换Tool m_iSuckerIndex
				if (ChangeTool(m_iSuckerIndex + 1)== 1)
				{
					sStepInfo.SetSubStep(18);
				}
				else
				{
					AlarmSetStep(sStepInfo, 11, sStepInfo.GetSubStep());
				}
			}
			break;

		case 18:
			{
				//移动到吸头m_iPutPassPos位置
				swprintf(atcName, SD_ROBOT_PASSTRAY_POS, m_iPutPassPos + 1);
				if (m_iSuckerIndex == 0)
				{
					if (JumpPos(atcName))
					{
						sStepInfo.SetSubStep(21);
						sStepInfo.UpdataStTime();
					}
					else
					{
						AlarmSetStep(sStepInfo, 1, sStepInfo.GetSubStep());
					}
				}
				else
				{
					if (GotoPos(atcName))
					{
						sStepInfo.SetSubStep(21);
						sStepInfo.UpdataStTime();
					}
					else
					{
						AlarmSetStep(sStepInfo, 1, sStepInfo.GetSubStep());
					}
				}
			}
			break;

		case 19:
			{
				//气缸伸出
				swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
//				swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, 1);
				if (OpenOutPort(atcName))
				{
					sStepInfo.SetSubStep(20);
					sStepInfo.UpdataStTime();
				}
				else
				{
					AlarmSetStep(sStepInfo, 4, sStepInfo.GetSubStep(), 20);
				}
			}
			break;

		case 20:
			{
				//检测气缸伸出到位
				swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
//				swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, 1);
				if (ReadInPort(atcName) == 1)
				{
					sStepInfo.SetSubStep(22);
				}
				else
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, ALARM_SUCKERCYLDOWN, sStepInfo.GetSubStep(), 21);
					}					
				}
			}
			break;

		case 21:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(19);
				}
				else
				{
					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, ALARM_ROBOTWAIT, sStepInfo.GetSubStep(), 22);
					}					
				}
			}
			break;

		case 22:
			{
				//关闭吸头真空
				bRet = TRUE;
				swprintf(atcName, SD_GT_O_SUCKERVAC_OPEN, m_iSuckerIndex + 1);				
				bRet &= CloseOutPort(atcName);
				swprintf(atcName, SD_GT_O_SUCKERVAC_CLOSE, m_iSuckerIndex + 1);
				bRet &= OpenOutPort(atcName);
				if (bRet)
				{
					sStepInfo.SetSubStep(23);
					sStepInfo.UpdataStTime();
				}
				else
				{
					AlarmSetStep(sStepInfo, ALARM_SUCKERVACCTRL, sStepInfo.GetSubStep(), 23);
				}
			}
			break;

		case 23:
			{
				swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
//				swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, 1);
				if (sStepInfo.IsTimeOut(50) )
				{
					//气缸收回
					//切换到 tool 0
					if ((ChangeTool(0) == 1))
					{
						if (CloseOutPort(atcName))
						{
							
							sStepInfo.SetSubStep(23001);
							sStepInfo.UpdataStTime();
						}
						else
						{
							AlarmSetStep(sStepInfo, ALARM_SUCKERCYLCLOSE, sStepInfo.GetSubStep(), 23001);
						}
					}
					else
					{
						AlarmSetStep(sStepInfo, ALARM_TOOLSET, sStepInfo.GetSubStep());
					}
					
				}
			}
			break;

		case 23001:
			{
				swprintf(atcName, SD_GT_I_SUCKERCYL_UP, m_iSuckerIndex + 1);
//				swprintf(atcName, SD_GT_I_SUCKERCYL_UP, 1);
				if (ReadInPort(atcName) == 1)
				{
					AddPsCnt();
					++m_iPutPassPos;
					++m_iSuckerIndex;
					sStepInfo.SetSubStep(16);
				}
				else
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, ALARM_SUCKERCYLUP, sStepInfo.GetSubStep(), 16);
					}
					
				}
			}
			break;

		case 24:
			{
				//移动到等待位置
				if ((ChangeTool(0) == 1))
				{
					if (JumpPos(SD_ROBOT_WAIT_POS))
					{
						sStepInfo.SetSubStep(25);
						sStepInfo.UpdataStTime();;
					}
					else
					{
						AlarmSetStep(sStepInfo, ALARM_ROBOTMOVE, sStepInfo.GetSubStep());
					}
				}
				else
				{
					AlarmSetStep(sStepInfo, ALARM_TOOLSET, sStepInfo.GetSubStep());
				}
			}
			break;

		case 25:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					m_iSuckerIndex = 0;
					sStepInfo.SetSubStep(26);
				}
				else
				{
					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, ALARM_ROBOTWAIT, sStepInfo.GetSubStep(), 24);
					}	
				}
				
			}
			break;

		case 26:
			{
				//来判断是否应该换吹盘了检测取起来的板是否是X板
				if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
				{
					if (m_iSuckerIndex < m_mapTKPosInfo[m_iPickPosIndex].iTkCount)
					{
						if (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] == 2)
						{
							sStepInfo.SetSubStep(27);
						}
						else
						{
							++m_iSuckerIndex;
						}
					}
					else
					{
						sStepInfo.SetSubStep(31);
					}
				}
				else
				{
					if (m_iSuckerIndex < 4)
					{
						if (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] == 2)
						{
							sStepInfo.SetSubStep(27);
						}
						else
						{
							++m_iSuckerIndex;
						}
					}
					else
					{
						sStepInfo.SetSubStep(31);
					}
				}				
			}
			break;

		case 27:
			{
				//切换Tool m_iSuckerIndex
				swprintf(atcName, SD_ROBOT_XBOARD_POS, m_iSuckerIndex + 1);
				if (JumpPos(atcName))
				{
					sStepInfo.SetSubStep(28);
					sStepInfo.UpdataStTime();
				}
				else
				{
					AlarmSetStep(sStepInfo, ALARM_ROBOTMOVE, sStepInfo.GetSubStep());
				}
			}
			break;

		case 28:
			{
				//气缸伸出
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(29);
				}
				else
				{
					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, ALARM_ROBOTWAIT, sStepInfo.GetSubStep(), 27);
					}					
				}
			}
			break;

		case 29:
			{
				//移动到吸头m_iPutFailPos位置
				bRet = TRUE;
				swprintf(atcName, SD_GT_O_SUCKERVAC_OPEN, m_iSuckerIndex + 1);
				bRet &= CloseOutPort(atcName);
				swprintf(atcName, SD_GT_O_SUCKERVAC_CLOSE, m_iSuckerIndex + 1);
				bRet &= OpenOutPort(atcName);
				if (bRet)
				{
					sStepInfo.SetSubStep(30);
					sStepInfo.UpdataStTime();
				}
				else
				{
					AlarmSetStep(sStepInfo, ALARM_SUCKERVACCTRL, sStepInfo.GetSubStep(), 30);
				}
			}
			break;

		case 30:
			{
				if (sStepInfo.IsTimeOut(50))
				{
					++m_iSuckerIndex;
					sStepInfo.SetSubStep(26);
				}
			}
			break;

		case 31:
			{
// 				if (((m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[0] - 1] != 1) && (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[0] - 1] != 2))
// 					|| ((m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[1] - 1] != 1) && (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[1] - 1] != 2))
// 					|| ((m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[2] - 1] != 1) && (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[2] - 1] != 2))
// 					|| ((m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[3] - 1] != 1) && (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[3] - 1] != 2))
// 					)
// 				{
// 					m_iSuckerIndex = 0;
// 					sStepInfo.SetSubStep(32);
// 				}
// 				else
// 				{
// 					sStepInfo.SetSubStep(60);
// 				}
				BOOL bRet = FALSE;
				for (int i = 0; i < m_mapTKPosInfo[m_iPickPosIndex].iTkCount; ++i)
				{
					if (((m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[i] - 1] != 1) && (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[i] - 1] != 2)))
					{
						bRet = TRUE;
						break;
					}
				}

				if (bRet)
				{
					m_iSuckerIndex = 0;
					sStepInfo.SetSubStep(32);
				}
				else
				{
					sStepInfo.SetSubStep(60);
				}
			}
			break;

		case 32:
			{
				if (JumpPos(SD_ROBOT_TRANSNG_POS))
				{
					sStepInfo.SetSubStep(33);
					sStepInfo.UpdataStTime();
				}
				else
				{
					AlarmSetStep(sStepInfo, ALARM_ROBOTMOVE, sStepInfo.GetSubStep());
				}
			}
			break;

		case 33:
			{
				if (!IsRobotBusy(10))
				{
					m_iSuckerIndex = 0;
					sStepInfo.SetSubStep(40);
				}
				else
				{
					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, ALARM_ROBOTWAIT, sStepInfo.GetSubStep(), 32);
					}					
				}
			}
			break;

		case 40:
			{
				//来判断是否应该换吹盘了检测取起来的板是否是NG板
				if (m_bAllowPutFail && (m_iPutFailPos < m_iNGTrayRow * m_iNGTrayCol))
				{
					if (m_mapTKPosInfo[m_iPickPosIndex].iTakeMode != 0)
					{
						if (m_iSuckerIndex < m_mapTKPosInfo[m_iPickPosIndex].iTkCount)
						{
							if (((m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] != 1) && (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] != 2)))
							{
								sStepInfo.SetSubStep(41);
							}
							else
							{
								++m_iSuckerIndex;
							}
						}
						else
						{
							sStepInfo.SetSubStep(50);
						}
					}
					else
					{
						if (m_iSuckerIndex < m_mapTKPosInfo[m_iPickPosIndex].iTkCount)
						{
							if (((m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] != 1) && (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] != 2)))
							{
								sStepInfo.SetSubStep(41);
							}
							else
							{
								++m_iSuckerIndex;
							}
						}
						else
						{
							sStepInfo.SetSubStep(50);
						}
					}
				}
				else
				{
					if (m_iSuckerIndex < 4)
					{
						if (((m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] != 1) && (m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] != 2)))
						{
							if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT * 2))
							{
								//发送消息说等待
								AlarmSetStep(sStepInfo, ALARM_WAITPUTFAIL, sStepInfo.GetSubStep());
							}
						}
						else
						{
							++m_iSuckerIndex;
						}
					}
					else
					{
						sStepInfo.SetSubStep(50);
					}
				}
			}
			break;

		case 41:
			{
				//切换Tool m_iSuckerIndex

				if ((ChangeTool(m_iSuckerIndex + 1) == 1))
				{
					swprintf(atcName, SD_ROBOT_NGTRAY_POS, m_iPutFailPos + 1);
					if (JumpPos(atcName))
					{
						sStepInfo.SetSubStep(44);
						sStepInfo.UpdataStTime();;
					}
					else
					{
						AlarmSetStep(sStepInfo, ALARM_ROBOTMOVE, sStepInfo.GetSubStep());
					}
				}
				else
				{
					AlarmSetStep(sStepInfo, ALARM_CHANGETOOL, sStepInfo.GetSubStep());
				}
			}
			break;

		case 42:
			{
				//移动到吸头m_iPutFailPos位置
				//气缸伸出
				swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
//				swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, 1);
				if (OpenOutPort(atcName))
				{
					sStepInfo.SetSubStep(43);
					sStepInfo.UpdataStTime();
				}
				else
				{
					AlarmSetStep(sStepInfo, ALARM_SUCKERCYLOPEN, sStepInfo.GetSubStep(), 43);
				}
			}
			break;

		case 43:
			{
				//检测气缸伸出到位
				swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
//				swprintf(atcName, SD_GT_I_SUCKERCYL_DOWN, 1);
				if (ReadInPort(atcName) == 1)
				{
					sStepInfo.SetSubStep(45);
				}
				else
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, ALARM_SUCKERCYLDOWN, sStepInfo.GetSubStep(), 44);
					}
				}
			}
			break;

		case 44:
			{
				//检测移动完成
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(42);
				}
				else
				{
					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, ALARM_ROBOTWAIT, sStepInfo.GetSubStep(), 41);
					}					
				}
			}
			break;

		case 45:
			{
				
				bRet = TRUE;
				swprintf(atcName, SD_GT_O_SUCKERVAC_OPEN, m_iSuckerIndex + 1);
				bRet &= CloseOutPort(atcName);
				swprintf(atcName, SD_GT_O_SUCKERVAC_CLOSE, m_iSuckerIndex + 1);
				bRet &= OpenOutPort(atcName);
				if (bRet)
				{
					sStepInfo.SetSubStep(46);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 46:
			{
				//关闭吸头真空
				if (sStepInfo.IsTimeOut(50))
				{
					sStepInfo.SetSubStep(47);
				}
			}
			break;

		case 47:
			{
				swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, m_iSuckerIndex + 1);
//				swprintf(atcName, SD_GT_O_SUCKERCYL_DOWN, 1);
				if (CloseOutPort(atcName))
				{
					sStepInfo.SetSubStep(48);
					sStepInfo.UpdataStTime();
				}
				else
				{
					AlarmSetStep(sStepInfo, ALARM_SUCKERCYLCLOSE, sStepInfo.GetSubStep(), 48);
				}
			}
			break;

		case 48:
			{
				swprintf(atcName, SD_GT_I_SUCKERCYL_UP, m_iSuckerIndex + 1);
//				swprintf(atcName, SD_GT_I_SUCKERCYL_UP,  1);
				if (ReadInPort(atcName) == 1)
				{
					sStepInfo.SetSubStep(49);
					sStepInfo.UpdataStTime();
				}
				else
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, ALARM_SUCKERCYLUP, sStepInfo.GetSubStep(), 49);
					}
				}
			}
			break;

		case 49:
			{
				if (sStepInfo.IsTimeOut(50))
				{
					if ((ChangeTool(0) == 1))
					{
						if ((m_aiTestResult[m_mapTKPosInfo[m_iPickPosIndex].aiSuckerToPos[m_iSuckerIndex] - 1] != 0))
						{							
							AddOtherCnt();
						}
						else
						{
							AddFiCnt();
						}
						
						++m_iPutFailPos;
						++m_iSuckerIndex;
						sStepInfo.SetSubStep(40);
					}
					else
					{
						AlarmSetStep(sStepInfo, ALARM_CHANGETOOL, sStepInfo.GetSubStep());
					}
					
				}
			}
			break;

		case 50:
			{
				if (ChangeTool(0) == 1)
				{
					sStepInfo.SetSubStep(51);
				}
				else
				{
					AlarmSetStep(sStepInfo, ALARM_CHANGETOOL, sStepInfo.GetSubStep());
				}
			}
			break;

		case 51:
			{
				if (JumpPos(SD_ROBOT_TRANSNG_POS))
				{
					sStepInfo.SetSubStep(53);
					sStepInfo.UpdataStTime();
				}
				else
				{
					AlarmSetStep(sStepInfo, ALARM_ROBOTMOVE, sStepInfo.GetSubStep());
				}
			}
			break;

		case 53:
			{
				if (!IsRobotBusy(10))
				{
					sStepInfo.SetSubStep(60);
				}
				else
				{
					if (sStepInfo.IsTimeOut(ROBOT_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, ALARM_ROBOTWAIT, sStepInfo.GetSubStep(), 51);
					}					
				}
			}
			break;

		case 60:
			{
				m_iSuckerIndex = 0;
				++m_iPickPosIndex;

				if (m_iPickPosIndex >= m_mapTKPosInfo.size())
				{
					m_iPickPosIndex = 0;
				}

				sStepInfo.SetSubStep(0);
			}
			break;
		}

		m_classIniConfig.WriteInt(_T("StepInfo"), _T("PickAndPut"), sStepInfo.GetSubStep());
		m_classIniConfig.WriteInt(_T("StepInfo"), _T("PutFailPos"), m_iPutFailPos);
		m_classIniConfig.WriteInt(_T("StepInfo"), _T("SuckerIndex"), m_iSuckerIndex);
		m_classIniConfig.WriteInt(_T("StepInfo"), _T("PickPosIndex"), m_iPickPosIndex);
		m_classIniConfig.WriteInt(_T("StepInfo"), _T("PutPassPos"), m_iPutPassPos);
	}

	void CWorkFlow::F_FailTray(StepInfo &sStepInfo)
	{
		int iRet = 0;

		switch (sStepInfo.GetSubStep())
		{
		case 0:
			{
				//检测是否有Tray信号
				iRet = ReadInPort(_T("NG位有无料"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(1);
				}
				else
				{
					m_bAllowPutFail = FALSE;
					sStepInfo.SetSubStep(4);
				}
			}
			break;

		case 1:
			{
				if (m_iPutFailPos < (m_iNGTrayRow * m_iNGTrayCol))
				{
					//检测Tray到位信号
					if (!m_bAllowPutFail && OpenOutPort(_T("NG位伸出")) && CloseOutPort(_T("NG位缩回")))
					{
						sStepInfo.SetSubStep(2);
					}
					else
					{
						if (ReadInPort(_T("NG出料")) == 1)
						{
							m_bAllowPutFail = FALSE;
							sStepInfo.SetSubStep(4);
						}
					}
				}
				else
				{
					m_bAllowPutFail = FALSE;
					sStepInfo.SetSubStep(4);
				}
			}
			break;

		case 2:
			{
				//检测Tray到位信号
				iRet = ReadInPort(_T("NG位伸出到位"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(3);
				}
			}
			break;

		case 3:
			{
				m_bAllowPutFail = TRUE;
				sStepInfo.SetSubStep(1);
			}
			break;

		case 4:
			{
				//Tray 伸出
				if (OpenOutPort(_T("NG位缩回")) && CloseOutPort(_T("NG位伸出")))
				{
					sStepInfo.SetSubStep(5);
				}
			}
			break;

		case 5:
			{
				//检测伸出到位
				iRet = ReadInPort(_T("NG位缩回到位"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(6);
					SetThreeLight(L_RED);
					SendMsg(WM_ULMSG_INFORMATION, INFO_FAILTRAYFULL, 0);
				}
			}
			break;

		case 6:
			{
				//检测Tray无信号
				iRet = ReadInPort(_T("NG位有无料"));
				if (0 == iRet)
				{
					sStepInfo.SetSubStep(7);
				}
			}
			break;

		case 7:
			{
				//500ms 扔检测不到开始检测Tray有的信号
				iRet = ReadInPort(_T("NG出料"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(8);
				}
			}
			break;

		case 8:
			{
				//检测有Tray的信号
				iRet = ReadInPort(_T("NG位有无料"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(9);
				}
				else
				{
					sStepInfo.SetSubStep(7);
				}
			}
			break;

		case 9:
			{
				//检测按钮被按下信号
				m_iPutFailPos = 0;
				SendMsg(WM_ULMSG_RESETPCB, 2, 0);
				sStepInfo.SetSubStep(0);
				
			}
			break;
		}

		m_classIniConfig.WriteInt(_T("StepInfo"), _T("FailTray"), sStepInfo.GetSubStep());
		m_classIniConfig.WriteInt(_T("StepInfo"), _T("AllowPutFail"), m_bAllowPutFail);
	}

	void CWorkFlow::F_PassTray(StepInfo &sStepInfo)
	{
		int iRet= 0;

		switch (sStepInfo.GetSubStep())
		{
		case 0:
			{
				//检测气缸位置
				iRet = ReadInPort(_T("中转位有无料"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(1);
				}
				else if (0 == iRet)
				{
					sStepInfo.SetSubStep(11);
				}
				else
				{
					//发送错误代码报警
				}

			}
			break;

		case 1:
			{
				//检测到是否有板
				if (ReadInPort(_T("中转位左移到位")) == 1)
				{
					sStepInfo.SetSubStep(4);
					m_iCylCount = 0;
				}
				else
				{
					sStepInfo.SetSubStep(2);	
				}
			}
			break;

		case 2:
			{
				if (OpenOutPort(_T("中转位左移")) && CloseOutPort(_T("中转位右移")))
				{
					sStepInfo.UpdataStTime();
					sStepInfo.SetSubStep(3);
				}
				else
				{
					//发送错误代码报警
				}


			}
			break;

		case 3:
			{
				//检测中转气缸右移到位
				iRet = ReadInPort(_T("中转位左移到位"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(4);
					m_iCylCount = 0;
				}
				else if (0 == iRet)
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						//发送超时报警
						AlarmSetStep(sStepInfo, 20, 3);
					}
				}
				else
				{
					//发送错误代码报警
				}
			}
			break;

		case 4:
			{
				//检测伸出到位
				if (OpenOutPort(_T("侧压定位Y伸出")))
				{
					sStepInfo.SetSubStep(5);
					sStepInfo.UpdataStTime();
				}

			}
			break;

		case 5:
			{
				//气缸夹紧
				iRet = ReadInPort(_T("侧压定位Y左右伸出"));
				if (iRet == 1)
				{
					sStepInfo.SetSubStep(5001);
					sStepInfo.UpdataStTime();
				}
				else if (0 == iRet)
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						//发送超时
						CloseOutPort(_T("侧压定位Y伸出"));
						sStepInfo.SetSubStep(4);
						Sleep(500);
					}
				}
			}
			break;

		case 5001:
			{
				if (sStepInfo.IsTimeOut(500))
				{
					if (OpenOutPort(_T("侧压定位X伸出")))
					{
						sStepInfo.SetSubStep(5002);
						sStepInfo.UpdataStTime();
					}
					
				}
			}
			break;


		case 5002:
			{

				if ((ReadInPort(_T("侧压定位X左右伸出")) == 1) )
				{
					sStepInfo.SetSubStep(10);
					sStepInfo.UpdataStTime();
				}
				else
				{
					if (sStepInfo.IsTimeOut(5001))
					{
						CloseOutPort(_T("侧压定位X伸出"));
						sStepInfo.UpdataStTime();
						sStepInfo.SetSubStep(5001);
					}
				}
			}
			break;

		case 10:
			{
				//检测气缸夹紧信号
//				if (m_iPutPassPos < (m_iTrayRow * m_iTrayCol))
				{
					if (sStepInfo.IsTimeOut(500))
					{
						m_bAllowPutPass = TRUE;
						sStepInfo.SetSubStep(11);
					}
					
				}
				
				
			}
			break;

		case 11:
			{
				//收料盘点击移动到指定位置
				iRet = ReadInPort(_T("供料位料盘有无"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(12);
				}
				else
				{
					sStepInfo.SetSubStep(100);
				}
			}
			break;

		case 12:
			{
				//检测传感器信号是否已经到位
				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 2);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

				if (m_classGT.AxisMove(2, m_lETrayPos, 0, dMoveSpeed, dMoveAcc, dMoveDec))
				{
					sStepInfo.SetSubStep(13);
				}
			}
			break;

		case 13:
			{
				if (ReadInPort(_T("供料位料盘到位")) == 1)
				{
					sStepInfo.SetSubStep(20);
				}
				else
				{
					sStepInfo.SetSubStep(14);
				}
			}
			break;

		case 14:
			{
				if (!m_classGT.AxisIsBusy(2))
				{
					sStepInfo.SetSubStep(100);
				}
				else
				{
					sStepInfo.SetSubStep(13);
				}
			}
			break;

		case 20:
			{
				if (m_classGT.AxisStop(2))
				{
					sStepInfo.SetSubStep(21);
				}
			}
			break;

		case 21:
			{
				iRet = ReadInPort(_T("收料位料盘有无"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(22);
				}
				else
				{
					sStepInfo.SetSubStep(25);
				}
			}
			break;
		
		case 22:
			{
// 				double dJogSpeed = 20.0;
// 				double dJogAcc = 0.1;
// 				double dJogDec = 0.1;
// 				TCHAR atcSection[128] = {0};
// 				swprintf(atcSection, _T("Z%d"), 1);
// 				dJogSpeed = m_classIniConfig.ReadDouble(atcSection, _T("JogSpeed"), 20.0);
// 				dJogAcc = m_classIniConfig.ReadDouble(atcSection, _T("JogAcc"), 0.1);
// 				dJogDec = m_classIniConfig.ReadDouble(atcSection, _T("JogDec"), 0.1);
// 
// 				if (m_classGT.JogMove(1, TRUE, dJogSpeed, dJogAcc, dJogDec))
// 				{
// 					sStepInfo.SetSubStep(23);
// 				}
				long lFTrayPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("FTrayPos"), 0);
				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 1);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

				if (m_classGT.AxisMove(1, lFTrayPos, 0, dMoveSpeed, dMoveAcc, dMoveDec))
				{
					sStepInfo.UpdataStTime();
					sStepInfo.SetSubStep(23);
				}
			}
			break;

		case 23:
			{
				if (ReadInPort(_T("收料位料盘到位")) == 1)
				{
					sStepInfo.SetSubStep(24);
				}
			}
			break;

		case 24:
			{
				if (m_classGT.AxisStop(1))
				{
					sStepInfo.SetSubStep(30);
				}
			}
			break;

		case 25:
			{
				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 1);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);
				long lFTrayPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("FTrayPos"), 0);
				if (m_classGT.AxisMove(1, lFTrayPos / 2, 0, dMoveSpeed, dMoveAcc, dMoveDec))
				{
					sStepInfo.UpdataStTime();
					sStepInfo.SetSubStep(26);
				}
			}
			break;

		case 26:
			{
				if (!m_classGT.AxisIsBusy(1))
				{
					sStepInfo.SetSubStep(30);
				}
			}
			break;



		case 30:
			{
				if (m_iPutPassPos >= (m_iTrayRow * m_iTrayCol))
				{
					m_bAllowPutPass = FALSE;
				}
	
				if (!m_bAllowPutPass)
				{
					sStepInfo.SetSubStep(31);
					m_iCylCount = 0;
					sStepInfo.UpdataStTime();
				}
				else
				{
					if (ReadInPort(_T("入料")) == 1)
					{
						sStepInfo.SetSubStep(100);
					}
					else
					{
						if (m_bTrayNRun)
						{
							if (sStepInfo.IsTimeOut(500))
							{
								++m_iPutPassPos;
								sStepInfo.UpdataStTime();
							}
						}
					}
				}
			
			}
			break;

		case 31:
			{
				if (CloseOutPort(_T("侧压定位Y伸出")) && CloseOutPort(_T("侧压定位X伸出")))
				{
					sStepInfo.SetSubStep(32);
					sStepInfo.UpdataStTime();
				}
			}
			break;


		case 32:
			{
				
				if ((ReadInPort(_T("侧压定位Y左右退回")) == 1) && (ReadInPort(_T("侧压定位X左右退回")) == 1))
				{
					if (m_iCylCount < 3)
					{
						sStepInfo.SetSubStep(32001);
					}
					else
					{
						sStepInfo.SetSubStep(33);
					}
				}
				else
				{
					if (sStepInfo.IsTimeOut(2000))
					{
						if (m_iCylCount < 3)
						{
							sStepInfo.SetSubStep(32001);
						}
						else
						{
							sStepInfo.SetSubStep(33);
						}
					}
				}
			}
			break;

		case 32001:
			{
				if (OpenOutPort(_T("侧压定位Y伸出")) && OpenOutPort(_T("侧压定位X伸出")))
				{
					sStepInfo.SetSubStep(32002);
					sStepInfo.UpdataStTime();
				}
			}
			break;


		case 32002:
			{
				if ((ReadInPort(_T("侧压定位Y左右伸出")) == 1) && (ReadInPort(_T("侧压定位X左右伸出")) == 1))
				{
					sStepInfo.SetSubStep(31);
					++m_iCylCount;
					Sleep(500);
				}
				else
				{
					if (sStepInfo.IsTimeOut(2000))
					{
						sStepInfo.SetSubStep(31);
						++m_iCylCount;
						Sleep(500);
					}
				}
			}
			break;

		case 33:
			{
				iRet = ReadInPort(_T("收料位料盘有无"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(34);
					sStepInfo.UpdataStTime();
				}
				else
				{
					sStepInfo.SetSubStep(33001);
				}
			}
			break;

		case 33001:
			{
				long lFTrayPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("FTrayPos"), 0);
				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 1);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);
				if (m_classGT.AxisMove(1, lFTrayPos, 0, dMoveSpeed, dMoveAcc, dMoveDec))
				{
					sStepInfo.UpdataStTime();
					sStepInfo.SetSubStep(36);
				}
			}
			break;


		case 34:
			{
				if (ReadInPort(_T("收料位料盘到位")) == 1)
				{
					sStepInfo.SetSubStep(35);
				}
				else
				{
					if (sStepInfo.IsTimeOut(1000))
					{
						sStepInfo.SetSubStep(34001);
					}					
				}
			}
			break;

		case 34001:
			{
				if (!m_classGT.AxisIsBusy(1))
				{
					sStepInfo.SetSubStep(34002);
				}
				else
				{
					sStepInfo.SetSubStep(34);
				}
			}
			break;

		case 34002:
			{
// 				double dJogSpeed = 20.0;
// 				double dJogAcc = 0.1;
// 				double dJogDec = 0.1;
// 				TCHAR atcSection[128] = {0};
// 				swprintf(atcSection, _T("Z%d"), 1);
// 				dJogSpeed = m_classIniConfig.ReadDouble(atcSection, _T("JogSpeed"), 20.0);
// 				dJogAcc = m_classIniConfig.ReadDouble(atcSection, _T("JogAcc"), 0.1);
// 				dJogDec = m_classIniConfig.ReadDouble(atcSection, _T("JogDec"), 0.1);
// 
// 				if (m_classGT.JogMove(1, TRUE, dJogSpeed, dJogAcc, dJogDec))
// 				{
// 					RecordLog(_T("Jog移动!"), 4);
// 					sStepInfo.SetSubStep(34003);
// 				}
				long lFTrayPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("FTrayPos"), 0);
				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 1);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

				if (m_classGT.AxisMove(1, lFTrayPos, 0, dMoveSpeed, dMoveAcc, dMoveDec))
				{
					sStepInfo.UpdataStTime();
					sStepInfo.SetSubStep(34003);
				}
			}
			break;

		case 34003:
			{
				if (ReadInPort(_T("收料位料盘到位")) == 1)
				{
					sStepInfo.SetSubStep(34004);
				}
			}
			break;

		case 34004:
			{
				if (m_classGT.AxisStop(1))
				{
					sStepInfo.SetSubStep(35);
				}
			}
			break;
			

		case 35:
			{
				long lPos = 0;

				if (m_classGT.GetCurPos(1, lPos))
				{
					long lFTrayPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("FTrayPos"), 0);
					long lFTrayOffset = m_classIniProduct.ReadInt(_T("GTPos"), _T("FTrayOffset"), 0);
					double dMoveSpeed = 20.0;
					double dMoveAcc = 0.1;
					double dMoveDec = 0.1;
					TCHAR atcSection[128] = {0};
					swprintf(atcSection, _T("Z%d"), 1);
					dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
					dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
					dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

					if ((lPos + lFTrayOffset) <  lFTrayPos)
					{
						if (m_classGT.AxisMove(1, (lPos + lFTrayOffset), 0, dMoveSpeed, dMoveAcc, dMoveDec))
						{
							TCHAR atcMsg[256] = {0};
							swprintf(atcMsg, _T("当前位置: %d, FTrayOffset: %d,"), lPos, lFTrayOffset);
							RecordLog(atcMsg, 4);
							sStepInfo.UpdataStTime();
							sStepInfo.SetSubStep(36);
						}
						else
						{
							TCHAR atcMsg[256] = {0};
							swprintf(atcMsg, _T("移动失败 >> 当前位置: %d, FTrayOffset: %d,"), lPos, lFTrayOffset);
							RecordLog(atcMsg, 4);
						}
					}				
				}
				
			}
			break;

		case 36:
			{
				if (!m_classGT.AxisIsBusy(1))
				{
					sStepInfo.SetSubStep(37);
				}
			}
			break;

		case 37:
			{
				if ((ReadInPort(_T("中转位对射传感器1")) == 1) && (ReadInPort(_T("中转位对射传感器2")) == 1))
				{
					if (OpenOutPort(_T("中转位右移"))  &&  CloseOutPort(_T("中转位左移")))
					{
						sStepInfo.SetSubStep(38);
						sStepInfo.UpdataStTime();
					}
				}
				else
				{
					if (!IsSuspend())
					{
						Suspend();
						SendMsg(WM_ULMSG_SUSPEND, NULL, NULL);
					}
					SetThreeLight(L_RED);
					SendMsg(WM_ULMSG_WARNING, 31, NULL);	
				}
			}
			break;

		case 38:
			{
				if (ReadInPort(_T("中转位右移到位")) == 1)
				{
					sStepInfo.SetSubStep(39001);
				}
				else
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						//发送超时报警
						AlarmSetStep(sStepInfo, 21, 38);
					}
				}
			}
			break;

		case 39:
			{
				int iFullPos = m_classIniConfig.ReadInt(_T("GTPos"), _T("FFullPos"), 100);
				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 1);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

				if (m_classGT.AxisMove(1, iFullPos, 0, dMoveSpeed, dMoveAcc, dMoveDec))
				{
					sStepInfo.SetSubStep(40);
				}
			}
			break;

		case 39001:
			{
				iRet = ReadInPort(_T("中转位有无料"));
				if (0 == iRet)
				{
					sStepInfo.SetSubStep(39);
				}
				else if (1 == iRet)
				{
					SetThreeLight(L_RED);
					SendMsg(WM_ULMSG_WARNING, 28, NULL);
				}

			}
			break;

		case 39002:
			{
				iRet = ReadInPort(_T("中框带料信号"));
				if (0 == iRet)
				{
					sStepInfo.SetSubStep(39);
				}
				else if (1 == iRet)
				{
					SetThreeLight(L_RED);
					SendMsg(WM_ULMSG_WARNING, 28, NULL);
				}
			}
			break;

		case 40:
			{
				if (ReadInPort(_T("供料位料盘到位")) == 1)
				{
					sStepInfo.SetSubStep(41);
				}
				else
				{
					sStepInfo.SetSubStep(41);
				}
			}
			break;

		case 41:
			{
				long lETrayOffset = m_classIniProduct.ReadInt(_T("GTPos"), _T("ETrayOffset"), 0);
				int iDistance = m_classIniConfig.ReadInt(_T("GTPos"), _T("Distance"), 300);

				if ((iDistance < 100) || (iDistance > 1000))
				{
					iDistance = 300;
				}

				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 2);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

				if (m_classGT.AxisMove(2, lETrayOffset + iDistance * m_iAddDis, 1, dMoveSpeed, dMoveAcc, dMoveDec))
				{
					sStepInfo.SetSubStep(42);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 42:
			{
				if (!m_classGT.AxisIsBusy(2))
				{
					sStepInfo.SetSubStep(43);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 43:
			{
				if (sStepInfo.IsTimeOut(500))
				{
					sStepInfo.SetSubStep(44);
				}
			}
			break;

		case 44:
			{
				long lETrayOffset = m_classIniProduct.ReadInt(_T("GTPos"), _T("ETrayOffset"), 0);
				int iDistance = m_classIniConfig.ReadInt(_T("GTPos"), _T("Distance"), 300);

				if ((iDistance < 100) || (iDistance > 1000))
				{
					iDistance = 300;
				}

				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 2);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

				if (m_classGT.AxisMove(2, -(lETrayOffset + iDistance * m_iAddDis), 1, dMoveSpeed, dMoveAcc, dMoveDec))
				{
					sStepInfo.SetSubStep(45);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 45:
			{
				if (!m_classGT.AxisIsBusy(2))
				{
					sStepInfo.SetSubStep(46);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 46:
			{
				iRet = ReadInPort(_T("中转位有无料"));
				if (1 == iRet)
				{
					sStepInfo.SetSubStep(47);
					sStepInfo.UpdataStTime();
					m_iAddDis = 0;
				}
				else
				{
					++m_iAddDis;
					sStepInfo.SetSubStep(41);
				}
			}
			break;

		case 47:
			{
				if (!m_classGT.AxisIsBusy(1))
				{
					sStepInfo.SetSubStep(48);
				}
				else
				{
					sStepInfo.SetSubStep(47001);
				}
					
			}
			break;

		case 47001:
			{
				if (ReadInPort(_T("供料位料盘到位")) == 0)
				{
					if (sStepInfo.IsTimeOut(500))
					{
						sStepInfo.SetSubStep(47002);
					}					
				}
				else
				{
					sStepInfo.SetSubStep(47);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 47002:
			{
				if (m_classGT.AxisStop(1))
				{
					sStepInfo.SetSubStep(47);
				}
			}
			break;

		case 48:
			{
				if (CloseOutPort(_T("中转位右移"))  &&  OpenOutPort(_T("中转位左移")))
				{
					sStepInfo.SetSubStep(49);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 49:
			{
				if (ReadInPort(_T("中转位左移到位")) == 1)
				{
					sStepInfo.SetSubStep(50);
				}
				else
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, 20, 49);
					}
				}
			}
			break;

		case 50:
			{
				//检测伸出到位
				if (OpenOutPort(_T("侧压定位Y伸出")))
				{
					sStepInfo.SetSubStep(51);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 51:
			{
				//气缸夹紧
				iRet = ReadInPort(_T("侧压定位Y左右伸出"));
					if (1 == iRet)
					{
						sStepInfo.SetSubStep(51001);
						sStepInfo.UpdataStTime();
					}
					else
					{
						if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
						{
//							CloseOutPort(_T("侧压定位Y伸出"));
							sStepInfo.SetSubStep(51001);
							Sleep(500);
						}
						
					}
		
			}
			break;

		case 51001:
			{
				if (sStepInfo.IsTimeOut(500))
				{
					if (OpenOutPort(_T("侧压定位X伸出")))
					{
						sStepInfo.SetSubStep(51002);
						sStepInfo.UpdataStTime();
					}

				}
			}
			break;

		case 51002:
			{
				
					iRet = ReadInPort(_T("侧压定位X左右伸出"));

					if (1 == iRet)
					{
						sStepInfo.SetSubStep(52);
						sStepInfo.UpdataStTime();
					}
					else
					{
						if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
						{
							//发送超时
//							CloseOutPort(_T("侧压定位X伸出"));
							sStepInfo.SetSubStep(52);
							sStepInfo.UpdataStTime();
						}
					}
			}
			break;

		case 52:
			{
				iRet = ReadInPort(_T("中转位有无料"));
				if (0 == iRet)
				{
					SetThreeLight(L_RED);
					SendMsg(WM_ULMSG_WARNING, 32, NULL);
				}
				else if (1 == iRet)
				{
					m_iPutPassPos = 0;
					m_bAllowPutPass = TRUE;
					sStepInfo.SetSubStep(53);
					SendMsg(WM_ULMSG_RESETPCB, 1, 0);				
				}

			}
			break;

		case 53:
			{
				if ((ReadInPort(_T("收料位料盘到位")) == 1) || (ReadInPort(_T("供料位料盘有无")) == 0))
				{
					sStepInfo.SetSubStep(100);
				}
				else
				{
					sStepInfo.SetSubStep(11);
				}
			}
			break;

		case 100:
			{
				int iOpenPos = m_classIniConfig.ReadInt(_T("GTPos"), _T("FOpenPos"), HOME_OFFSET);
				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 1);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

				if (m_classGT.AxisMove(1, iOpenPos, 0, dMoveSpeed, dMoveAcc, dMoveDec) )
				{
					sStepInfo.SetSubStep(101);
				}
			}
			break;

		case 101:
			{
				int iOpenPos = m_classIniConfig.ReadInt(_T("GTPos"), _T("EOpenPos"), HOME_OFFSET);
				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 2);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

				if (m_classGT.AxisMove(2, iOpenPos, 0, dMoveSpeed, dMoveAcc, dMoveDec) )
				{
					sStepInfo.SetSubStep(102);
				}
			}
			break;

		case 102:
			{
				if (!m_classGT.AxisIsBusy(1) && !m_classGT.AxisIsBusy(2))
				{
					sStepInfo.SetSubStep(103);
				}
			}
			break;

		case 103:
			{
				if (CloseOutPort(_T("料框夹夹紧")) && OpenOutPort(_T("料框夹松开")))
				{
					sStepInfo.SetSubStep(104);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 104:
			{
				if ((ReadInPort(_T("收料位侧夹松开到位")) == 1))
				{
					sStepInfo.SetSubStep(105);
				}
				else
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, 22, 104);
					}
				}
			}
			break;

		case 105:
			{
				SetThreeLight(L_RED);
				SendMsg(WM_ULMSG_INFORMATION, INFO_BOXFULLOREMPTY, NULL);
				SetThreeLight(L_GRN);
				sStepInfo.SetSubStep(106);
			}
			break;

		case 106:
			{
	//			if ((ReadInPort(_T("满料抽屉到位")) == 0))
				{
					sStepInfo.SetSubStep(107);
				}
			}
			break;

		case 107:
			{
				if ((ReadInPort(_T("入料")) == 1))
				{
					sStepInfo.SetSubStep(108);
				}
			}
			break;

		case 108:
			{
				if ((ReadInPort(_T("满料抽屉到位")) == 1) && (ReadInPort(_T("空料抽屉到位")) == 1))
				{
					sStepInfo.SetSubStep(109);
				}
			}
			break;

		case 109:
			{
				if (OpenOutPort(_T("料框夹夹紧")) && CloseOutPort(_T("料框夹松开")))
				{
					sStepInfo.SetSubStep(110);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 110:
			{
				if ((ReadInPort(_T("收料位侧夹夹紧到位")) == 1))
				{
					sStepInfo.SetSubStep(111);
				}
				else
				{
					if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
					{
						AlarmSetStep(sStepInfo, 23, 110);
					}
				}
			}
			break;

		case 111:
			{
				int iCheckPos = m_classIniConfig.ReadInt(_T("GTPos"), _T("FCheckPos"), 100);
				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 1);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

				if (m_classGT.AxisMove(1, iCheckPos, 0, dMoveSpeed, dMoveAcc, dMoveDec) )
				{
					sStepInfo.SetSubStep(112);
				}
			}
			break;

		case 112:
			{
				int iCheckPos = m_classIniConfig.ReadInt(_T("GTPos"), _T("ECheckPos"), 100);
				double dMoveSpeed = 20.0;
				double dMoveAcc = 0.1;
				double dMoveDec = 0.1;
				TCHAR atcSection[128] = {0};
				swprintf(atcSection, _T("Z%d"), 2);
				dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
				dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
				dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

				if (m_classGT.AxisMove(2, iCheckPos, 0, dMoveSpeed, dMoveAcc, dMoveDec) )
				{
					sStepInfo.SetSubStep(113);
				}
			}
			break;

		case 113:
			{
				if (!m_classGT.AxisIsBusy(1) && !m_classGT.AxisIsBusy(2))
				{
					sStepInfo.SetSubStep(11);
				}
			}
			break;		

		}

		m_classIniConfig.WriteInt(_T("StepInfo"), _T("PassTray"), sStepInfo.GetSubStep());
	}

	void CWorkFlow::F_EmptyTray(StepInfo &sStepInfo)
	{
		switch (sStepInfo.GetSubStep())
		{
		case 0:
			{
				if (m_bReset)
				{
					sStepInfo.SetSubStep(1);
				}
			}
			break;

		case 1:
			{
				if (ReadInPort(_T("中转位右移到位")) == 1)
				{
					m_bAllowPutPass = FALSE;
					sStepInfo.SetSubStep(2);
				}
				else
				{
					sStepInfo.SetSubStep(10);
				}
			}
			break;

		case 2:
			{
				if (ReadInPort(_T("中转位有无料")) == 1)
				{
					sStepInfo.SetSubStep(3);
				}
				else
				{
					sStepInfo.SetSubStep(10);
				}
			}
			break;

		case 3:
			{
				if (m_bAllowMoveToFull)
				{
					sStepInfo.SetSubStep(4);
				}
			}
			break;

		case 4:
			{
				if (CloseOutPort(_T("中转位右移"))  &&  OpenOutPort(_T("中转位左移")))
				{
					sStepInfo.SetSubStep(5);
				}
			}
			break;

		case 5:
			{
				if (ReadInPort(_T("中转位左移到位")) == 1)
				{
					sStepInfo.SetSubStep(6);
				}
			}
			break;

		case 6:
			{
				if (OpenOutPort(_T("侧压定位Y伸出")) && OpenOutPort(_T("侧压定位X伸出")))
				{
					sStepInfo.SetSubStep(7);
				}
			}
			break;

		case 7:
			{
				if ((ReadInPort(_T("侧压定位Y左右伸出")) == 1) && (ReadInPort(_T("侧压定位X左右伸出")) == 1))
				{
					sStepInfo.SetSubStep(1);
					m_bAllowPutPass = TRUE;
					m_bAllowMoveToFull = FALSE;
				}
				
			}
			break;

		case 10:
			{
				if ((ReadInPort(_T("空料抽屉到位")) == 1))
				{
//					if ( (ReadInPort(_T("供料位侧夹夹紧到位")) == 1))
					{
						sStepInfo.SetSubStep(11);
					}
// 					else
// 					{
// 						if (sStepInfo.IsTimeOut(1000))
// 						{
// 							CloseOutPort(_T("料框夹松开"));							
// 							OpenOutPort(_T("料框夹夹紧"));
// 							sStepInfo.UpdataStTime();
// 						}
// 					}
				}
				
			}
			break;

		case 11:
			{
				if (m_classGT.AxisMove(2, HOME_OFFSET, 0))
				{
					sStepInfo.SetSubStep(12);
				}
			}
			break;

		case 12:
			{
				if (!m_classGT.AxisIsBusy(2))
				{
					m_bAllowMoveToEmpty = TRUE;
					sStepInfo.SetSubStep(13);
				}
			}
			break;

		case 13:
			{
				if (ReadInPort(_T("供料位料盘有无")) == 1)
				{
					sStepInfo.SetSubStep(14);
				}
				else
				{
					sStepInfo.SetSubStep(19);
				}
			}
			break;

		case 14:
			{
				if (m_classGT.AxisMove(2, m_lETrayPos, 0))
				{
					sStepInfo.SetSubStep(15);
				}
			}
			break;

		case 15:
			{
				if (ReadInPort(_T("供料位料盘到位")) == 1)
				{
					sStepInfo.SetSubStep(25);
				}
				else
				{
					sStepInfo.SetSubStep(16);
				}
			}
			break;

		case 16:
			{
				if (!m_classGT.AxisIsBusy(2))
				{
					sStepInfo.SetSubStep(17);
				}
				else
				{
					sStepInfo.SetSubStep(15);
				}

			}
			break;

		case 17:
			{
				if (m_classGT.AxisMove(2, HOME_OFFSET, 0))
				{
					sStepInfo.SetSubStep(18);
				}
			}
			break;

		case 18:
			{
				if (!m_classGT.AxisIsBusy(2))
				{
					sStepInfo.SetSubStep(19);
				}
				
			}
			break;

		case 19:
			{
				if (CloseOutPort(_T("料框夹夹紧")) && OpenOutPort(_T("料框夹松开")))
				{
					sStepInfo.SetSubStep(20);
				}
			}
			break;

		case 20:
			{
				if ((ReadInPort(_T("供料位侧夹松开到位")) == 1))
				{
					sStepInfo.SetSubStep(21);
				}
			}
			break;

		case 21:
			{
				//发送报警型号 
				sStepInfo.SetSubStep(40);
			}
			break;

		case 25:
			{
				if (m_classGT.AxisStop(2))
				{
					sStepInfo.SetSubStep(30);
				}
			}
			break;

		case 30:
			{
				if ((ReadInPort(_T("中转位右移到位")) == 1))
				{
					if (sStepInfo.IsTimeOut(500))
					{
						if (ReadInPort(_T("中转位有无料")) == 1)
						{
							sStepInfo.SetSubStep(2);
						}
						else
						{
							m_bAllowPutPass = FALSE;
							sStepInfo.SetSubStep(31);
							sStepInfo.UpdataStTime();
						}
						
					} 
				
				}
				else
				{
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 31:
			{
				if (sStepInfo.IsTimeOut(500) && m_classGT.AxisMove(2, m_lETrayOffset, 1))
				{
					sStepInfo.SetSubStep(32);
				}
			}
			break;

		case 32:
			{
				if (!m_classGT.AxisIsBusy(2))
				{
					sStepInfo.SetSubStep(33);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 33:
			{
				if (sStepInfo.IsTimeOut(500) && m_classGT.AxisMove(2, -m_lETrayOffset, 1))
				{
					sStepInfo.SetSubStep(34);
				}
			}
			break;

		case 34:
			{
				if (!m_classGT.AxisIsBusy(2))
				{
					sStepInfo.SetSubStep(35);
				}
			}
			break;

		case 35:
			{
				if (ReadInPort(_T("中转位有无料")) == 1)
				{
					m_iPutPassPos = 0;
					SendMsg(WM_ULMSG_RESETPCB, 1, 0);
					sStepInfo.SetSubStep(3);
				}
			}
			break;

		case 40:
			{
				if ((ReadInPort(_T("空料抽屉到位")) == 0) && (ReadInPort(_T("供料位侧夹夹紧到位")) == 0))
				{
					sStepInfo.SetSubStep(41);
				}
			}
			break;

		case 41:
			{
				if ((ReadInPort(_T("入料")) == 1))
				{
					sStepInfo.SetSubStep(42);
				}
			}
			break;

		case 42:
			{
				if ((ReadInPort(_T("空料抽屉到位")) == 1) 	&& (ReadInPort(_T("供料位料盘有无")) == 1))
				{
					sStepInfo.SetSubStep(43);
				}
				else
				{
					sStepInfo.SetSubStep(41);
				}
			}
			break;

		case 43:
			{
				if (OpenOutPort(_T("料框夹夹紧")) && CloseOutPort(_T("料框夹松开")))
				{
					sStepInfo.SetSubStep(13);
				}
			}

		}

		m_classIniConfig.WriteInt(_T("StepInfo"), _T("EmptyTray"), sStepInfo.GetSubStep());
		m_classIniConfig.WriteInt(_T("StepInfo"), _T("AllowMoveToEmpty"), m_bAllowMoveToEmpty);
	}

	void CWorkFlow::F_FullTray(StepInfo &sStepInfo)
	{
		switch (sStepInfo.GetSubStep())
		{
		case 0:
			{
				if (m_bReset)
				{
					sStepInfo.SetSubStep(1);
				}
			}
			break;

		case 1:
			{
				if (m_classGT.AxisMove(1, HOME_OFFSET, 0))
				{
					sStepInfo.SetSubStep(2);
					sStepInfo.UpdataStTime();
				}
			}
			break;

		case 2:
			{
				if (!m_classGT.AxisIsBusy(1))
				{
					if (ReadInPort(_T("收料位料盘到位")) == 1)
					{
						sStepInfo.SetSubStep(30);
					}
					else
					{
						sStepInfo.SetSubStep(3);
					}
					
					if (!m_bAllowMoveToFull)
					{
						m_bAllowMoveToFull = TRUE;
					}
				}
				else
				{
					if (ReadInPort(_T("收料位料盘到位")) == 1)
					{
						sStepInfo.UpdataStTime();
					}
					else
					{
						if (!m_bAllowMoveToFull)
						{
							if (sStepInfo.IsTimeOut(1000))
							{
								m_bAllowMoveToFull = TRUE;
								if (m_classGT.AxisStop(1))
								{
									sStepInfo.SetSubStep(3);
								}
							}
						}
						
					}
				}
			}
			break;

		case 3:
			{
				//中转位有无料
				
					if ((ReadInPort(_T("中转位左移到位")) == 1) && m_bAllowPutPass)
					{
						if (ReadInPort(_T("中转位有无料")) == 1)
						{
							sStepInfo.SetSubStep(4);
							sStepInfo.UpdataStTime();
						}			
						else
						{
							m_bAllowMoveToFull = FALSE;
							sStepInfo.SetSubStep(13);
						}
					}
				
			}
			break;

		case 4:
			{
//				if (m_bAllowPutPass)
				{
					if ((ReadInPort(_T("侧压定位Y左右伸出")) == 1) && (ReadInPort(_T("侧压定位X左右伸出")) == 1))
					{
						sStepInfo.SetSubStep(5);
						sStepInfo.UpdataStTime();
					}
					else
					{
						if (sStepInfo.IsTimeOut(1000))
						{
							OpenOutPort(_T("侧压定位Y伸出"));
							OpenOutPort(_T("侧压定位X伸出"));
							sStepInfo.UpdataStTime();
						}
						
					}
	
				}
			}
			break;

		case 5:
			{
				if (m_iPutPassPos >= m_iTrayRow * m_iTrayCol)
				{
					if (CloseOutPort(_T("侧压定位Y伸出")) && CloseOutPort(_T("侧压定位X伸出")))
					{
						sStepInfo.SetSubStep(6);
					}					
				}
				else
				{
					if (sStepInfo.IsTimeOut(100))
					{
//						++m_iPutPassPos;
						sStepInfo.UpdataStTime();
					}
				}

			}
			break;

		case 6:
			{
				if ((ReadInPort(_T("侧压定位Y左右退回")) == 1) && (ReadInPort(_T("侧压定位X左右退回")) == 1))
				{
					if (ReadInPort(_T("收料位料盘有无")) == 1)
					{
						sStepInfo.SetSubStep(7);
					}
					else
					{
						if (m_classGT.AxisMove(1, m_lFTrayPos, 0))
						{
							sStepInfo.UpdataStTime();
							sStepInfo.SetSubStep(20);
						}
					}
				}
				
			}
			break;

		case 7:
			{
				if (m_classGT.JogMove(1, TRUE))
				{
					sStepInfo.SetSubStep(8);
				}
			}
			break;

		case 8:
			{
				if (ReadInPort(_T("收料位料盘到位")) == 1)
				{
					sStepInfo.SetSubStep(10);
				}
// 				else
// 				{
// 					sStepInfo.SetSubStep(9);
// 				}
			}
			break;
			
		case 9:
			{
				if (!m_classGT.AxisIsBusy(1))
				{
					//发送停止信号 报警
				}
			}
			break;

		case 10:
			{
				if (m_classGT.AxisStop(1))
				{
					sStepInfo.SetSubStep(11);
				}
			}
			break;

		case 11:
			{
				if (m_classGT.AxisMove(1, m_lFTrayOffset, 1))
				{
					sStepInfo.SetSubStep(12);
				}
			}
			break;

		case 12:
			{
				if (!m_classGT.AxisIsBusy(1))
				{
					m_bAllowMoveToFull = FALSE;
					sStepInfo.SetSubStep(13);
				}
			}
			break;

		case 13:
			{
				if ((ReadInPort(_T("侧压定位Y左右退回")) == 1) && (ReadInPort(_T("侧压定位X左右退回")) == 1))
				{
					if (CloseOutPort(_T("中转位左移")) && OpenOutPort(_T("中转位右移")))
					{
						sStepInfo.SetSubStep(14);
					}
				}
				else
				{
					if (sStepInfo.IsTimeOut(1000))
					{
						CloseOutPort(_T("侧压定位Y伸出"));
						CloseOutPort(_T("侧压定位X伸出"));
						sStepInfo.UpdataStTime();
					}
				}
			}
			break;

		case 14:
			{
				if (ReadInPort(_T("中转位右移到位")) == 1)
				{
					sStepInfo.SetSubStep(1);
				}
			}
			break;

		case 20:
			{
				if (ReadInPort(_T("收料位料盘有无")) == 1)
				{
					sStepInfo.SetSubStep(8);
				}
				else
				{
					if (sStepInfo.IsTimeOut(3000))
					{
						sStepInfo.SetSubStep(12);
					}
				}
			}
			break;

		case 30:
			{
				if (CloseOutPort(_T("料框夹夹紧")) && OpenOutPort(_T("料框夹松开")))
				{
					sStepInfo.SetSubStep(31);
				}
			}
			break;

		case 31:
			{
				if ((ReadInPort(_T("收料位侧夹松开到位")) == 1))
				{
					sStepInfo.SetSubStep(32);
				}
			}
			break;

		case 32:
			{
				//发送收料框满信号 报警
				sStepInfo.SetSubStep(33);
			}
			break;

		case 33:
			{
				if ((ReadInPort(_T("满料抽屉到位")) == 0) && (ReadInPort(_T("供料位侧夹夹紧到位")) == 0))
				{
					sStepInfo.SetSubStep(34);
				}
			}
			break;

		case 34:
			{
				if ((ReadInPort(_T("入料")) == 1))
				{
					sStepInfo.SetSubStep(35);
				}
			}
			break;

		case 35:
			{
				if ((ReadInPort(_T("空料抽屉到位")) == 1) 	&& (ReadInPort(_T("供料位料盘有无")) == 1))
				{
					sStepInfo.SetSubStep(36);
				}
				else
				{
					sStepInfo.SetSubStep(34);
				}
			}
			break;

		case 36:
			{
				if (OpenOutPort(_T("料框夹夹紧")) && CloseOutPort(_T("料框夹松开")))
				{
					sStepInfo.SetSubStep(3);
				}
			}
		}

		m_classIniConfig.WriteInt(_T("StepInfo"), _T("FullTray"), sStepInfo.GetSubStep());
		m_classIniConfig.WriteInt(_T("StepInfo"), _T("AllowMoveToFull"), m_bAllowMoveToFull);
	}

	void CWorkFlow::F_InTrack(StepInfo &sStepInfo)
	{
		int iRet = 0;

		switch (sStepInfo.GetSubStep())
		{
		case 0:
			{
				//检测准备好信号
				iRet = ReadInPort(_T("分板机准备完成"));

				if ((iRet == 1) && CloseOutPort(_T("取料完成")))
				{
					sStepInfo.SetSubStep(1);
				}
			}
			break;

		case 1:
			{
				//关闭所有信号
				if (CloseOutPort(_T("移至拍照位1")) && CloseOutPort(_T("移至拍照位2")) 
					&& CloseOutPort(_T("移至拍照位3")) && CloseOutPort(_T("移至拍照位4")))
				{
					sStepInfo.SetSubStep(2);
					m_iCaptrueIndex = 0;
				}				
			}
			break;

		case 2:
			{
				if (m_bSkipVision)
				{
					for (int i = 0; i < 50; ++i)
					{
						m_aiTestResult[i] = 1;
					}

					if (CloseOutPort(_T("移至拍照位1")) && CloseOutPort(_T("移至拍照位2")) 
						&& CloseOutPort(_T("移至拍照位3")) && CloseOutPort(_T("移至拍照位4"))
						&& OpenOutPort(_T("移至取料位")))
					{
						sStepInfo.SetSubStep(8);
					}	
				}
				else
				{
					if (m_iCaptrueCount > 2)
					{
						if (m_iCaptrueIndex < m_iCaptrueCount)
						{
							//发送到拍照m_iCaptureIndex位
							switch (m_iCaptrueIndex)
							{
							case 0:
								{
									if (OpenOutPort(_T("移至拍照位1")) && OpenOutPort(_T("移至拍照位4")))
									{
										sStepInfo.SetSubStep(3);
									}
								}
								break;

							case 1:
								{
									if (OpenOutPort(_T("移至拍照位2")) && OpenOutPort(_T("移至拍照位4")))
									{
										sStepInfo.SetSubStep(3);
									}
								}
								break;

							case 2:
								{
									if (OpenOutPort(_T("移至拍照位2")) && OpenOutPort(_T("移至拍照位3")))
									{
										sStepInfo.SetSubStep(3);
									}
								}
								break;

							case 3:
								{
									if (OpenOutPort(_T("移至拍照位1")) && OpenOutPort(_T("移至拍照位3")))
									{
										sStepInfo.SetSubStep(3);
									}
								}
								break;
							}

							sStepInfo.UpdataStTime();
						}
						else
						{
							sStepInfo.SetSubStep(6);
						}
					}
					else
					{
						if (m_iCaptrueIndex < m_iCaptrueCount)
						{
							//发送到拍照m_iCaptureIndex位
							switch (m_iCaptrueIndex)
							{
							case 0:
								{
									if (OpenOutPort(_T("移至拍照位1")))
									{
										sStepInfo.SetSubStep(3);
									}
								}
								break;

							case 1:
								{
									if (OpenOutPort(_T("移至拍照位2")) )
									{
										sStepInfo.SetSubStep(3);
									}
								}
								break;

							case 2:
								{
									if (OpenOutPort(_T("移至拍照位2")) && OpenOutPort(_T("移至拍照位3")))
									{
										sStepInfo.SetSubStep(3);
									}
								}
								break;

							case 3:
								{
									if (OpenOutPort(_T("移至拍照位1")) && OpenOutPort(_T("移至拍照位3")))
									{
										sStepInfo.SetSubStep(3);
									}
								}
								break;
							}

							sStepInfo.UpdataStTime();
						}
						else
						{
							sStepInfo.SetSubStep(6);
						}
					}
					
				}
				
				
			}
			break;

		case 3:
			{
				//检测到拍照位置信号
				if (m_iCaptrueCount > 2)
				{
					switch (m_iCaptrueIndex)
					{
					case 0:
						{
							if ((ReadInPort(_T("已到拍照位1")) == 1) && (ReadInPort(_T("已到拍照位4")) == 1) )
							{
								sStepInfo.SetSubStep(4);
							}
							else
							{
								if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
								{
									AlarmSetStep(sStepInfo, 24, 4);
								}
							}
						}
						break;

					case 1:
						{
							if ((ReadInPort(_T("已到拍照位2")) == 1) && (ReadInPort(_T("已到拍照位4")) == 1) )
							{
								sStepInfo.SetSubStep(4);
							}
							else
							{
								if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
								{
									AlarmSetStep(sStepInfo, 25, 4);
								}
							}
						}
						break;

					case 2:
						{
							if ((ReadInPort(_T("已到拍照位2")) == 1) && (ReadInPort(_T("已到拍照位3")) == 1))
							{
								sStepInfo.SetSubStep(4);
							}
							else
							{
								if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
								{
									AlarmSetStep(sStepInfo, 26, 4);
								}
							}
						}
						break;

					case 3:
						{
							if ((ReadInPort(_T("已到拍照位1")) == 1) && (ReadInPort(_T("已到拍照位3")) == 1))
							{
								sStepInfo.SetSubStep(4);
							}
							else
							{
								if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
								{
									AlarmSetStep(sStepInfo, 27, 4);
								}
							}
						}
						break;
					}
				}
				else
				{
					switch (m_iCaptrueIndex)
					{
					case 0:
						{
							if ((ReadInPort(_T("已到拍照位1")) == 1))
							{
								sStepInfo.SetSubStep(4);
							}
							else
							{
								if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
								{
									AlarmSetStep(sStepInfo, 24, 4);
								}
							}
						}
						break;

					case 1:
						{
							if ((ReadInPort(_T("已到拍照位2")) == 1))
							{
								sStepInfo.SetSubStep(4);
							}
							else
							{
								if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
								{
									AlarmSetStep(sStepInfo, 25, 4);
								}
							}
						}
						break;

					case 2:
						{
							if ((ReadInPort(_T("已到拍照位2")) == 1) && (ReadInPort(_T("已到拍照位3")) == 1))
							{
								sStepInfo.SetSubStep(4);
							}
							else
							{
								if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
								{
									AlarmSetStep(sStepInfo, 26, 4);
								}
							}
						}
						break;

					case 3:
						{
							if ((ReadInPort(_T("已到拍照位1")) == 1) && (ReadInPort(_T("已到拍照位3")) == 1))
							{
								sStepInfo.SetSubStep(4);
							}
							else
							{
								if (sStepInfo.IsTimeOut(CYL_TIMEOUT))
								{
									AlarmSetStep(sStepInfo, 27, 4);
								}
							}
						}
						break;
					}
				}
				

				
			}
			break;

		case 4:
			{
				//发送拍照信号
				TCHAR atcMsg[128] = {0};
				swprintf(atcMsg, _T("%s %d-%d:%d,%d"), m_strProductName.c_str(), m_iCaptrueIndex + 1, m_iCaptrueCount, m_mapCaptureInfo[m_iCaptrueIndex].iRow, m_mapCaptureInfo[m_iCaptrueIndex].iCol);
				USES_CONVERSION;
				if (m_pIVision && m_pIVision->SendData((BYTE *)T2A(atcMsg), _tcslen(atcMsg)))
				{
					RecordLog(atcMsg, 2);
					sStepInfo.SetSubStep(5);
					sStepInfo.UpdataStTime();
				}
				else
				{
					if (!m_pIVision->IsConnect())
					{
						tstring strIp = m_classIniConfig.ReadString(_T("Vision"), _T("Ip"), _T("127.0.0.1"));
						int iPort = m_classIniConfig.ReadInt(_T("Vision"), _T("Port"), 2000);
						if (m_pIVision->ConnectServer(strIp.c_str(), iPort))
						{
							sStepInfo.SetSubStep(4);
						}
					}
				}
			}
			break;

		case 5:
			{
				//等待检测结果信号
				BYTE abyRecv[1024] = {0};
				if (m_pIVision && (m_pIVision->RecvData(abyRecv, 1024) > 0))
				{
					USES_CONVERSION;
					tstring strValue = _T("");
					strValue = A2T((char *)abyRecv);
					if (strValue == _T("拍照OK"))
					{
						RecordLog(strValue.c_str(), 2);
						++m_iCaptrueIndex;
						CloseOutPort(_T("移至拍照位1"));
						CloseOutPort(_T("移至拍照位2")); 
						CloseOutPort(_T("移至拍照位3"));
						CloseOutPort(_T("移至拍照位4"));
						sStepInfo.SetSubStep(2);
					}
				}
				else
				{
					if (sStepInfo.IsTimeOut(10000))
					{
						m_pIVision->DisConnect();
						Sleep(300);
						tstring strIp = m_classIniConfig.ReadString(_T("Vision"), _T("Ip"), _T("127.0.0.1"));
						int iPort = m_classIniConfig.ReadInt(_T("Vision"), _T("Port"), 2000);
						if (m_pIVision->ConnectServer(strIp.c_str(), iPort))
						{
							sStepInfo.SetSubStep(4);
							sStepInfo.UpdataStTime();
						}
						
					}
				}

// 				++m_iCaptrueIndex;
// 				sStepInfo.SetSubStep(2);
			}
			break;

		case 6:
			{
				//发送到取板位信号
				if (CloseOutPort(_T("移至拍照位1")) && CloseOutPort(_T("移至拍照位2")) 
					&& CloseOutPort(_T("移至拍照位3")) && CloseOutPort(_T("移至拍照位4"))
					&& OpenOutPort(_T("移至取料位")))
				{
					sStepInfo.SetSubStep(7);
					sStepInfo.UpdataStTime();
				}	
			}
			break;

		case 7:
			{
				BYTE abyRecv[1024] = {0};
				if (m_pIVision && (m_pIVision->RecvData(abyRecv, 1024) > 0))
				{
					USES_CONVERSION;
					tstring strValue = _T("");
					strValue = A2T((char *)abyRecv);
					RecordLog(strValue.c_str(), 2);
					TransResult(abyRecv, 0);
					sStepInfo.SetSubStep(8);

					int iFailCount = 0;
					int iIndex = 0;
					BOOL bNoCode = FALSE;
					iFailCount = m_classIniConfig.ReadInt(_T("Yeild"), _T("FailCount"), 3);

					for (int i = 0; i < m_iProductCol * m_iProductRow; ++i)
					{
						if (m_aiResultTemp[i] == 0)
						{
							++iIndex;
						}
						else if (m_aiResultTemp[i] == 3)
						{
							bNoCode = TRUE;
						}
					}

					if (bNoCode)
					{
						SetThreeLight(L_RED);
						SendMsg(WM_ULMSG_WARNING, 33, 0);
						SetThreeLight(L_GRN);
					}

					if (iIndex >= iFailCount)
					{
						SetThreeLight(L_RED);
						SendMsg(WM_ULMSG_WARNING, 29, 0);
						SetThreeLight(L_GRN);
					}

				}
				else
				{
					if (sStepInfo.IsTimeOut(10000))
					{
						AlarmSetStep(sStepInfo, sStepInfo.GetSubStep(), 8);
						for (int i = 0; i < 30; ++i)
						{
							m_aiResultTemp[i] = 3;
						}
						sStepInfo.UpdataStTime();


					}
					
				}
				
			
			}
			break;

		case 8:
			{
				//设置允许取板信号
				if ((ReadInPort(_T("已到取料位")) == 1) && CloseOutPort(_T("移至取料位")))
				{
					m_bAllPick =  TRUE;
					SendMsg(WM_ULMSG_UPDATARST, NULL, NULL);
					sStepInfo.SetSubStep(9);
				}
				
			}
			break;

		case 9:
			{
				//等待取完信号
				if (!m_bAllPick && OpenOutPort(_T("取料完成")))
				{
					sStepInfo.SetSubStep(10);
				}
			}
			break;

		case 10:
			{
				iRet = ReadInPort(_T("分板机准备完成"));

				if (iRet == 0)
				{
					sStepInfo.SetSubStep(11);
				}
			}
			break;

		case 11:
			{
				if (CloseOutPort(_T("取料完成")))
				{
					sStepInfo.SetSubStep(0);
				}
			}
			break;
		}

		m_classIniConfig.WriteInt(_T("StepInfo"), _T("InTrack"), sStepInfo.GetSubStep());
		m_classIniConfig.WriteInt(_T("StepInfo"), _T("CaptrueIndex"), m_iCaptrueIndex);
	}

	void CWorkFlow::GetAllIOStatus()
	{
		if (m_classGT.IsInit())
		{
			BOOL bRet = TRUE;
			for (int i = 0; i < 5; ++i)
			{
				if ((m_classGT.ReadMulitInPort(m_alInValue[i], i) != 1) || (m_classGT.ReadMulitOutPort(m_alOutValue[i], i) != 1))
				{
					bRet = FALSE;
				}
			}
		}
	}

	int CWorkFlow::GetIOStatus(LPCTSTR lpstrName, LPCTSTR lpstrType)
	{
		int iRet = -1;
		tstring strType = _T("OutPort");
		if (m_classGT.IsInit())
		{
			if (strType == lpstrType)
			{
				if (m_mapPlcInfo[_T("OutPort")].find(lpstrName) != m_mapPlcInfo[_T("OutPort")].end())
				{
					if (m_alOutValue[m_mapPlcInfo[_T("OutPort")][lpstrName].iPortNo] & (1 << m_mapPlcInfo[_T("OutPort")][lpstrName].iHexNo))
					{
						iRet = 0;
					}
					else
					{
						iRet = 1;
					}
				}
			}
			else
			{
				if (m_mapPlcInfo[_T("InPort")].find(lpstrName) != m_mapPlcInfo[_T("InPort")].end())
				{
					if (m_alInValue[m_mapPlcInfo[_T("InPort")][lpstrName].iPortNo] & (1 << m_mapPlcInfo[_T("InPort")][lpstrName].iHexNo))
					{
						iRet = 0;
					}
					else
					{
						iRet = 1;
					}
				}
			}
		}

		return iRet;
	}

	BOOL CWorkFlow::SetThreeLight(const int iLight)
	{
		BOOL bRet = TRUE;
		switch (iLight)
		{
		case L_ALLCLOSE:
			{
				bRet &= CloseOutPort(_T("红灯"));
				bRet &= CloseOutPort(_T("黄灯"));
				bRet &= CloseOutPort(_T("绿灯"));
				bRet &= CloseOutPort(_T("蜂鸣"));
			}
			break;

		case L_ALLOPEN:
			{
				bRet &= OpenOutPort(_T("红灯"));
				bRet &= OpenOutPort(_T("黄灯"));
				bRet &= OpenOutPort(_T("绿灯"));
				bRet &= OpenOutPort(_T("蜂鸣"));
			}
			break;

		case L_RED:
			{
				bRet &= CloseOutPort(_T("黄灯"));
				bRet &= CloseOutPort(_T("绿灯"));
				bRet &= OpenOutPort(_T("蜂鸣"));
				bRet &= OpenOutPort(_T("红灯"));
			}
			break;

		case L_YEL:
			{				
				bRet &= CloseOutPort(_T("绿灯"));
				bRet &= CloseOutPort(_T("蜂鸣"));
				bRet &= CloseOutPort(_T("红灯"));
				bRet &= OpenOutPort(_T("黄灯"));
			}
			break;

		case L_GRN:
			{
				bRet &= CloseOutPort(_T("蜂鸣"));
				bRet &= CloseOutPort(_T("红灯"));
				bRet &= CloseOutPort(_T("黄灯"));
				bRet &= OpenOutPort(_T("绿灯"));
			}
			break;

		default:
			{
				bRet = FALSE;
			}
			break;
		}
		return bRet;
	}


	int CWorkFlow::GetPosCount()
	{
		int iCount = 0;
		iCount = m_vecPosIndex.size();
		return iCount;
	}

	BOOL CWorkFlow::GetPosValue(const int iIndex, LPTSTR lpstrName, double &dXPos, double &dYPos, double &dZPos, double &dUPos)
	{
		BOOL bRet = FALSE;
		
		if (iIndex < m_vecPosIndex.size())
		{
			if (m_mapRobotPosInfo.find(m_vecPosIndex[iIndex]) != m_mapRobotPosInfo.end())
			{
				dXPos = m_mapRobotPosInfo[m_vecPosIndex[iIndex]].dXPos;
				dYPos = m_mapRobotPosInfo[m_vecPosIndex[iIndex]].dYPos;
				dZPos = m_mapRobotPosInfo[m_vecPosIndex[iIndex]].dZPos;
				dUPos = m_mapRobotPosInfo[m_vecPosIndex[iIndex]].dUPos;
				_tcscpy(lpstrName, m_vecPosIndex[iIndex].c_str());
				bRet = TRUE;
			}
			else
			{
				if (m_mapProductPosInfo.find(m_vecPosIndex[iIndex]) != m_mapProductPosInfo.end())
				{
					dXPos = m_mapProductPosInfo[m_vecPosIndex[iIndex]].dXPos;
					dYPos = m_mapProductPosInfo[m_vecPosIndex[iIndex]].dYPos;
					dZPos = m_mapProductPosInfo[m_vecPosIndex[iIndex]].dZPos;
					dUPos = m_mapProductPosInfo[m_vecPosIndex[iIndex]].dUPos;
					_tcscpy(lpstrName, m_vecPosIndex[iIndex].c_str());
					bRet = TRUE;
				}
			}
		}
		
		return bRet;
	}

	BOOL CWorkFlow::GetIOName(const int iIndex, LPCTSTR lpstrType, LPTSTR lpstrName)
	{
		BOOL bRet = FALSE;

		if (m_mvIOName.find(lpstrType) != m_mvIOName.end())
		{
			if (m_mvIOName[lpstrType].size() > iIndex)
			{
				_tcscpy(lpstrName, m_mvIOName[lpstrType][iIndex].c_str());
				bRet = TRUE;
			}			
		}

		return bRet;
	}

	int CWorkFlow::GetIOCount(LPCTSTR lpstrType)
	{
		int iCount = 0; 
		if (m_mvIOName.find(lpstrType) != m_mvIOName.end())
		{
			iCount = m_mvIOName[lpstrType].size();		
		}

		return iCount;
	}

	void CWorkFlow::TrayNR()
	{
		m_bTrayNRun = TRUE;
		StepInfo sSubStep;
		sSubStep.Init();
		while (m_bTrayNRun)
		{
			if (GetIOStatus(_T("停止"), _T("InPort")) == 1)
			{
				m_classGT.AxisStop(1);
				m_classGT.AxisStop(2);
				m_bTrayNRun = FALSE;
				continue;
			}
			else
			{
				F_PassTray(sSubStep);
			}

			Sleep(10);
		}
	}

	BOOL CWorkFlow::SetEOpenPos()
	{
		long lPos = 0;
		BOOL bRet = FALSE;

		if (m_classGT.GetCurPos(2, lPos))
		{
			m_classIniProduct.WriteInt(_T("GTPos"), _T("EOpenPos"), lPos);
			bRet = TRUE;
		}

		return bRet;
	}

	BOOL CWorkFlow::MoveToEOpenPos()
	{
		long lPos = 0;
		BOOL bRet = FALSE;
		lPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("EOpenPos"), 0);
		bRet = AxisMovePos(2, lPos);
		return bRet;
	}

	BOOL CWorkFlow::SetFOpenPos()
	{
		long lPos = 0;
		BOOL bRet = FALSE;

		if (m_classGT.GetCurPos(1, lPos))
		{
			m_classIniProduct.WriteInt(_T("GTPos"), _T("FOpenPos"), lPos);
			bRet = TRUE;
		}

		return bRet;
	}

	BOOL CWorkFlow::MoveToFOpenPos()
	{
		long lPos = 0;
		BOOL bRet = FALSE;
		lPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("FOpenPos"), 0);
		bRet = AxisMovePos(1, lPos);
		return bRet;
	}

	BOOL CWorkFlow::SetECheckPos()
	{
		long lPos = 0;
		BOOL bRet = FALSE;

		if (m_classGT.GetCurPos(2, lPos))
		{
			m_classIniProduct.WriteInt(_T("GTPos"), _T("ECheckPos"), lPos);
			bRet = TRUE;
		}

		return bRet;
	}

	BOOL CWorkFlow::MoveToECheckPos()
	{
		long lPos = 0;
		BOOL bRet = FALSE;
		lPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("ECheckPos"), 0);
		bRet = AxisMovePos(2, lPos);
		return bRet;
	}

	BOOL CWorkFlow::SetFCheckPos()
	{
		long lPos = 0;
		BOOL bRet = FALSE;

		if (m_classGT.GetCurPos(1, lPos))
		{
			m_classIniProduct.WriteInt(_T("GTPos"), _T("FCheckPos"), lPos);
			bRet = TRUE;
		}

		return bRet;
	} 

	BOOL CWorkFlow::MoveToFCheckPos()
	{
		long lPos = 0;
		BOOL bRet = FALSE;
		lPos = m_classIniProduct.ReadInt(_T("GTPos"), _T("FCheckPos"), 0);
		bRet = AxisMovePos(1, lPos);
		return bRet;
	}

	BOOL CWorkFlow::SetFFullPos()
	{
		long lPos = 0;
		BOOL bRet = FALSE;

		if (m_classGT.GetCurPos(1, lPos))
		{
			m_classIniProduct.WriteInt(_T("GTPos"), _T("FFullPos"), lPos);
			bRet = TRUE;
		}

		return bRet;
	}

	BOOL CWorkFlow::SetEEmptyPos()
	{
		long lPos = 0;
		BOOL bRet = FALSE;

		if (m_classGT.GetCurPos(1, lPos))
		{
			m_classIniProduct.WriteInt(_T("GTPos"), _T("EEmptyPos"), lPos);
			bRet = TRUE;
		}

		return bRet;
	}

	int CWorkFlow::GetClassType()
	{
		return m_classIniConfig.ReadInt(_T("Class"), _T("ClassType"), 0);
	}

	
	//////////////////////////////////////////////////////////////////////////
	//UPH
	int CWorkFlow::GetHourPassCount()
	{
		if (m_pIUphRecord)
		{
			return (int)m_pIUphRecord->GetHourPassCount();
		}
		else
		{
			return 0;
		}
	}

	int CWorkFlow::GetHourFailCount()
	{
		if (m_pIUphRecord)
		{
			return (int)m_pIUphRecord->GetHourFailCount();
		}
		else
		{
			return 0;
		}
	}

	double CWorkFlow::GetHourYeild()
	{
		if (m_pIUphRecord)
		{
			double dYeild = 100.0;
			int iFailCount = 0;
			int iPassCount = 0;

			iFailCount = m_pIUphRecord->GetHourFailCount();
			iPassCount = m_pIUphRecord->GetHourPassCount();

			if (iPassCount + iFailCount > 0)
			{
				dYeild = ((double)(iPassCount)) / ((double)(iPassCount + iFailCount)) * 100.0;
			}

			return dYeild;
		}
		else
		{
			return 0;
		}
	}

	int CWorkFlow::GetDayPassCount()
	{
		if (m_pIUphRecord)
		{
			return m_iClassPsCnt;
//			return (int)m_pIUphRecord->GetDayPassCount();
		}
		else
		{
			return 0;
		}
	}

	int CWorkFlow::GetHourOtherCount()
	{
		if (m_pIUphRecord)
		{
			return (int)m_pIUphRecord->GetHourOtherCount();
		}
		else
		{
			return 0;
		}
	}

	int CWorkFlow::GetDayOtherCount()
	{
		if (m_pIUphRecord)
		{
//			return (int)m_pIUphRecord->GetDayOtherCount();
			return m_iClassNcCnt;
		}
		else
		{
			return 0;
		}
	}

	int CWorkFlow::GetDayFailCount()
	{
		if (m_pIUphRecord)
		{
//			return (int)m_pIUphRecord->GetDayFailCount();
			return m_iClassNgCnt;
		}
		else
		{
			return 0;
		}
	}

	double CWorkFlow::GetDayYeild()
	{
		if (m_pIUphRecord)
		{
			//return m_pIUphRecord->GetDayYeild();
			double dYeild = 100.0;
			int iFailCount = 0;
			int iPassCount = 0;

// 			iFailCount = m_pIUphRecord->GetDayFailCount();
// 			iPassCount = m_pIUphRecord->GetDayPassCount();

			iFailCount = m_iClassNgCnt;
			iPassCount = m_iClassPsCnt;

			if (iPassCount + iFailCount > 0)
			{
				dYeild = ((double)(iPassCount)) / ((double)(iPassCount + iFailCount)) * 100.0;
			}

			return dYeild;
		}
		else
		{
			return 0;
		}
	}

	void CWorkFlow::AddPsCnt()
	{
		if (m_pIUphRecord)
		{
			m_pIUphRecord->AddHourPassCount();
		}

		m_csClass.Lock();
		++m_iClassPsCnt;
		m_classIniConfig.WriteInt(_T("Class"), _T("ClassPsCnt"), m_iClassPsCnt);
		m_csClass.Unlock();

		PostMsg(WM_ULMSG_PUTOKP0S, m_iPutPassPos, NULL);
	}

	void CWorkFlow::AddFiCnt()
	{
		if (m_pIUphRecord)
		{
			m_pIUphRecord->AddHourFailCount();
		}

		m_csClass.Lock();
		++m_iClassNgCnt;
		m_classIniConfig.WriteInt(_T("Class"), _T("ClassNgCnt"), m_iClassNgCnt);
		m_csClass.Unlock();

		PostMsg(WM_ULMSG_PUTNGP0S, m_iPutFailPos, NULL);

		if (m_classIniConfig.ReadDouble(_T("Yeild"), _T("Spec"), 85.0) > GetHourYeild())
		{
			if ((m_pIUphRecord->GetHourPassCount() + m_pIUphRecord->GetHourFailCount()) > m_classIniConfig.ReadInt(_T("Yeild"), _T("NoStopCount"), 20))
			{
				SetThreeLight(L_RED);
				//			ViewMsg(ALARM_UPHYIELDLOW);
				SendMsg(WM_ULMSG_WARNING, ALARM_UPHYIELDLOW, 0);
				SetThreeLight(L_GRN);
			}
			else
			{
				SetThreeLight(L_RED);
				PostMsg(WM_ULMSG_INFORMATION, INFO_UPHLOWTARGET, 0);
			}

		}
		else if (m_classIniConfig.ReadDouble(_T("Yeild"), _T("Target"), 90.0) > GetHourYeild())
		{
			SetThreeLight(L_RED);
			PostMsg(WM_ULMSG_INFORMATION, INFO_UPHLOWTARGET, 0);
//			SetThreeLight(L_GRN);
		}
	}

	void CWorkFlow::AddOtherCnt()
	{
		if (m_pIUphRecord)
		{
			m_csClass.Lock();
			++m_iClassNcCnt;
			m_classIniConfig.WriteInt(_T("Class"), _T("ClassNcCnt"), m_iClassNcCnt);
			m_csClass.Unlock();
			m_pIUphRecord->AddHourOtherCount();
		}

		PostMsg(WM_ULMSG_PUTNGP0S, m_iPutFailPos, NULL);
	}
	//UPH
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//线程
	void CWorkFlow::InTrack()
	{
		while (m_bThread)
		{
			if (m_bStart)
			{
				if (m_bSuspend)
				{
					Sleep(100);
					continue;;
				}

				F_InTrack(m_sInTrackFlow);
				Sleep(10);
			}
			else
			{
				Sleep(500);
			}
		}
	}

	void CWorkFlow::FailTray()
	{
		while (m_bThread)
		{
			if (m_bStart)
			{
				if (m_bSuspend)
				{
					Sleep(100);
					continue;;
				}

				F_FailTray(m_sFailTrayFlow);
				Sleep(10);
			}
			else
			{
				Sleep(500);
			}
		}
	}

	void CWorkFlow::PassTray()
	{
		m_sEmptyTrayFlow.Init();
		StepInfo sStep;
		sStep.Init();
		while (m_bThread)
		{
			if (m_bStart)
			{
				if (m_bSuspend)
				{
					Sleep(100);
					continue;;
				}

				//F_PassTray(m_sPassTrayFlow);
				F_EmptyTray(m_sEmptyTrayFlow);
				F_FullTray(sStep);
				Sleep(1);
			}
			else
			{
				Sleep(500);
			}
		}
	}

	void CWorkFlow::PickAndPut()
	{
		while (m_bThread)
		{
			if (m_bStart)
			{
				if (m_bSuspend)
				{
					Sleep(100);
					continue;;
				}

				F_PickAndPut(m_sPickAndPutFlow);
				Sleep(10);
			}
			else
			{
				GetAllIOStatus();
				Sleep(500);
			}
		}
	}

	void CWorkFlow::Alarm()
	{
		BOOL bSuspend = FALSE;
		int iReset = 0;
		int iStart = 0;
		int iStop = 0;
		int iSuspend = 0;
		int iSafeDoor = 0;
		int iAxisAlarm = 1;
		int iEmergencyStop = 0;

		while (m_bThread)
		{
			iReset = m_classIniConfig.ReadInt(_T("HotButton"), _T("Reset"), 1);
			iStart = m_classIniConfig.ReadInt(_T("HotButton"), _T("Start"), 1);
			iStop = m_classIniConfig.ReadInt(_T("HotButton"), _T("Stop"), 1);
			iSuspend = m_classIniConfig.ReadInt(_T("HotButton"), _T("Suspend"), 0);
			iSafeDoor = m_classIniConfig.ReadInt(_T("HotButton"), _T("SafeDoor"), 1);
			iAxisAlarm = m_classIniConfig.ReadInt(_T("HotButton"), _T("AxisAlarm"), 3);
			iEmergencyStop = m_classIniConfig.ReadInt(_T("HotButton"), _T("EmergencyStop"), 1);


			if (GetIOStatus(_T("复位"), _T("InPort")) == iReset)
			{
				if (m_bStart)
				{
					if (!IsSuspend())
					{
						SetThreeLight(L_GRN);
					}	
				}
// 				else
// 				{
// 					SendMsg(WM_ULMSG_RESET, NULL, NULL);
// 				}
			}
			else if (GetIOStatus(_T("暂停"), _T("InPort")) == iSuspend)
			{
				if (m_bStart)
				{
					if (!IsSuspend())
					{
						Suspend();
					}

					SendMsg(WM_ULMSG_SUSPEND, NULL, NULL);
				}
			}
// 			else if (GetIOStatus(_T("启动"), _T("InPort")) == 1)
// 			{
// 				if (!m_bStart)
// 				{
// 					SendMsg(WM_ULMSG_START, NULL, NULL);
// 				}
// 				else
// 				{
// 					if (!IsSuspend())
// 					{
// 						SetThreeLight(L_GRN);
// 					}
// 					else
// 					{
// 						Resume();
// 					}
// 				}
// 			}
			else if (GetIOStatus(_T("停止"), _T("InPort")) == iStop)
			{
				if (m_bReset)
				{
					SendMsg(WM_ULMSG_STOP, NULL, NULL);
				}				
			}
			else if (GetIOStatus(_T("安全门"), _T("InPort")) == iSafeDoor)
			{
				if (m_bStart)
				{
// 					m_bEngSuspend = TRUE;
// 					m_pIRobot->SuspendRobot();
// 					SendMsg(WM_ULMSG_WARNING, ALARM_SAFEDOOR, NULL);
// 					if (GetIOStatus(_T("安全门"), _T("InPort")) == 1)
// 					{
// 						m_bEngSuspend = FALSE;
// 						if (!IsSuspend())
// 						{
// 							m_pIRobot->ResumeRobot();
// 						}
// 						
// 					}

					Suspend();
					SetThreeLight(L_RED);
					SendMsg(WM_ULMSG_WARNING, ALARM_SAFEDOOR, NULL);
					Resume();
				}
			}
			else if (GetIOStatus(_T("急停"), _T("InPort")) == iStop)
			{
				if (m_bReset || m_bStart)
				{
					SendMsg(WM_ULMSG_STOP, NULL, NULL);
				}

				SetThreeLight(L_RED);
				SendMsg(WM_ULMSG_WARNING, 34, NULL);
				SetThreeLight(L_YEL);
			}
// 			else if (GetIOStatus(_T("NG门禁"), _T("InPort")) == 0)
// 			{
// 				if (m_bStart)
// 				{
// 					m_bEngSuspend = TRUE;
// 					m_pIRobot->SuspendRobot();
// 					SendMsg(WM_ULMSG_WARNING, ALARM_NGDOOR, NULL);
// 					if (GetIOStatus(_T("NG门禁"), _T("InPort")) == 1)
// 					{
// 						m_bEngSuspend = FALSE;
// 						if (!IsSuspend())
// 						{
// 							m_pIRobot->ResumeRobot();
// 						}
// 					}

// 					Suspend();
// 					SendMsg(WM_ULMSG_WARNING, ALARM_NGDOOR, NULL);
// 					Resume();
// 				}
// 			}

			if (m_classGT.GetAxisAlarm(1) == iAxisAlarm) 
			{
				
				if (m_bStart)
				{
					SendMsg(WM_ULMSG_STOP, NULL, NULL);
				}

				SetThreeLight(L_RED);
				SendMsg(WM_ULMSG_WARNING, 30, NULL);
				SetThreeLight(L_YEL);
			}

			if (m_classGT.GetAxisAlarm(2) == iAxisAlarm)
			{
				
				if (m_bStart)
				{
					SendMsg(WM_ULMSG_STOP, NULL, NULL);
				}

				SetThreeLight(L_RED);
				SendMsg(WM_ULMSG_WARNING, 30, NULL);
				SetThreeLight(L_YEL);
			}

			SYSTEMTIME sysTime;
			GetLocalTime(&sysTime);

			if (m_classIniConfig.ReadInt(_T("Class"), _T("ClassType"), 0) == 0)
			{
				if (sysTime.wHour == 20)
				{
					if (sysTime.wMinute > 30)
					{
						m_csClass.Lock();
						m_iClassPsCnt = 0;
						m_iClassNgCnt = 0;
						m_iClassNcCnt = 0;
						m_iClassType = 1;
						m_classIniConfig.WriteInt(_T("Class"), _T("ClassPsCnt"), m_iClassPsCnt);
						m_classIniConfig.WriteInt(_T("Class"), _T("ClassType"), m_iClassType);
						m_classIniConfig.WriteInt(_T("Class"), _T("ClassNgCnt"), m_iClassNgCnt);
						m_classIniConfig.WriteInt(_T("Class"), _T("ClassNcCnt"), m_iClassNcCnt);
						m_csClass.Unlock();
					}
				}
				else if (sysTime.wHour > 20 || sysTime.wHour < 8)
				{
					m_csClass.Lock();
					m_iClassPsCnt = 0;
					m_iClassNgCnt = 0;
					m_iClassNcCnt = 0;
					m_iClassType = 1;
					m_classIniConfig.WriteInt(_T("Class"), _T("ClassPsCnt"), m_iClassPsCnt);
					m_classIniConfig.WriteInt(_T("Class"), _T("ClassType"), m_iClassType);
					m_classIniConfig.WriteInt(_T("Class"), _T("ClassNgCnt"), m_iClassNgCnt);
					m_classIniConfig.WriteInt(_T("Class"), _T("ClassNcCnt"), m_iClassNcCnt);
					m_csClass.Unlock();
				}
			}
			else
			{
				if (sysTime.wHour == 8)
				{
					if (sysTime.wMinute > 30)
					{
						m_csClass.Lock();
						m_iClassPsCnt = 0;
						m_iClassNgCnt = 0;
						m_iClassNcCnt = 0;
						m_iClassType = 0;
						m_classIniConfig.WriteInt(_T("Class"), _T("ClassPsCnt"), m_iClassPsCnt);
						m_classIniConfig.WriteInt(_T("Class"), _T("ClassType"), m_iClassType);
						m_classIniConfig.WriteInt(_T("Class"), _T("ClassNgCnt"), m_iClassNgCnt);
						m_classIniConfig.WriteInt(_T("Class"), _T("ClassNcCnt"), m_iClassNcCnt);
						m_csClass.Unlock();
					}
				}
				else if (sysTime.wHour > 8 && sysTime.wHour < 20)
				{
					m_csClass.Lock();
					m_iClassPsCnt = 0;
					m_iClassNgCnt = 0;
					m_iClassNcCnt = 0;
					m_iClassType = 0;
					m_classIniConfig.WriteInt(_T("Class"), _T("ClassPsCnt"), m_iClassPsCnt);
					m_classIniConfig.WriteInt(_T("Class"), _T("ClassType"), m_iClassType);
					m_classIniConfig.WriteInt(_T("Class"), _T("ClassNgCnt"), m_iClassNgCnt);
					m_classIniConfig.WriteInt(_T("Class"), _T("ClassNcCnt"), m_iClassNcCnt);
					m_csClass.Unlock();
				}
			}
			
			Sleep(100);
		}
	}

	void CWorkFlow::WorkFlow()
	{
		StepInfo *pStepInfo = NULL;
		while (m_bThread)
		{
			if (m_bStart)
			{
				m_csVector.Lock();
				if (!m_vecStepInfo.empty())
				{			
					pStepInfo = m_vecStepInfo[0];
					m_vecStepInfo.erase(m_vecStepInfo.begin());
				}
				else
				{
					pStepInfo = NULL;
				}
				m_csVector.Unlock();

				if (pStepInfo && m_bSuspend && !pStepInfo->IsNoSupsend()/*| m_bEngSuspend*/)
				{
					Sleep(100);
					m_csVector.Lock();
					pStepInfo->UpdataStTime();
					m_vecStepInfo.push_back(pStepInfo);
					m_csVector.Unlock();
					continue;
				}
				
				if (pStepInfo)
				{
					switch (pStepInfo->GetMainStep())
					{
					case STEP_PICKANDPUT:
						{
							F_PickAndPut(*pStepInfo);
						}
						break;

					case STEP_EMPTTRAY:
						{
							F_EmptyTray(*pStepInfo);
						}
						break;

					case STEP_FAILTRAY:
						{
							F_FailTray(*pStepInfo);
						}
						break;

					case STEP_INTRACK:
						{
							F_InTrack(*pStepInfo);
						}
						break;

					case STEP_FULLTRAY:
						{
							F_FullTray(*pStepInfo);
						}
						break;

					case STEP_PASSRAY:
						{
							F_PassTray(*pStepInfo);
						}
						break;
					}

					m_csVector.Lock();
					m_vecStepInfo.push_back(pStepInfo);
					m_csVector.Unlock();
				}
		
				Sleep(10);				

			}
			else
			{
				Sleep(1000);
			}
		}
	}

	void CWorkFlow::Signal()
	{
		while (m_bThread)
		{
			GetAllIOStatus();
			Sleep(10);
		}
	}
	//线程
	//////////////////////////////////////////////////////////////////////////

	//Robot
	BOOL CWorkFlow::GotoDistance(const LPCTSTR lpstrAxis, double dDistance, const int iMoveMode /* = 0 */)
	{
		BOOL bRet = FALSE;
		if (m_pIRobot)
		{
			bRet = m_pIRobot->GotoDistance(lpstrAxis, dDistance, iMoveMode);
		}

		return bRet;
	}

	BOOL CWorkFlow::IsRobotBusy(DWORD dwTimeOut /* = 20000 */)
	{
		BOOL bRet = FALSE;

		if (m_pIRobot)
		{
			bRet = m_pIRobot->IsRobotBusy(dwTimeOut);
		}

		return bRet;
	}

	BOOL CWorkFlow::JumpPos(const int iPosIndex, const double dZHight /* = 600 */)
	{
		BOOL bRet = FALSE;

		if (m_pIRobot)
		{
			bRet = m_pIRobot->GotoPos(iPosIndex, 0.0);
		}

		return bRet;
	}


	BOOL CWorkFlow::JumpPosMod(RobotPosInfo sRobotPosInfo, const double dZHight /* = 600.0 */)
	{
		BOOL bRet = FALSE;

		if (m_pIRobot)
		{
			bRet = m_pIRobot->JumpPosMod(sRobotPosInfo, 0.0);
		}

		return bRet;
	}

	BOOL CWorkFlow::QryCurPos(double &dXPos, double &dYPos, double &dZPos, double &dUPos)
	{
		BOOL bRet = FALSE;

		if (m_pIRobot)
		{
			RobotPosInfo sRobotPosInfo;
			if (m_pIRobot->QryCurPos(sRobotPosInfo) == 1)
			{
				bRet = TRUE;
			}
			dXPos = sRobotPosInfo.dXPos;
			dYPos = sRobotPosInfo.dYPos;
			dZPos = sRobotPosInfo.dZPos;
			dUPos = sRobotPosInfo.dUPos;
		}

		return bRet;
	}

	BOOL CWorkFlow::StorePos(LPCTSTR lpstrPosName)
	{
		BOOL bRet = FALSE;

		if (m_mapRobotPosInfo.find(lpstrPosName) != m_mapRobotPosInfo.end())
		{
			RobotPosInfo sRobotPos;
			if (m_pIRobot && (m_pIRobot->QryCurPos(sRobotPos) == 1))
			{
				CIniFile classIni;
				TCHAR atcPosData[128] = {0};
				classIni.SetIniFile((m_strConfigDir + _T("Position.dat")).c_str());
				swprintf(atcPosData, _T("%.3f,%.3f,%.3f,%.3f,"), sRobotPos.dXPos, sRobotPos.dYPos, sRobotPos.dZPos, sRobotPos.dUPos);
				classIni.WriteString(RPOSITION, lpstrPosName, atcPosData);
				m_mapRobotPosInfo[lpstrPosName] = sRobotPos;
				bRet = TRUE;
			}
		}
		else if (m_mapProductPosInfo.find(lpstrPosName) != m_mapProductPosInfo.end())
		{
			RobotPosInfo sRobotPos;
			if (m_pIRobot && (m_pIRobot->QryCurPos(sRobotPos) == 1))
			{
				CIniFile classIni;
				TCHAR atcPosData[128] = {0};
				classIni.SetIniFile((m_strProductDir + m_strProductName  + _T("\\Position.dat")).c_str());
				swprintf(atcPosData, _T("%.3f,%.3f,%.3f,%.3f,"), sRobotPos.dXPos, sRobotPos.dYPos, sRobotPos.dZPos, sRobotPos.dUPos);
				classIni.WriteString(RPOSITION, lpstrPosName, atcPosData);
				m_mapProductPosInfo[lpstrPosName] = sRobotPos;
				bRet = TRUE;
			}
		}

		return bRet;
	}

	BOOL CWorkFlow::StorePos(LPCTSTR lpstrPosName, const double dXPos, const double dYPos, const double dZPos, const double dUPos)
	{
		BOOL bRet = FALSE;
		RobotPosInfo sRobotPos;
		sRobotPos.dXPos = dXPos;
		sRobotPos.dYPos = dYPos;
		sRobotPos.dZPos = dZPos;
		sRobotPos.dUPos = dUPos;

		if (m_mapRobotPosInfo.find(lpstrPosName) != m_mapRobotPosInfo.end())
		{	
			CIniFile classIni;
			TCHAR atcPosData[128] = {0};
			classIni.SetIniFile((m_strConfigDir + _T("Position.dat")).c_str());
			swprintf(atcPosData, _T("%.3f,%.3f,%.3f,%.3f,"), sRobotPos.dXPos, sRobotPos.dYPos, sRobotPos.dZPos, sRobotPos.dUPos);
			classIni.WriteString(RPOSITION, lpstrPosName, atcPosData);
			m_mapRobotPosInfo[lpstrPosName] = sRobotPos;
			bRet = TRUE;		
		}
		else if (m_mapProductPosInfo.find(lpstrPosName) != m_mapProductPosInfo.end())
		{
			CIniFile classIni;
			TCHAR atcPosData[128] = {0};
			classIni.SetIniFile((m_strProductDir + m_strProductName  + _T("\\Position.dat")).c_str());
			swprintf(atcPosData, _T("%.3f,%.3f,%.3f,%.3f,"), sRobotPos.dXPos, sRobotPos.dYPos, sRobotPos.dZPos, sRobotPos.dUPos);
			classIni.WriteString(RPOSITION, lpstrPosName, atcPosData);
			m_mapProductPosInfo[lpstrPosName] = sRobotPos;
			bRet = TRUE;

		}

		return bRet;
	}

	BOOL CWorkFlow::JumpPos(LPCTSTR lpstrPosName)
	{
		BOOL bRet = FALSE;
		RobotPosInfo sRobotPos;
		if (m_mapRobotPosInfo.find(lpstrPosName) != m_mapRobotPosInfo.end())
		{		
			if (m_pIRobot )
			{
				sRobotPos = m_mapRobotPosInfo[lpstrPosName];
				bRet = TRUE;
			}
		}
		else if (m_mapProductPosInfo.find(lpstrPosName) != m_mapProductPosInfo.end())
		{
			if (m_pIRobot )
			{
				sRobotPos = m_mapProductPosInfo[lpstrPosName];
				bRet = TRUE;
			}
		}

		if (!IsRobotBusy(10))
		{
			bRet = m_pIRobot->JumpPosMod(sRobotPos, ROBOT_SAFEHIGH);
		}
		else
		{
			bRet = FALSE;
		}

		return bRet;
	}

	BOOL CWorkFlow::GotoPos(LPCTSTR lpstrPosName)
	{
		BOOL bRet = FALSE;
		RobotPosInfo sRobotPos;
		if (m_mapRobotPosInfo.find(lpstrPosName) != m_mapRobotPosInfo.end())
		{		
			if (m_pIRobot )
			{
				sRobotPos = m_mapRobotPosInfo[lpstrPosName];
				bRet = TRUE;
			}
		}
		else if (m_mapProductPosInfo.find(lpstrPosName) != m_mapProductPosInfo.end())
		{
			if (m_pIRobot )
			{
				sRobotPos = m_mapProductPosInfo[lpstrPosName];
				bRet = TRUE;
			}
		}

		if (!IsRobotBusy(10))
		{
			bRet = m_pIRobot->GotoPosMod(sRobotPos, ROBOT_SAFEHIGH);
		}
		else
		{
			bRet = FALSE;
		}

		return bRet;
	}

	int CWorkFlow::ChangeTool(const int iToolNo)
	{
		int iRet = 0;

		if (m_pIRobot)
		{
			iRet = m_pIRobot->ChangeTool(iToolNo);
		}

		return iRet;
	}

	BOOL CWorkFlow::IsRobotConnect()
	{
		BOOL bRet = FALSE;

		if (m_pIRobot)
		{
			bRet = m_pIRobot->IsRobotConnnect();
		}

		return bRet;
	}

	BOOL CWorkFlow::SetSpeed(const int iSpeed)
	{
		BOOL bRet = FALSE;

		if (m_pIRobot)
		{
			if (iSpeed > 20)
			{
				m_pIRobot->SetPower(TRUE);
			}
			else
			{
				m_pIRobot->SetPower(FALSE);
			}

			bRet = (m_pIRobot->SetSpeed(iSpeed, 120) == 1);
		}

		return bRet;
	}

	BOOL CWorkFlow::ConnectRobot()
	{
		BOOL bRet = FALSE;

		if (m_pIRobot)
		{
			TCHAR atcMsg[512] = {0};
			bRet = m_pIRobot->InitRobot(atcMsg);

			if (!bRet)
			{
				m_pIRobot->CloseConnect();
			}
		}

		return bRet;
	}

	BOOL CWorkFlow::DisConnectRobot()
	{
		BOOL bRet = FALSE;

		if (m_pIRobot)
		{
			m_pIRobot->CloseConnect();
			bRet = TRUE;
		}

		return bRet;
	}

	BOOL CWorkFlow::CalibCamera()
	{
		StepInfo sStep;
		sStep.Init();
		sStep.SetMainSubStep(0, 0);
		sStep.SetSubStep(0);
		while (sStep.GetMainStep() ==0 )
		{
			F_CalibCamera(sStep);
			if (GetIOStatus(_T("停止"), _T("InPort")) == 1)
			{
				MessageBox(m_hParaent, _T("检测到停止信号自动相机标定已停止"), _T("报警"), MB_OK);
				ChangeTool(0);
				return FALSE;
			}
			Sleep(10);
		}

		return TRUE;
	}

	BOOL CWorkFlow::CalibPickTool()
	{
		StepInfo sStep;
		sStep.Init();
		sStep.SetMainSubStep(0, 0);
		sStep.SetSubStep(0);
		while (sStep.GetMainStep() ==0 )
		{
			F_CreatePickTool(sStep);
			if (GetIOStatus(_T("停止"), _T("InPort")) == 1)
			{
				MessageBox(m_hParaent, _T("检测到停止信号自动吸头标定已停止"), _T("报警"), MB_OK);
				ChangeTool(0);
				return FALSE;
			}
			Sleep(10);
		}

		return TRUE;
	}

	BOOL CWorkFlow::CalibPCB(const int iModel)
	{
		F_CalibPutPos(iModel);
		return TRUE;
	}

	BOOL CWorkFlow::AxisMoveDis(const int iAxis, long lDistance)
	{
		BOOL bRet = FALSE;
		double dMoveSpeed = 20.0;
		double dMoveAcc = 0.1;
		double dMoveDec = 0.1;
		TCHAR atcSection[128] = {0};
		swprintf(atcSection, _T("Z%d"), iAxis);
		dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
		dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
		dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

		if (m_classGT.AxisMove(iAxis, lDistance, 1, dMoveSpeed, dMoveAcc, dMoveDec))
		{
			DWORD dwSTTime = ::GetTickCount();
			while ((::GetTickCount() - dwSTTime < 20000) && !bRet)
			{
				if (!m_classGT.AxisIsBusy(iAxis))
				{
					bRet = TRUE;
				}

				Sleep(100);
			}

			if (!bRet)
			{
				m_classGT.AxisStop(iAxis);
			}
		}

		return bRet;
	}

	BOOL CWorkFlow::AxisMovePos(const int iAxis, long lPos)
	{
		BOOL bRet = FALSE;
		double dMoveSpeed = 20.0;
		double dMoveAcc = 0.1;
		double dMoveDec = 0.1;
		TCHAR atcSection[128] = {0};
		swprintf(atcSection, _T("Z%d"), iAxis);
		dMoveSpeed = m_classIniConfig.ReadDouble(atcSection, _T("MoveSpeed"), 20.0);
		dMoveAcc = m_classIniConfig.ReadDouble(atcSection, _T("MoveAcc"), 0.1);
		dMoveDec = m_classIniConfig.ReadDouble(atcSection, _T("MoveDec"), 0.1);

		if (m_classGT.AxisMove(iAxis, lPos, 0, dMoveSpeed, dMoveAcc, dMoveDec))
		{
			DWORD dwSTTime = ::GetTickCount();
			while ((::GetTickCount() - dwSTTime < 20000) && !bRet)
			{
				if (!m_classGT.AxisIsBusy(iAxis))
				{
					long lCurPos = 0;
					
					if (m_classGT.GetCurPos(1, lCurPos))
					{
						if (abs(lPos - lCurPos) < 10)
						{
							bRet = TRUE;
						}
					}
					
				}

				Sleep(100);
			}

			if (!bRet)
			{
				m_classGT.AxisStop(iAxis);
			}
		}

		return bRet;
	}

	BOOL CWorkFlow::StoreTrayPos(const int iAxis)
	{
		BOOL bRet = FALSE;
		long lPos = 0;

		if (m_classGT.GetCurPos(iAxis, lPos))
		{
			bRet = TRUE;
			switch (iAxis)
			{
			case 1:
				{
					m_classIniProduct.WriteInt(_T("GTPos"), _T("FTrayPos"), lPos);
					m_lFTrayPos = lPos;
				}
				break;

			case 2:
				{
					m_classIniProduct.WriteInt(_T("GTPos"), _T("ETrayPos"), lPos);
					m_lETrayPos = lPos;
				}
				break;

			default: 
				{
					bRet = FALSE;
				}
				break;
			}
		}

		return bRet;

	}

	BOOL CWorkFlow::StoreOffSet(const int iAxis)
	{
		BOOL bRet = FALSE;
		long lPos = 0;


		if (m_classGT.GetCurPos(iAxis, lPos))
		{
			bRet = TRUE;
			switch (iAxis)
			{
			case 1:
				{					
					m_lFTrayOffset = lPos - m_lFSensorPos;
					m_classIniProduct.WriteInt(_T("GTPos"), _T("FTrayOffset"), m_lFTrayOffset);
				}
				break;

			case 2:
				{
					m_lETrayOffset = lPos - m_lESensorPos;
					m_classIniProduct.WriteInt(_T("GTPos"), _T("ETrayOffset"), m_lETrayOffset);
				}
				break;

			default: 
				{
					bRet = FALSE;
				}
				break;
			}
		}

		return bRet;
	}

	BOOL CWorkFlow::CalibTrayHavePos(const int iAxis)
	{
		BOOL bRet = FALSE;
		StepInfo sStep;
		sStep.Init();
		while (sStep.GetMainStep() == 0)
		{
			if (GetIOStatus(_T("停止"), _T("InPort")) == 1)
			{
				MessageBox(m_hParaent, _T("检测到停止信号自动Tray标定已停止"), _T("报警"), MB_OK);
				return FALSE;
			}

			switch (sStep.GetSubStep())
			{

			case 0:
				{
					switch (iAxis)
					{
					case 1:
						{
							int iCheckPos = m_classIniConfig.ReadInt(_T("GTPos"), _T("FCheckPos"), 100);
							if (m_classGT.AxisMove(1, iCheckPos, 0) )
							{
								sStep.SetSubStep(1);
							}
						}
						break;

					case 2:
						{
							int iCheckPos = m_classIniConfig.ReadInt(_T("GTPos"), _T("FCheckPos"), 100);
							if (m_classGT.AxisMove(2, iCheckPos, 0) )
							{
								sStep.SetSubStep(1);
							}
						}
						break;

					default:
						{
							sStep.SetMainSubStep(-1, 0);
						}
						break;
					}
				}
				break;

			case 1:
				{
					if (!m_classGT.AxisIsBusy(iAxis))
					{
						sStep.SetSubStep(2);
					}
				}
				break;

			case 2:
				{
					switch (iAxis)
					{
					case 1:
						{
							if (ReadInPort(_T("收料位料盘有无")) == 1)
							{
								sStep.SetSubStep(3);
							}
						}
						break;

					case 2:
						{
							if (ReadInPort(_T("供料位料盘有无")) == 1)
							{
								sStep.SetSubStep(3);
							}
						}
						break;

					default:
						{
							sStep.SetMainSubStep(-1, 0);
						}
						break;
					}
				}
				break;

			case 3:
				{
					double dJogSpeed = 20.0;
					double dJogAcc = 0.1;
					double dJogDec = 0.1;
					TCHAR atcSection[128] = {0};
					swprintf(atcSection, _T("Z%d"), iAxis);
					dJogSpeed = m_classIniConfig.ReadDouble(atcSection, _T("JogSpeed"), 20.0);
					dJogAcc = m_classIniConfig.ReadDouble(atcSection, _T("JogAcc"), 0.1);
					dJogDec = m_classIniConfig.ReadDouble(atcSection, _T("JogDec"), 0.1);

					if (m_classGT.JogMove(iAxis, TRUE, dJogSpeed, dJogAcc, dJogDec))
					{
						sStep.SetSubStep(4);
					}
				}
				break;

			case 4:
				{
					switch (iAxis)
					{
					case 1:
						{
							if (ReadInPort(_T("收料位料盘到位")) == 1)
							{
								sStep.SetSubStep(5);
							}
							else
							{
//								sStep.SetSubStep(3);
							}
						}
						break;

					case 2:
						{
							if (ReadInPort(_T("供料位料盘到位")) == 1)
							{
								sStep.SetSubStep(5);
							}
							else
							{
//								sStep.SetSubStep(3);
							}
						}
						break;

					default:
						{
							m_classGT.AxisStop(iAxis);
							sStep.SetMainSubStep(-1, 0);
						}
						break;
					}
				}
				break;

			case 5:
				{
					if (m_classGT.AxisStop(iAxis))
					{
						sStep.SetSubStep(6);
					}
				}
				break;

			case 6:
				{
					long lPos = 0;

					if (m_classGT.GetCurPos(iAxis, lPos))
					{
						switch (iAxis)
						{
						case 1:
							{
								m_lFSensorPos = lPos;
								m_classIniProduct.WriteInt(_T("GTPos"), _T("FSensorPos"), m_lFSensorPos);
								bRet = TRUE;
							}
							break;

						case 2:
							{
								m_lESensorPos = lPos;
								m_classIniProduct.WriteInt(_T("GTPos"), _T("ESensorPos"), m_lESensorPos);
								bRet = TRUE;
							}
							break;

						default:
							{
				
							}
							break;
						}
					}
					
					sStep.SetMainSubStep(-1, 0);
				}
				break;
			}
			Sleep(1);
		}

		return bRet;
	}

	long CWorkFlow::GetAxisPos(const int iAxis)
	{
		long lPos = 0;
		if (!m_classGT.GetCurPos(iAxis, lPos))
		{
			lPos = 0;
		}

		return lPos;
	}

	BOOL CWorkFlow::GoHome(const int iAxis)
	{
		BOOL bRet = FALSE;

		if (!m_bStart)
		{

			double dHomeSpeed = 10.0;
			double dHomeAcc = 0.03;
			double dHomeDec = 0.25;
			TCHAR atcSection[128] = {0};
			swprintf(atcSection, _T("Z%d"), iAxis);
			dHomeSpeed = m_classIniConfig.ReadDouble(atcSection, _T("HomeSpeed"), 10.0);
			dHomeAcc = m_classIniConfig.ReadDouble(atcSection, _T("HomeAcc"), 0.03);
			dHomeDec = m_classIniConfig.ReadDouble(atcSection, _T("HomeDec"), 0.25);

			bRet = m_classGT.GoHome(iAxis, dHomeSpeed, dHomeAcc, dHomeDec);
		}

		return bRet;
	}

	//Msg
	LRESULT CWorkFlow::SendMsg(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return ::SendMessage(m_hParaent, Msg, wParam, lParam);
	}

	LRESULT CWorkFlow::PostMsg(UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return ::PostMessage(m_hParaent, Msg, wParam, lParam);
	}

	LRESULT CWorkFlow::AlarmMsg(WPARAM wParam)
	{
		LRESULT lRst = 0;
//		m_pIRobot->SuspendRobot();
		m_csSuspend.Lock();
		SetThreeLight(L_RED);
		m_bSuspend = TRUE;
		++m_iSuspendCount;
		m_csSuspend.Unlock();
		lRst = SendMsg(WM_ULMSG_WARNING, wParam, NULL);
		m_csSuspend.Lock();
		--m_iSuspendCount;
		if (m_iSuspendCount < 1)
		{
//			if (m_pIRobot->ResumeRobot() == 1)
			{
				m_bSuspend = FALSE;
				SetThreeLight(L_GRN);
			}
// 			else
// 			{
// 				++m_iSuspendCount;
// 			}
			
		}
		m_csSuspend.Unlock();
	
		return lRst;
	}

	void CWorkFlow::ViewMsg(WPARAM wParam)
	{
		PostMsg(WM_ULMSG_INFORMATION_V, wParam, NULL);
	}

	void CWorkFlow::AlarmSetStep(StepInfo &sStepInfo, const int iAlaramNo, const int iYesStep, const int iCancelStep/* = -1*/)
	{
		LRESULT lRst = 0;
		lRst = AlarmMsg(iAlaramNo);
		if (IDYES == lRst)
		{
			sStepInfo.SetSubStep(iYesStep);
			sStepInfo.UpdataStTime();
		}
		else if (IDNO == lRst)
		{
			SendMsg(WM_ULMSG_STOP, NULL, NULL);
		}
		else
		{
			if (iCancelStep != -1)
			{
				sStepInfo.SetSubStep(iCancelStep);
				sStepInfo.UpdataStTime();
			}
		}
	}

	//Start Reset Suspend Stop
	BOOL CWorkFlow::Reset()
	{
		if (SendMsg(WM_ULMSG_INFORMATION, INFO_WANTRESET, NULL) == IDYES)
		{
			ViewMsg(INFO_RESET);
			Sleep(10);
			m_csStart.Lock();
			if (!m_bStart)
			{
				OpenOutPort(_T("料框夹夹紧"));
				CloseOutPort(_T("料框夹松开"));
				CloseOutPort(_T("吸盘1下降"));
				CloseOutPort(_T("吸盘2下降"));
				CloseOutPort(_T("吸盘3下降"));
				CloseOutPort(_T("吸盘4下降"));

				if ((ReadInPort(_T("满料抽屉到位")) == 1) && (ReadInPort(_T("空料抽屉到位")) == 1))
				{
					double dHomeSpeed = 10.0;
					double dHomeAcc = 0.03;
					double dHomeDec = 0.25;
					TCHAR atcSection[128] = {0};
					swprintf(atcSection, _T("Z%d"), 1);
					dHomeSpeed = m_classIniConfig.ReadDouble(atcSection, _T("HomeSpeed"), 10.0);
					dHomeAcc = m_classIniConfig.ReadDouble(atcSection, _T("HomeAcc"), 0.03);
					dHomeDec = m_classIniConfig.ReadDouble(atcSection, _T("HomeDec"), 0.25);

					double dHomeSpeed2 = 10.0;
					double dHomeAcc2 = 0.03;
					double dHomeDec2 = 0.25;
					swprintf(atcSection, _T("Z%d"), 2);
					dHomeSpeed2 = m_classIniConfig.ReadDouble(atcSection, _T("HomeSpeed"), 10.0);
					dHomeAcc2 = m_classIniConfig.ReadDouble(atcSection, _T("HomeAcc"), 0.03);
					dHomeDec2 = m_classIniConfig.ReadDouble(atcSection, _T("HomeDec"), 0.25);

					if (m_pIRobot->IsRobotConnnect() && (m_pIRobot->SetPower(FALSE) == 1) && JumpPos(SD_ROBOT_WAIT_POS))
					{
						m_bReset = !IsRobotBusy(ROBOT_TIMEOUT);
						SetThreeLight(L_YEL);

						if (!m_bReset)
						{
							//								ViewMsg(INFO_ROBOTWAIT);
							SetThreeLight(L_RED);
							SendMsg(WM_ULMSG_INFORMATION, INFO_ROBOTWAIT, 0);
							SetThreeLight(L_YEL);
						}
					}
					else
					{
						m_bReset = FALSE;
						if (!m_pIRobot->IsRobotConnnect())
						{
							//								ViewMsg(INFO_ROBOTDISCONNECT);
							SetThreeLight(L_RED);
							SendMsg(WM_ULMSG_INFORMATION, INFO_ROBOTDISCONNECT, 0);
							SetThreeLight(L_YEL);
						}
						else 
						{
							//								ViewMsg(INFO_ROBOTSETFAIL);
							SetThreeLight(L_RED);
							SendMsg(WM_ULMSG_INFORMATION, INFO_ROBOTSETFAIL, 0);
							SetThreeLight(L_YEL);
						}
					}


					if (m_bReset)
					{
						if (m_classGT.GoHome(1, dHomeSpeed, dHomeAcc, dHomeDec) && m_classGT.GoHome(2, dHomeSpeed2, dHomeAcc2, dHomeDec2))
						{

						}
						else
						{
							m_bReset = FALSE;
							SetThreeLight(L_RED);
							SendMsg(WM_ULMSG_WARNING, ALARM_GOHOMEFAIL, NULL);
							SetThreeLight(L_YEL);
						}
					}
					
				}
				else
				{
					m_bReset = FALSE;
					SetThreeLight(L_RED);
					SendMsg(WM_ULMSG_WARNING, ALARM_PASSTRAY, NULL);
					SetThreeLight(L_YEL);
				}
			}

			m_csStart.Unlock();

			if (m_bReset)
			{
				ViewMsg(INFO_RESET_COM);
			}
			else
			{
				ViewMsg(INFO_RESET_FAIL);
			}
			
		}

		return m_bReset;
	}

	BOOL CWorkFlow::Start()
	{
		m_csStart.Lock();
		if (m_bReset && !m_bStart && m_pIRobot->IsRobotConnnect() 
			 && (m_pIRobot->SetPower(TRUE) == 1) && (m_pIRobot->ChangeTool(0) == 1)/* && (m_pIRobot->SetSpeed(98, 120) == 1)*/
			/* && m_pIVision->IsConnect()*/)
		{
			
			m_iAddDis = 0;
			LRESULT lRst = 0;
			m_bAllowPutFail = FALSE;
			m_bAllowPutPass = FALSE;
//			lRst = SendMsg(WM_ULMSG_INFORMATION, INFO_CONTINUEINFO, NULL);
			CloseOutPort(_T("侧压定位Y伸出"));
			CloseOutPort(_T("侧压定位X伸出"));
			
			SetSpeed(30);

			int iFCheckPos = m_classIniConfig.ReadInt(_T("GTPos"), _T("FCheckPos"), 100);
			int iECheckPos = m_classIniConfig.ReadInt(_T("GTPos"), _T("ECheckPos"), 100);

			if (AxisMovePos(1, iFCheckPos) && AxisMovePos(2, iECheckPos))
			{
				switch (IDYES)
				{
				case IDNO:

				case IDYES:
					{					
						m_sPickAndPutFlow.Init();
						m_sPickAndPutFlow.SetMainSubStep(STEP_PICKANDPUT, 0);
						m_sInTrackFlow.Init();
						m_sInTrackFlow.SetNoSuspend(TRUE);
						m_sInTrackFlow.SetMainSubStep(STEP_INTRACK, 0);
						m_sFailTrayFlow.Init();
						m_sFailTrayFlow.SetMainSubStep(STEP_FAILTRAY, 0);
						m_sFailTrayFlow.SetNoSuspend(TRUE);
						m_sPassTrayFlow.Init();
						m_sPassTrayFlow.SetMainSubStep(STEP_PASSRAY, 0);
						m_sPassTrayFlow.SetNoSuspend(TRUE);
						m_bAllPick = FALSE;
						m_bEngSuspend = FALSE;
						m_bSuspend = FALSE;
						m_iSuspendCount = 0;
						m_iPickPosIndex = 0;
						m_iSuckerIndex = 0;
						m_iCaptrueIndex = 0;

						for (int i = 0; i < m_iPutFailPos; ++i)
						{
							PostMsg(WM_ULMSG_PUTNGP0S, i, NULL);
						}

						for (int i = 0; i < m_iPutPassPos; ++i)
						{
							PostMsg(WM_ULMSG_PUTOKP0S, i, NULL);
						}

						m_csVector.Lock();
						m_vecStepInfo.clear();
						m_vecStepInfo.push_back(&m_sInTrackFlow);
						m_vecStepInfo.push_back(&m_sPickAndPutFlow);
						m_vecStepInfo.push_back(&m_sFailTrayFlow);
						m_vecStepInfo.push_back(&m_sPassTrayFlow);
						m_csVector.Unlock();
						m_bStart = TRUE;

					}
					break;
				}
			}
			else
			{

			}
			
		}

		m_csStart.Unlock();

		if (m_bStart)
		{
			ViewMsg(INFO_RUNNING);
			SetThreeLight(L_GRN);
		}
		else if (!m_pIRobot->IsRobotConnnect())
		{
			ViewMsg(INFO_ROBOTDISCONNECT);
		}
		else if (!m_pIVision->IsConnect())
		{
			ViewMsg(INFO_VISIONDISCONNECT);
		}
		else if (!m_bReset)
		{
			ViewMsg(INFO_NORESETCOM);
		}

		return m_bStart;
	}

	BOOL CWorkFlow::Suspend()
	{
		//if (m_pIRobot && (m_pIRobot->SuspendRobot() == 1))
		{
			m_csSuspend.Lock();
			SetThreeLight(L_YEL);
			m_bSuspend = TRUE;
			++m_iSuspendCount;
			m_csSuspend.Unlock();
		}

		if (m_bSuspend || m_bEngSuspend)
		{
			ViewMsg(INFO_SUSPEND);
		}

		return (m_bSuspend || m_bEngSuspend);
	}

	BOOL CWorkFlow::Resume()
	{
		m_csSuspend.Lock();
		
		if (m_bSuspend)
		{
			--m_iSuspendCount;
			if (m_iSuspendCount < 1 )
			{
				if (!m_bEngSuspend)
				{
					//if (m_pIRobot->ResumeRobot() == 1)
					{
						m_bSuspend = FALSE;
						SetThreeLight(L_GRN);
					}
// 					else
// 					{
// 						++m_iSuspendCount;
// 					}
				}
			
			}
		}

		m_csSuspend.Unlock();

		if (!m_bSuspend && !m_bEngSuspend)
		{
			ViewMsg(INFO_RUNNING);
		}

		return !m_bSuspend;
	}

	BOOL CWorkFlow::Stop()
	{
		BOOL bRet = FALSE;
		
		if (SendMsg(WM_ULMSG_INFORMATION, INFO_WANTSTOP, NULL) == IDYES)
		{
			m_csStart.Lock();
			m_bStart = FALSE;
			m_bReset = FALSE;
			m_classGT.AxisStop(1);
			m_classGT.AxisStop(2);
	
			
			if (m_pIRobot)
			{
				m_pIRobot->ChangeTool(0);
				m_pIRobot->IsRobotBusy(ROBOT_TIMEOUT);
				m_pIRobot->SetPower(FALSE);
				m_pIRobot->SetSpeed(10, 10);
				if (IsSuspend())
				{
					m_pIRobot->StopRobot();
					DisConnectRobot();
				}			
			}

			bRet = TRUE;
			m_csStart.Unlock();
			SetThreeLight(L_YEL);
		}

		return bRet;
	}

	BOOL CWorkFlow::IsRunning()
	{
		return m_bStart;
	}

	BOOL CWorkFlow::IsSuspend()
	{
		return (m_bSuspend || m_bEngSuspend);
	}


	//RecordLog
	void CWorkFlow::RecordLog(LPCTSTR lpstrData, const int iType)
	{
		if (m_pIFile)
		{
			tstring strLogDir = _T("");
			
			switch (iType)
			{
			case 0:
				{
					strLogDir = m_classIniConfig.GetModulePath() + _T("Log\\Alarm\\");
				}
				break;

			case 1:
				{
					strLogDir = m_classIniConfig.GetModulePath() + _T("Log\\Message\\");
				}
				break;

			case 2:
				{
					strLogDir = m_classIniConfig.GetModulePath() + _T("Log\\Vision\\");
				}
				break;

			case 3:
				{
					strLogDir = m_classIniConfig.GetModulePath() + _T("Log\\PutBoardMessage\\");
				}
				break;

			case 4:
				{
					strLogDir = m_classIniConfig.GetModulePath() + _T("Log\\Axis\\");
				}
				break;
			}

			TCHAR atcData[512] = {0};
			TCHAR atcTime[128] = {0};
			SYSTEMTIME sysTime;
			TCHAR atcName[512] = _T("");
			::GetLocalTime(&sysTime);

			swprintf(atcName, _T("%s%04d-%02d-%02d.csv"), strLogDir.c_str(), sysTime.wYear, sysTime.wMonth, sysTime.wDay);
			swprintf(atcTime, _T("%02d:%02d:%02d,"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			if (m_pIFile->Open(atcName))
			{
				USES_CONVERSION;
				m_pIFile->Write(T2A(atcTime), strlen(T2A(atcTime)));
				m_pIFile->Write(T2A(lpstrData), strlen(T2A(lpstrData)));
				if ((lpstrData[strlen(T2A(lpstrData)) - 1] == '\r')
					|| (lpstrData[strlen(T2A(lpstrData)) - 1] == '\n'))
				{
				}
				else
				{
					m_pIFile->Write("\r\n", 3);
				}
			}			
			m_pIFile->Close();
		}
	}

	void CWorkFlow::GetCurrentProduct(LPTSTR lpstrName)
	{
		if (lpstrName)
		{
			_tcscpy(lpstrName, m_strProductName.c_str());
		}
	}

	void CWorkFlow::SetParent(HWND hWnd)
	{
		m_hParaent = hWnd;
	}

	BOOL CWorkFlow::ConnectVision()
	{
		BOOL bRet= FALSE;
		if (m_pIVision)
		{
			tstring strIp = m_classIniConfig.ReadString(_T("Vision"), _T("Ip"), _T("127.0.0.1"));
			int iPort = m_classIniConfig.ReadInt(_T("Vision"), _T("Port"), 2000);
			bRet = m_pIVision->ConnectServer(strIp.c_str(), iPort);

			if (bRet)
			{
				TCHAR atcMsg[128] = {0};
				BYTE  abyRecv[128] = {0};
				swprintf(atcMsg, _T("Product:%s"), m_strProductName.c_str());
				USES_CONVERSION;
				m_pIVision->SendData((BYTE *)T2A(atcMsg), _tcslen(atcMsg));
				Sleep(500);
				m_pIVision->RecvData(abyRecv, 128);

				tstring strRecv = _T("");

				strRecv = A2T((char *)abyRecv);

				if (strRecv != _T("Product:OK"))
				{
					m_pIVision->DisConnect();
					bRet = FALSE;
				}
			}
		}
		return bRet;
	}

	BOOL CWorkFlow::IsVisionConnect()
	{
		BOOL bRet= FALSE;
		if (m_pIVision)
		{
			 bRet = m_pIVision->IsConnect();		
		}
		return bRet;
	}

	BOOL CWorkFlow::DisconnectVision()
	{
		BOOL bRet= TRUE;
		if (m_pIVision)
		{
			m_pIVision->DisConnect();
		}
		return bRet;
	}

	int CWorkFlow::GetBoardRst(const int iIndex)
	{
		return m_aiResultTemp[iIndex];
	}

	
}

void CreateWorkFlow(CIWorkFlow **ppWorkFlow)
{
	if (*ppWorkFlow)
	{
		ReleaseWorkFlow(*ppWorkFlow);
		*ppWorkFlow = NULL;
	}

	*ppWorkFlow = new SD_WORKFLOW::CWorkFlow();
}

void ReleaseWorkFlow(CIWorkFlow *pWorkFlow)
{
	if (pWorkFlow)
	{
		delete pWorkFlow;
	}

	pWorkFlow = NULL;
}