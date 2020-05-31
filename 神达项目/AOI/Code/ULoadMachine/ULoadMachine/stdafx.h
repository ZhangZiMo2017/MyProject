
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����

#include <tchar.h>
#include <string>
#include <process.h>
#include <algorithm>


#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

#pragma warning(disable: 4996)

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��



#include "CIniFile.h"
#include "Encrypt.h"
#include "View/UIlib.h"
#include "ViewBase.h"
#include "Login.h"
#include "ChangePswd.h"
#include "MessageBox.h"
#include "GFunction.h"


#include "..\..\SD_AWorkFlow\SD_AWorkFlow\SD_IWorkFlow.h"

using namespace DuiLib;
#ifdef _DEBUG
#pragma comment(lib, "Lib/DuiLib.lib")
#pragma comment(lib, "../../../Bin/Debug/SD_AWorkFlow.lib")
#pragma comment(lib, "../../../Bin/Debug/Encrypt.lib")
#else
#pragma comment(lib, "Lib/DuiLib.lib")
#pragma comment(lib, "../../../Bin/Release/SD_AWorkFlow.lib")
#pragma comment(lib, "../../../Bin/Release/Encrypt.lib")
#endif




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


