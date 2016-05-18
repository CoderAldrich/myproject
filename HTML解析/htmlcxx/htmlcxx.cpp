// htmlcxx.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "htmlcxx.h"
#include "ParserDom.h"
using namespace htmlcxx;

#include <algorithm>
#include <hash_map>
#include <regex>
#include "CRC2CheckSum.h"


typedef stdext::hash_map<DWORD,char> hash_map_alone,*phash_map_alone;
typedef hash_map_alone::iterator hash_map_alone_ptr;

void makelower(std::string &str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
}

bool stringcheck(std::string tarstring,std::string restring)
{
	if ( tarstring.size() == 0 || restring.size() == 0 )
	{
		return false;
	}

	std::tr1::regex rgx(restring);

	return regex_search(tarstring,rgx);
}

bool ParseHtml(const char *phtml,pelem_feature pelemfeature,plist_result presult)
{
	if(!phtml || NULL==pelemfeature || pelemfeature->tagname.size() == 0 || NULL==presult) return false;

	tree<HTML::Node> tr;
	std::string html(phtml);
	HTML::ParserDom parser;
	parser.parse(html);
	tr = parser.getTree();

	makelower(pelemfeature->tagname);

	hash_map_alone dataalone;

	for (tree<HTML::Node>::iterator it = tr.begin();it!=tr.end();it++)
	{
		//�Ƚ�tagname
		std::string tagname = it->tagName();
		makelower(tagname);
		if (tagname != pelemfeature->tagname)
		{
			continue;
		}

		//�Ƚϱ�ǩ�ı�
		bool bctxtextmatch = true;
		if( pelemfeature->re_contenttext.size() > 0  )
		{
			bctxtextmatch = stringcheck(it->mContentText,pelemfeature->re_contenttext);
		}

		if ( false == bctxtextmatch )
		{
			continue;
		}
		
		//�Ƚ�����

		it->parseAttributes();

		bool battributematch = true;

		std::pair<bool,std::string> attrpair = it->attribute(pelemfeature->strattributename);

		if( false == stringcheck(attrpair.second,pelemfeature->re_attributevalue) )
		{
			battributematch = false;
		}

		if ( false == battributematch)
		{
			continue;
		}

		if (pelemfeature->attributequery.size() != 0 && pelemfeature->attributequery != pelemfeature->strattributename)
		{
			attrpair = it->attribute(pelemfeature->attributequery);
		}

		DWORD dwchksum = CRC32((void *)(attrpair.second.c_str()),attrpair.second.size());
		if(dataalone.find(dwchksum) == dataalone.end())
		{
			dataalone[dwchksum]='0';
			presult->push_back(attrpair.second);
		}
		else
		{
			int a=0;
		}
		
	}

	return true;
}