#pragma once

/*
���ܣ����̴���֪ͨ�Ļص�������ʽ����
������
DWORD dwParentID     �����̵�ID
DWORD dwProcessID    ����ID
LPCWSTR pszProcessPath  ����·��
BOOL bCreate           �Ƿ�Ϊ����֪ͨ  TRUE ����֪ͨ FALSE ���̽���֪ͨ
����ֵ����

ע�⣺�˻ص��������뾡�췵�أ�������ܻ��ʧ�������̴���֪ͨ������ͨ�����л���֪ͨ����Ȼ�����δ���
*/
typedef VOID (CALLBACK *RROCESS_CREATE_CALLBACK)(DWORD dwParentID,DWORD dwProcessID,LPCWSTR pszProcessPath,BOOL bCreate);

typedef BOOL (WINAPI *TypeSetProcessCreateCallBack)(RROCESS_CREATE_CALLBACK pCallBack);