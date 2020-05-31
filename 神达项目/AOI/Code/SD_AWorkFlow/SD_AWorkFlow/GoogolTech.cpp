#include "StdAfx.h"
#include "GoogolTech.h"




CGoogolTech::CGoogolTech(void)
{
	m_bInitCard = FALSE;
	CIniFile cInifile;
	m_strConfig = cInifile.GetModulePath() + _T("GTS800.cfg");
	m_strExtConfig = cInifile.GetModulePath() + _T("ExtModule.cfg");
	m_ausOutValue[0] = 0xFFFF;
	m_ausOutValue[1] = 0xFFFF;
	m_ausOutValue[2] = 0xFFFF;
	m_ausOutValue[3] = 0xFFFF;
	InitCard();
}

CGoogolTech::~CGoogolTech(void)
{
	CloseCard();
}

int CGoogolTech::ReadInPort(const int iPortNo, const int iPortHex, const int iStation /* = 1 */)
{
	int iRet = -1;

	if (iPortNo == 0)
	{
		long lValue = 0;
		m_csExtIO.Lock();
		if (GT_GetDi(MC_GPI, &lValue) == 0)
		{
			if (lValue & (1 << iPortHex))
			{
				iRet = 0;
			}
			else
			{
				iRet = 1;
			}
		}
		m_csExtIO.Unlock();
	}
	else
	{
		USHORT usValue = 0;
		m_csExtIO.Lock();
		if (GT_GetExtIoBit(iPortNo - 1, iPortHex, &usValue) == 0)
		{
			if (usValue )
			{
				iRet = 0;
			}
			else
			{
				iRet = 1;
			}
		}
		m_csExtIO.Unlock();
	}

	return iRet;

}

int CGoogolTech::CheckOutPort(const int iPortNo, const int iPortHex, const int iStation /* = 1 */)
{
	int iRet = -1;

	if (iPortNo == 0)
	{
		long lValue = 0;
		m_csExtIO.Lock();
		if (GT_GetDo(MC_GPO, &lValue) == 0)
		{
			if (lValue & (1 << iPortHex))
			{
				iRet = 0;
			}
			else
			{
				iRet = 1;
			}
		}
		m_csExtIO.Unlock();
	}
	else
	{
		USHORT usValue = 0;
		m_csExtIO.Lock();
		if (GT_GetExtIoBit(iPortNo - 1, iPortHex, &usValue) == 0)
		{
			if (usValue )
			{
				iRet = 0;
			}
			else
			{
				iRet = 1;
			}
		}
		m_csExtIO.Unlock();
	}

	return iRet;
}

int CGoogolTech::OpenOutPort(const int iPortNo, const int iPortHex, const int iStation /* = 1 */)
{
	int iRet = -1;

	if (iPortNo == 0)
	{
		long lValue = 0;
		m_csExtIO.Lock();
		if (GT_SetDoBit(MC_GPO, iPortHex + 1, 0) == 0)
		{
			iRet = 1;
		}
		m_csExtIO.Unlock();
	}
	else
	{
		USHORT usValue = 0;
		m_csExtIO.Lock();
		if (GT_SetExtIoBit(iPortNo - 1, iPortHex, 0) == 0)
		{
			m_ausOutValue[iPortNo - 1] = (m_ausOutValue[iPortNo - 1] & (~(1 << iPortHex)));
			iRet = 1;
		}
		m_csExtIO.Unlock();
	}

	return iRet;
}

int CGoogolTech::CloseOutPort(const int iPortNo, const int iPortHex, const int iStation /* = 1 */)
{
	int iRet = -1;

	if (iPortNo == 0)
	{
		long lValue = 0;
		m_csExtIO.Lock();
		if (GT_SetDoBit(MC_GPO, iPortHex + 1, 1) == 0)
		{
			iRet = 1;
		}
		m_csExtIO.Unlock();
	}
	else
	{
		USHORT usValue = 0;
		m_csExtIO.Lock();
		if (GT_SetExtIoBit(iPortNo - 1, iPortHex, 1) == 0)
		{
			m_ausOutValue[iPortNo - 1] = (m_ausOutValue[iPortNo - 1] | (1 << iPortHex));
			iRet = 1;
		}
		m_csExtIO.Unlock();
	}

	return iRet;
}

BOOL CGoogolTech::InitCard()
{
	USES_CONVERSION;

	if (!m_bInitCard)
	{
		short sRtn = 0;
 		sRtn |= GT_Open();
		char atcPath[128] = {0};
		sRtn |= GT_Reset();
		Sleep(500);	
		sRtn |= GT_LoadConfig(T2A(m_strConfig.c_str()));
	
		sRtn |= GT_OpenExtMdl("gts.dll");
 		
		
		sRtn |= GT_ClrSts(1, 4);
		
	
		sRtn |= GT_LoadExtConfig(T2A(m_strExtConfig.c_str()));
		sRtn |= GT_SetDo(MC_GPO,0xFFFF);
		sRtn |= GT_SetExtIoValue(0,0xFFFF);
		sRtn |= GT_SetExtIoValue(1,0xFFFF);
		sRtn |= GT_SetExtIoValue(2,0xFFFF);
		sRtn |= GT_SetExtIoValue(3,0xFFFF);

		if (sRtn == 0)
		{
			m_bInitCard = TRUE;
		}
	}

	return m_bInitCard;
}

