#pragma once
#include "gts.h"
//#include "ExtMdl.h"
#pragma comment(lib, "gts.lib")
#pragma comment(lib,"ExtMdl.lib")

class CGoogolTech
{
public:
	CGoogolTech(void);
	~CGoogolTech(void);

	int ReadInPort(const int iPortNo, const int iPortHex, const int iStation = 1);
	int CheckOutPort(const int iPortNo, const int iPortHex, const int iStation = 1);

	int OpenOutPort(const int iPortNo, const int iPortHex, const int iStation = 1);
	int CloseOutPort(const int iPortNo, const int iPortHex, const int iStation = 1);

	int ReadMulitOutPort(long &lValue, const int iPortNo, const int iStation = 1);
	int ReadMulitInPort(long &lValue, const int iPortNo, const int iStation = 1);

	BOOL InitCard();
	void CloseCard();
	BOOL IsInit();

	BOOL JogMove(const int iAxis, const BOOL bDir,const double dSpeed = 20.0, const double dAcc = 0.0625, const double dDec = 0.0625);
	BOOL AxisStop(const int iAxis);
	BOOL AxisMove(const int iAxis, long lDistance, const int iMode, const double dSpeed = 20, const double dAcc = 0.1, const double dDec = 0.1);  //iMode : 0为绝对坐标 1位先对坐标
	BOOL AxisIsBusy(const int iAxis);
	BOOL GoHome(const int iAxis, const double dSpeed = 10.0, const double dAcc = 0.03, const double dDec = 0.25, const BOOL bHomeDir = FALSE);
	BOOL GetCurPos(const int iAxis, long &lCurPos);

	int GetAxisAlarm(const int iAxis);
private:
//	CLock m_csExtIO;
	CLock m_csExtIO;
	BOOL m_bInitCard;
	tstring m_strConfig;
	tstring m_strExtConfig;

	USHORT  m_ausOutValue[4];
};

