// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� HTMLCXX_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// HTMLCXX_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef HTMLCXX_EXPORTS
#define export __declspec(dllexport)
#else
#define HTMLCXX_API __declspec(dllimport)
#endif
#include <list>
#include <string>
#include <algorithm>
using namespace std;

EXTERN_C
{
	export bool ParseHtml(const char *phtml,const char *ptagname,const char *pattrname,const char *pattrrule,list<string> *plistres);
};
