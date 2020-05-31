#pragma once
#include <tchar.h>
#include <string>
#include <map>
#include <WinDef.h>
#include <atlconv.h>
#include <process.h>
#include <vector>

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

#define map std::map
#define vector std::vector


#include "CIniFile.h"
#include "Global_Define.h"
#include "AM_IRobot.h"
#include "GoogolTech.h"
#include "AM_IClient.h"
#include "AM_IUphRecord.h"
#include "AM_IFile.h"


#pragma warning(disable: 4996)
#ifdef _DEBUG
#pragma comment(lib, "../../../Bin/Debug/AM_Client.lib")
#pragma comment(lib, "../../../Bin/Debug/AM_Robot.lib")
#pragma comment(lib, "../../../Bin/Debug/AM_UphRecord.lib")
#pragma comment(lib, "../../../Bin/Debug/AM_File.lib")
#else
#pragma comment(lib, "../../../Bin/Release/AM_Client.lib")
#pragma comment(lib, "../../../Bin/Release/AM_Robot.lib")
#pragma comment(lib, "../../../Bin/Release/AM_UphRecord.lib")
#pragma comment(lib, "../../../Bin/Release/AM_File.lib")
#endif // _DEBUG
