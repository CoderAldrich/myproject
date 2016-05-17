// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 HTMLCXX_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// HTMLCXX_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

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