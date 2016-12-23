#pragma once


#include <atlstr.h>
#include <map>
class CMemIniFile
{
public:
	typedef std::map<CStringW,CStringW> MAP_DATA,*PMAP_DATA;
	typedef MAP_DATA::iterator MAP_DATA_PTR;

	typedef std::map<CStringW,MAP_DATA> MAP_SECTION,*PMAP_SECTIPN;
	typedef MAP_SECTION::iterator MAP_SECTION_PTR;

protected:

	MAP_SECTION m_mapSection;

public:
	CMemIniFile( ){}

	~CMemIniFile(){}
protected:
	BOOL ParseDataLineW( LPCWSTR pszData,LONGLONG llLen,LONGLONG &llLineStart,LONGLONG &llLineEnd );
	VOID ParseSectionDataW(LPCWSTR pszData,LONGLONG llLen,CString &strName,CString &strValue);
	BOOL ParseUnicodeStringW(LPCWSTR pszData,LONGLONG llLen);
public:
	BOOL ParseMemoryDataW( CONST BYTE *pData , int nDataLen );
	CString GetIniString(LPCWSTR pszSection,LPCWSTR pszKeyName,LPCWSTR pszDefaultValue);
	UINT GetIniUint(LPCWSTR pszSection,LPCWSTR pszKeyName,UINT unDefaultValue);
	UINT GetSectionCount();
	UINT GetSectionDataCount(LPCWSTR pszSection);
	CString EnumSectionByIndex( UINT nIndex );
	BOOL WriteIniString( LPCWSTR pszSection,LPCWSTR pszKeyName,LPCWSTR pszKeyValue );
	BOOL WriteIniInt( LPCWSTR pszSection,LPCWSTR pszKeyName,int nValue );
	CString BuildData();
};