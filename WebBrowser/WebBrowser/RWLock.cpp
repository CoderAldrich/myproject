#include "StdAfx.h"
#include "RWLock.h"


CRWLock::CRWLock(void)
	: _rlockCount(0),
	_st(RWLOCK_IDLE),
	_rwaitingCount(0),
	_wwaitingCount(0)
{
	//_stLock = CreateMutex(NULL, FALSE, NULL);
	//assert(_stLock != INVALID_HANDLE_VALUE);
	InitializeCriticalSection(&_stLock);

	/*
	* ���赱ǰ�ж�������������ڵȴ�д���ͷ�,��ô��д�����ͷ�ʱ,������Щ������Ӧ���л�����ִ��.
	*/
	_ev = CreateEvent(NULL, TRUE, FALSE, NULL);
	
}


CRWLock::~CRWLock(void)
{
	//CloseHandle(_stLock);
	DeleteCriticalSection(&_stLock);
	CloseHandle(_ev);
}

void CRWLock::rlock()
{
	bool isWaitReturn = false;
	while(1)
	{
		//WaitForSingleObject(_stLock, INFINITE);
		EnterCriticalSection(&_stLock);
		if(isWaitReturn)
		{
			/*
			* �ȴ��¼�����,���¾�����.
			*/
			--_rwaitingCount;
		}

		if(_st == RWLOCK_IDLE)
		{
			/*
			* ����״̬,ֱ�ӵõ�����Ȩ
			*/
			_st = RWLOCK_R;
			_rlockCount++;
			//ReleaseMutex(_stLock);
			LeaveCriticalSection(&_stLock);
			break;
		}
		else if( _st == RWLOCK_R)
		{
			if(_wwaitingCount > 0)
			{
				/*
				* ��д�����ڵȴ�,��һ��ȴ�,��ʹд���ܻ�þ�������.
				*/
				++_rwaitingCount;
				ResetEvent(_ev);
				//SignalObjectAndWait(_stLock, _ev, INFINITE, FALSE);
				LeaveCriticalSection(&_stLock);

				/*
				* ��Ȼ LeaveCriticalSection() �� WaitForSingleObject() ֮����һ��ʱ�䴰��,
				* ��������windowsƽ̨���¼��ź��ǲ��ᶪʧ��,����û������.
				*/
				WaitForSingleObject(_ev, INFINITE);

				/*
				* �ȴ�����,�������Լ���.
				*/
				isWaitReturn = true;
			}
			else
			{	
				/*
				* �õ�����,����+1
				*/
				++_rlockCount;
				//ReleaseMutex(_stLock);
				LeaveCriticalSection(&_stLock);
				break;
			}
		}
		else if(_st == RWLOCK_W)
		{
			/*
			* �ȴ�д���ͷ�
			*/
			++_rwaitingCount;
			ResetEvent(_ev);
			//SignalObjectAndWait(_stLock, _ev, INFINITE, FALSE);
			LeaveCriticalSection(&_stLock);
			WaitForSingleObject(_ev, INFINITE);

			/*
			* �ȴ�����,�������Լ���.
			*/
			isWaitReturn = true;
		}
		else
		{
			
			break;
		}
	}
}

void CRWLock::wlock()
{
	bool isWaitReturn = false;

	while(1)
	{
		//WaitForSingleObject(_stLock, INFINITE);
		EnterCriticalSection(&_stLock);

		if(isWaitReturn) --_wwaitingCount;

		if(_st == RWLOCK_IDLE)
		{
			_st = RWLOCK_W;
			//ReleaseMutex(_stLock);
			LeaveCriticalSection(&_stLock);
			break;
		}
		else
		{
			++_wwaitingCount;
			ResetEvent(_ev);
			//SignalObjectAndWait(_stLock, _ev, INFINITE, FALSE);
			LeaveCriticalSection(&_stLock);
			WaitForSingleObject(_ev, INFINITE);

			isWaitReturn = true;
		}
	}
}

void CRWLock::unlock()
{
	//WaitForSingleObject(_stLock, INFINITE);
	EnterCriticalSection(&_stLock);
	if(_rlockCount > 0)
	{
		/* �������� */
		--_rlockCount;

		if( 0 == _rlockCount)
		{
			_st = RWLOCK_IDLE;

			/* �ͷ� */
			if( _wwaitingCount > 0 || _rwaitingCount > 0 )
			{
				/* 
				* ��ʱ�����������ڵȴ�,�������еȴ����߳�.(�ֶ��¼�).
				* ʹ��Щ�������¾�����.
				*/
				SetEvent(_ev);
			}
			else
			{
				/* ���� */
			}
		}
		else
		{
			/* ���ж��� */
		}
	}
	else
	{
		_st = RWLOCK_IDLE;

		/* д������ */
		if( _wwaitingCount > 0 || _rwaitingCount > 0 )
		{
			/* 
			* �����ռ�л�����_stLock�������,�����¼�,��ô���ܻ�ʹһЩ�������ܵõ���������.
			* �������unlockʱ,��һ���߳����õ���rlock����wlock.������ͷŻ�����,ֻ��֮ǰ�Ѿ��ȴ����������л�����������Ȩ.
			*/
			SetEvent(_ev);
		}
		else
		{
			/* ���� */
		}
	}
	//ReleaseMutex(_stLock);
	LeaveCriticalSection(&_stLock);
}