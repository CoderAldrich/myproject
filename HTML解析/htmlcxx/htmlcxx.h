// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� HTMLCXX_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// HTMLCXX_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

#include <map>
#include <string>
#include <list>

typedef struct tag_attribute_feature
{
	std::string strattributename;
	std::string re_attributevalue;
}attribute_feature,*pattribute_feature;

typedef std::list<attribute_feature>  list_attribute_feature,*plist_attribute_feature;
typedef list_attribute_feature::iterator list_attribute_feature_ptr;

typedef struct tag_elem_feature
{
	std::string tagname;
	std::string re_contenttext;
	list_attribute_feature attributefeature;
}elem_feature,*pelem_feature;

typedef std::list<std::string> list_result,*plist_result;


typedef bool (*TypeParseHtml)(const char *phtml,pelem_feature pelemfeature,std::string attributequery,plist_result presult);