void CGoogolTech::CloseCard()
{

	GT_SetDo(MC_GPO,0xFFFF);
	GT_SetExtIoValue(0,0xFFFF);
	GT_SetExtIoValue(1,0xFFFF);
	GT_CloseExtMdl();
	GT_Close();
	
	m_bInitCard = FALSE;
}

int CGoogolTech::ReadMulitInPort(long &lValue, const int iPortNo, const int iStation /* = 1 */)
{
	int iRet = -1;

	if (iPortNo == 0)
	{
		m_csExtIO.Lock();
		if (GT_GetDi(MC_GPI, &lValue) == 0)
		{
			iRet = 1;		
		}
		m_csExtIO.Unlock();
	}
	else
	{
		USHORT usValue = 0;
		m_csExtIO.Lock();
		if (GT_GetExtIoValue(iPortNo - 1, &usValue) == 0)
		{
			lValue = usValue;
			iRet = 1;	
		}
		m_csExtIO.Unlock();
	}

	return iRet;
}

int CGoogolTech::ReadMulitOutPort(long &lValue, const int iPortNo, const int iStation /* = 1 */)
{
	int iRet = -1;

	if (iPortNo == 0)
	{
		m_csExtIO.Lock();
		if (GT_GetDo(MC_GPO, &lValue) == 0)
		{
			iRet = 1;		
		}
		m_csExtIO.Unlock();
	}
	else
	{
		USHORT usValue = 0;
		m_csExtIO.Lock();
// 		if (GT_GetExtDoValue(iPortNo - 1, &usValue) == 0)
// 		{
// 			lValue = usValue;
// 			iRet = 1;	
// 		}
		lValue = m_ausOutValue[iPortNo - 1];
		m_csExtIO.Unlock();
	}

	return iRet;
}

BOOL CGoogolTech::IsInit()
{
	return m_bInitCard;
}

BOOL CGoogolTech::JogMove(const int iAxis, const BOOL bDir,const double dSpeed /* = 20.0 */, const double dAcc /* = 0.0625 */, const double dDec /* = 0.0625 */)
{
	BOOL bRet = FALSE;
	
	if (!AxisIsBusy(iAxis))
	{
		TJogPrm jog;
		short sRtn = 0;
		sRtn |= GT_PrfJog(iAxis);	
		sRtn |= GT_GetJogPrm(iAxis, &jog);
		jog.acc = dAcc;
		jog.dec = dDec;
		// 设置Jog运动参数
		sRtn |= GT_SetJogPrm(iAxis, &jog);
		// 设置AXIS轴的目标速度
		if (bDir)
		{
			sRtn |= GT_SetVel(iAxis, dSpeed);
		}
		else
		{
			sRtn |= GT_SetVel(iAxis, -dSpeed);
		}

		sRtn |= GT_Update(1<<(iAxis-1));

		if (sRtn == 0)
		{
			bRet = TRUE;
		}
		else
		{
			AxisStop(iAxis);
		}
	}

	return bRet;
}

