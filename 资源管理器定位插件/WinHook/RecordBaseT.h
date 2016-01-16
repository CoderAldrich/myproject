#pragma once
#include <Windows.h>
#include <atlstr.h>
#include <algorithm>
#include <string>
#include <map>
#include <list>
using namespace std;

#include "RWLock.h"

template<typename T,typename TT>
class CRecordBaseT
{
public:
	typedef map< T,TT > RECORD_MAP;
protected:
	CRWLock         m_rwLocker;
	RECORD_MAP m_RecordMap;
public:
	CRecordBaseT()
	{

	}
	~CRecordBaseT()
	{

	}
public:

	VOID WLock()
	{
		m_rwLocker.wlock();
	}
	VOID UnLock()
	{
		m_rwLocker.unlock();
	}

	RECORD_MAP *GetInternalMap()
	{
		return &m_RecordMap;
	}

	BOOL AddRecord( T KeyValue,TT pData)
	{
		BOOL bAddRecord = FALSE;

		m_rwLocker.wlock();

		RECORD_MAP::iterator it = m_RecordMap.find(KeyValue);
		if (it==m_RecordMap.end())
		{
			m_RecordMap.insert(make_pair(KeyValue, pData));

			bAddRecord = TRUE;
		}

		m_rwLocker.unlock();

		return bAddRecord;
	}
	BOOL DelRecord( T KeyValue,TT *ppData)
	{
		BOOL bDelRecord = FALSE;
		m_rwLocker.wlock();

		RECORD_MAP::iterator it = m_RecordMap.find(KeyValue);
		if (it!=m_RecordMap.end())
		{
			if ( ppData )
			{
				*ppData = it->second;
			}
			m_RecordMap.erase(it);

			bDelRecord = TRUE;
		}

		m_rwLocker.unlock();

		return bDelRecord;
	}

	BOOL SetRecordData(T KeyValue,TT pData)
	{
		BOOL bSetData = FALSE;
		m_rwLocker.wlock();

		RECORD_MAP::iterator it = m_RecordMap.find(KeyValue);
		if (it!=m_RecordMap.end())
		{
			it->second = pData;
			bSetData = TRUE;
		}

		m_rwLocker.unlock();

		return bSetData;

	}

	BOOL GetRecordData(T KeyValue,TT *ppData)
	{
		BOOL bGetData = FALSE;

		m_rwLocker.rlock();

		RECORD_MAP::iterator it = m_RecordMap.find(KeyValue);
		if (it!=m_RecordMap.end())
		{
			if(ppData)
			{
				*ppData = it->second;
				bGetData = TRUE;
			}
		}

		m_rwLocker.unlock();

		return bGetData;
	}
};


