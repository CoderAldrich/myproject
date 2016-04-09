#pragma once

#define RWLOCK_IDLE 0 /* 空闲 */
#define RWLOCK_R 0x01 /* 读锁 */
#define RWLOCK_W 0x02 /* 写锁 */

class CRWLock
{
private:
	int _st; /* 锁状态值 */
	int _rlockCount; /* 读锁计数 */
	int _rwaitingCount; /* 读等待计数 */
	int _wwaitingCount; /* 写等待计数 */
	HANDLE _ev; /* 通知事件 Event */
	//HANDLE _stLock; /* 访问状态值互斥量 */ /* 如果需要等待超时,则用 Mutex */
	CRITICAL_SECTION _stLock;

public:
	CRWLock(void);
	~CRWLock(void);
	void rlock();
	void wlock();
	void unlock();
};
