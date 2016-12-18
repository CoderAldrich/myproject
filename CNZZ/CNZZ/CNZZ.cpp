// CNZZ.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <Windows.h>
#include "WininetHelper.h"


int _tmain(int argc, _TCHAR* argv[])
{

	CString strData;
	strData =  HttpQueryData(L"http://new.cnzz.com/v1/login.php?siteid=1254094396");

	char chDataPost[]="password=254324101";
	HttpQueryData(L"http://new.cnzz.com/v1/login.php?t=login&siteid=1254094396",NULL,NULL,NULL,chDataPost,strlen(chDataPost));

	HttpQueryData(L"http://web.umeng.com/main.php?c=site&a=overview&ajax=module=report&siteid=1254094396&st=2016-11-25&et=2016-11-25&type=Line&Quota=pv,uv,ip&Period=48Hours&downloadType=csv",NULL,NULL,L"C:\\test\\data.csv");

	return 0;
}

