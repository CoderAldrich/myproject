#pragma once
#ifndef WND_PROC_HOOK_H_
#define WND_PROC_HOOK_H_

#include <Windows.h>
#include <atlstr.h>
#include <map>

#define RWLOCK_IDLE 0 /* ���� */
#define RWLOCK_R 0x01 /* ���� */
#define RWLOCK_W 0x02 /* д�� */
namespace
{
	class CRWLock
	{
	private:
		int _st; /* ��״ֵ̬ */
		int _rlockCount; /* �������� */
		int _rwaitingCount; /* ���ȴ����� */
		int _wwaitingCount; /* д�ȴ����� */
		HANDLE _ev; /* ֪ͨ�¼� Event */
		//HANDLE _stLock; /* ����״ֵ̬������ */ /* �����Ҫ�ȴ���ʱ,���� Mutex */
		CRITICAL_SECTION _stLock;

	public:
		CRWLock(void);
		~CRWLock(void);
		void rlock();
		void wlock();
		void unlock();
	};
}

typedef std::map<DWORD,HHOOK> MAP_THREAD_WINDOWS_HOOK;
typedef std::map<DWORD,HHOOK>::iterator MAP_THREAD_WINDOWS_HOOK_ITERATOR;

void BeginShieldHtmlMsg();
void EndShieldHtmlMsg();

#endif // WND_PROC_HOOK_H_

