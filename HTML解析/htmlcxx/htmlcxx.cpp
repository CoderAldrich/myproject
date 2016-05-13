// htmlcxx.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "htmlcxx.h"
#include "ParserDom.h"
using namespace htmlcxx;

#include <algorithm>
#include <hash_map>
#include "CRC2CheckSum.h"


typedef stdext::hash_map<DWORD,char> hash_map_alone,*phash_map_alone;
typedef hash_map_alone::iterator hash_map_alone_ptr;

void makelower(std::string &str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
}

bool stringcheck(std::string tarstring,std::string teststring,bool bfullmatch)
{
	if ( tarstring.size() == 0 || teststring.size() == 0 )
	{
		return false;
	}

	makelower(tarstring);
	makelower(teststring);
	
	if ( bfullmatch )
	{
		return tarstring == teststring;
	}
	else
	{
		int nres = tarstring.find(teststring);
		return nres>=0;
	}



}

bool ParseHtml(const char *phtml,pelem_feature pelemfeature,std::string attributequery,plist_result presult)
{
	if(!phtml || NULL==pelemfeature || pelemfeature->tagname.size() == 0 || attributequery.size() == 0 ||NULL==presult) return false;

	tree<HTML::Node> tr;
	std::string html(phtml);
	HTML::ParserDom parser;
	parser.parse(html);
	tr = parser.getTree();

	makelower(pelemfeature->tagname);

	hash_map_alone dataalone;

	for (tree<HTML::Node>::iterator it = tr.begin();it!=tr.end();it++)
	{
		//比较tagname
		std::string tagname = it->tagName();
		makelower(tagname);
		if (tagname != pelemfeature->tagname)
		{
			continue;
		}

		//比较标签文本
		bool bctxtextmatch = true;
		if( pelemfeature->contenttext.strfeature.size() > 0  )
		{
			bctxtextmatch = stringcheck(it->mContentText,pelemfeature->contenttext.strfeature,pelemfeature->contenttext.bfullmatch);
		}

		if ( false == bctxtextmatch )
		{
			continue;
		}
		
		//比较属性

		it->parseAttributes();

		bool battributematch = true;

		for (list_attribute_feature_ptr fit = pelemfeature->attributefeature.begin();fit!=pelemfeature->attributefeature.end();fit++)
		{
			std::pair<bool,std::string> attrpair = it->attribute(fit->strattributename);

			if( false == stringcheck(attrpair.second,fit->strattributevalue,fit->bfullmatch) )
			{
				battributematch = false;
				break;
			}
		}

		if ( false == battributematch)
		{
			continue;
		}

		std::pair<bool,std::string> attrpair = it->attribute(attributequery);
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