BOOL CGoogolTech::AxisStop(const int iAxis)
{
	if (GT_Stop(1<<(iAxis-1), 0) == 0)
	{
		Sleep(500);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CGoogolTech::GoHome(const int iAxis, const double dSpeed /* = 10.0 */, const double dAcc /* = 0.03 */, const double dDec /* = 0.25 */, const BOOL bHomeDir /* = FALSE */)
{
	
	BOOL bRet = FALSE;
	short sRtn, capture;
	TTrapPrm trapPrm;
	long status, pos;

	if (m_bInitCard)
	{

		sRtn = 0;
		sRtn |= GT_ClrSts(iAxis);
		sRtn |= GT_AxisOn(iAxis);

		sRtn |= GT_ClrSts(iAxis);
		// 切换到点位运动模式
		sRtn |= GT_PrfTrap(iAxis);
		// 读取点位模式运动参数
		sRtn |= GT_GetTrapPrm(iAxis, &trapPrm);
		trapPrm.acc = dAcc;
		trapPrm.dec = dDec;
		// 设置点位模式运动参数
		sRtn |= GT_SetTrapPrm(iAxis, &trapPrm);
		// 设置点位模式目标速度，即回原点速度
		sRtn |= GT_SetVel(iAxis, dSpeed);

		long lLimit = 0;
		long lHome = 0;
		GT_GetDi(MC_LIMIT_NEGATIVE, &lLimit);
		GT_GetDi(MC_HOME, &lHome);
ReGoHome:
		sRtn |= GT_ClrSts(iAxis);
		if ((lLimit & (1 << (iAxis - 1))) || !(lHome & (1 << (iAxis - 1))))
		{
			sRtn |= GT_ZeroPos(iAxis);

			sRtn |= GT_SetPos(iAxis, 40000);
			sRtn |= GT_Update(1<<(iAxis-1));
			do
			{
				// 读取轴状态
				sRtn |= GT_GetSts(iAxis, &status);
				// 等待运动停止
			}while(( status & 0x400 ));
		}

		sRtn |= GT_ClrSts(iAxis);
		sRtn |= GT_SetCaptureMode(iAxis, CAPTURE_HOME);

		// 设置点位模式目标位置，即原点搜索距离
		sRtn |= GT_SetPos(iAxis, SEARCH_HOME);

		// 启动运动
		sRtn |= GT_Update(1<<(iAxis-1));
		do
		{
			// 读取轴状态
			sRtn |= GT_GetSts(iAxis, &status);
			// 读取捕获状态
			sRtn |= GT_GetCaptureStatus(iAxis, &capture, &pos);

			// 如果运动停止，返回出错信息
			if( 0 == ( status & 0x400 ) )
			{
				GT_GetDi(MC_LIMIT_NEGATIVE, &lLimit);
				GT_GetDi(MC_HOME, &lHome);

				if ((lLimit & (1 << (iAxis - 1))))
				{
					goto ReGoHome;
				}
				else
				{
					return bRet;
				}
			}
			// 等待捕获触发
		}while( (0 == capture ));

		sRtn |= GT_Stop(1 << (iAxis - 1), 1 << (iAxis - 1));
		Sleep(1000);
		sRtn |= GT_ZeroPos(iAxis);
		// 显示捕获位置
		// 运动到"捕获位置+偏移量"
// 		sRtn |= GT_SetPos(iAxis, HOME_OFFSET);
// 
// 		// 在运动状态下更新目标位置
// 		sRtn |= GT_Update(1<<(iAxis-1));
// 
// 		do
// 		{
// 			// 读取轴状态
// 			sRtn |= GT_GetSts(iAxis, &status);
// 		}while( (status & 0x400 ));

		if (sRtn == 0)
		{
			bRet = TRUE;
		}
	}
	
	return bRet;
}

BOOL CGoogolTech::GetCurPos(const int iAxis, long &lCurPos)
{
	lCurPos = 0;
	short sRtn = 0;
	double dPos = 0.0;
	sRtn = GT_GetEncPos(iAxis, &dPos);
	lCurPos = (long)dPos;
// 	 GT_PrfTrap(iAxis);
// 	 sRtn = GT_GetPos(iAxis, &lCurPos);
	if (sRtn == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CGoogolTech::AxisIsBusy(const int iAxis)
{
	BOOL bRet = TRUE;
	short sRtn = 0;
	long lStatus = 0;

	sRtn |= GT_GetSts(iAxis, &lStatus);

	if ((sRtn == 0) )
	{
		if (lStatus & 0x400)
		{		
		}
		else
		{
			Sleep(100);
			bRet = FALSE;
		}
		
	}

	return bRet;
}

BOOL CGoogolTech::AxisMove(const int iAxis, long lDistance, const int iMode, const double dSpeed /* = 20 */, const double dAcc /* = 0.1 */, const double dDec /* = 0.1 */)
{
	BOOL bRet = FALSE;
	long lPos = 0;

	if (!GetCurPos(iAxis, lPos))
	{
		return FALSE;
	}
	
	if (!AxisIsBusy(iAxis))
	{
		TTrapPrm trap;
		short sRtn = 0;
		sRtn |= GT_PrfTrap(iAxis);
		// 读取点位运动参数
		sRtn |= GT_GetTrapPrm(iAxis, &trap);
		trap.acc = dAcc;
		trap.dec = dDec;
		trap.smoothTime = 0;
		// 设置点位运动参数
		sRtn |= GT_SetTrapPrm(iAxis, &trap);
		// 设置AXIS轴的目标位置
		
		if (iMode == 1)
		{
//			if (GetCurPos(iAxis, lPos))
			{
				lPos = lDistance + lPos;
			}
// 			else
// 			{
// 				lPos = 0;
// 				sRtn = 1;
// 			}
		}
		else
		{
			lPos = lDistance;
		}

		sRtn |= GT_SetPos(iAxis, lPos);
	
		// 设置AXIS轴的目标速度
		sRtn |= GT_SetVel(iAxis, dSpeed);

		// 启动AXIS轴的运动
		if (sRtn == 0)
		{
			sRtn |= GT_Update(1<<(iAxis-1));
		}

		if (sRtn == 0)
		{
			bRet = TRUE;
		}		
	}

	return bRet;
}

int CGoogolTech::GetAxisAlarm(const int iAxis)
{
	int iRet = 0;

	long lValue = 0;
	m_csExtIO.Lock();
	
	if (GT_GetSts(iAxis, &lValue) == 0)
	{
		if (lValue & 0x2)
		{
			iRet = 1;
		}
		else
		{
			iRet = 0;
		}
	}
	m_csExtIO.Unlock();

	return iRet;
}