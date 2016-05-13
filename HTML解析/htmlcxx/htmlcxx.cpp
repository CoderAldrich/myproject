// htmlcxx.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "htmlcxx.h"
#include "ParserDom.h"
using namespace htmlcxx;

#include <atlstr.h>
void makelower(string &str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
}
void DeletList(list<string> *plistres)
{
	if(plistres)
	{
		delete plistres;
	}
}
bool ParseHtml(const char *phtml,const char *ptagname,const char *pattrname,const char *pattrrule,list<string> **pplistres)
{
	if(!phtml || !ptagname || !pattrname || !pplistres) return false;

	tree<HTML::Node> tr;
	string html(phtml);
	HTML::ParserDom parser;
	parser.parse(html);
	tr = parser.getTree();

	list<string> *plistres = new list<string>;

	string targettagname(ptagname);
	string targetattrname(pattrname);
	string targetattrrule(pattrrule);
	
	makelower(targettagname);
	makelower(targetattrname);
	makelower(targetattrrule);

	for (tree<HTML::Node>::iterator it = tr.begin();it!=tr.end();it++)
	{
		try
		{
			string tagname = it->tagName();
			makelower(tagname);
			if (tagname == targettagname)
			{
				it->parseAttributes();
				pair<bool,string> attrpair = it->attribute(targetattrname);
				if (attrpair.first)
				{
					string attrval=attrpair.second;
					makelower(attrval);
					int findres = attrval.find(targetattrrule);
					if( findres >= 0)
					{
						//OutputDebugStringW(CString(attrpair.second.c_str())+TEXT("\n"));
						plistres->push_back(attrpair.second);
					}
				}
			}
		}
		catch( ... )
		{
			int a=0;
		}

	}

	*pplistres = plistres;

	return true;
}