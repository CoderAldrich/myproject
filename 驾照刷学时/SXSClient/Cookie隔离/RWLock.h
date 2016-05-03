#pragma once

#define RWLOCK_IDLE 0 /* ���� */
#define RWLOCK_R 0x01 /* ���� */
#define RWLOCK_W 0x02 /* д�� */

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
