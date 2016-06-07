#pragma once
#include <MsHTML.h>
#include <ExDisp.h>
#include "IWBCoreControler.h"
#include "IWBCoreNotifyer.h"

typedef VOID (WINAPI *TypeCWInit)(BOOL bPhoneMode , LPCWSTR pszUserAgent);
typedef IWBCoreControler * (WINAPI *TypeCWCreateView)( );
typedef BOOL (WINAPI *TypeInitShieldResource)();
typedef BOOL (WINAPI *TypeUpdateShildType)( LPCWSTR *pszArrayTypes,int nTypesCount );
typedef BOOL (WINAPI *TypeSetShieldResource)(BOOL bSwitchOn);
typedef BOOL (WINAPI *TypeInitStopWriteDisk)();
typedef BOOL (WINAPI *TypeSetEnableWriteDisk)(BOOL bEnableWriteDisk);
typedef BOOL (WINAPI *TypeSetSlient)();
