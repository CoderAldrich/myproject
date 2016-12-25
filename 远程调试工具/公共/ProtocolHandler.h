#pragma once

#include <map>
#include <atlstr.h>
class CProtocolHandler{
protected:
	typedef std::map<CStringA,CStringA> MAP_DATA,*PMAP_DATA;
	typedef MAP_DATA::iterator MAP_DATA_PTR;
	MAP_DATA   m_mapData;
public:
	CProtocolHandler();
	~CProtocolHandler();
	BOOL ParseProtocolString( LPCSTR pchDataBuffer,int nDataLen );
	CStringA GetParamValueString( LPCSTR pchKeyName,LPCSTR pchDefaultValue );
	int GetParamValueInt( LPCSTR pchKeyName,int nDefaultValue );
	VOID SetParamValueString( LPCSTR pchKeyName,LPCSTR pchValue );
	VOID SetParamValueInt( LPCSTR pchKeyName, int nValue );
	CStringA BuildData();
};