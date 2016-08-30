// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� HTMLCXX_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// HTMLCXX_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

#include <map>
#include <string>
#include <list>

typedef struct tag_elem_feature
{
	std::string tagname;
	std::string sub_contenttext;
	std::string strattributename;
	std::string sub_attributevalue;
	std::string attributequery;
}elem_feature,*pelem_feature;

typedef std::list<std::string> list_result,*plist_result;


typedef bool (*TypeParseHtml)(const char *phtml,pelem_feature pelemfeature,plist_result presult);