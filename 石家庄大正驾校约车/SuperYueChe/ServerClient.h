#pragma once

#include <afxinet.h>
#include <map>
using namespace std;

#include ".\json\json.h"
#ifdef DEBUG
#pragma comment(lib,"json_vc71_libmtd.lib")
#else
#pragma comment(lib,"json_vc71_libmt.lib")
#endif


typedef map<CString,CString> MAP_TIME_GUID;
typedef MAP_TIME_GUID::iterator MAP_TIME_GUID_PTR;

class CServerClient;

typedef struct SUB_THREAD_PARAM
{
	CServerClient *pThis;
	DWORD dwQueryTimeOut;
	BOOL  *pbWaiting;
	CString *pstrYueCheData;
	HANDLE hEvent;
	HANDLE hMutex;
}SUB_THREAD_PARAM,*PSUB_THREAD_PARAM;

#define MAX_SUB_QUERY_DATA_THREAD 20

class CServerClient
{
protected:

	CString strAspSession;
	CString strIDNumber;
	CString strName;
	CString strCode;
	CString strLoginID;
	CString strID;

	BOOL  m_bClientLogin;

	HANDLE hEvent;
	HANDLE hMutex;
	
	BOOL bQueryDataThreadRun;
	HANDLE hArraySubThreadHandle[MAX_SUB_QUERY_DATA_THREAD];
	SUB_THREAD_PARAM *pSubParam[MAX_SUB_QUERY_DATA_THREAD];

	
	CString strYueCheData;
public:
	BOOL bQueryDataThreadWait;
	CServerClient(void);
	~CServerClient(void);

	static CString UTF8ToUnicode(char* UTF8);
	static CString HttpQueryData( LPCWSTR pszUrl, PVOID pPostData = NULL,UINT nDataLen = 0  , DWORD dwTimeOut = INFINITE);

	static CString IndexToTimeString(int nIndex);

	static BOOL ParseLoginResult(
		const char *pStrResult,
		CString &strIDNumber,
		CString &strName,
		CString &strCode,
		CString &strLoginID,
		CString &strID,
		CString &strErrorText
		) ;
	static BOOL ParseQueryResult(const char *pStrQuery,CStringA strWorkTime,MAP_TIME_GUID *pTimeGUID);
	static BOOL ParseYueCheResult(const char *pStrQuery,CString &strErrorText);

	BOOL DoLoginServer(CString strUserName,CString strPassWord,CString &strErrorText);
	BOOL DoCheckLoginStatus();
	BOOL DoYueChe(LPSYSTEMTIME pDate,BOOL *pbSelItemArray);
	CString DoQueryYueCheDataTimeOut(DWORD dwTimeOut/*µ¥Î» ms*/);
	BOOL PrepareQueryYueCheData();
	CString SuperQueryYueCheData();

	VOID SaveLoginSession();
	VOID LoadLoginSession();
};
