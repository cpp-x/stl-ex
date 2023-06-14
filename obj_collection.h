#ifndef _ObjCollection_INCLUDED_
#define _ObjCollection_INCLUDED_

#include "string_helper.h"

#define _ObjCollection_DEFINED_

using namespace std;

template< typename T1, typename T2, char ch_sep = '¤'>
class CObjCollection
{
public:
    CObjCollection()
    {
        Init();
    }

    CObjCollection(T1 item1, T2 item2, unsigned int uiId = (unsigned int)-1)
    {
        m_item1 = item1;
        m_item2 = item2;
        m_uiId = uiId;
    }

    CObjCollection(const CObjCollection& src) 
    { 
        *this = src;
    }

    CObjCollection(char ch) 
    { 
        *this = ch;
    }


#ifdef _XML_NODE_DEFINED_
    CObjCollection(XMLNode& xNode) 
    { 
        *this = xNode;
    }
#endif    

    void Init()
    {
        m_item1 = T1();
        m_item2 = T2();
        m_uiId  = 0;
    }

    void operator=(char ch) 
    { 
        m_item1 = ch;
        m_item2 = ch;
    }

    CObjCollection& operator=(const CObjCollection& src) 
    { 
        m_item1 = src.m_item1;
        m_item2 = src.m_item2;
        m_uiId    = src.m_uiId;
        return *this;
    }

    CObjCollection& operator=(const string& src) 
    { 
        string strTmp;
        string_helper::string_substring(strTmp,src,0,ch_sep);
        m_item1 = strTmp;
        string_helper::string_substring(strTmp,src,1,ch_sep);
        m_item2 = strTmp;
        string_helper::string_substring(strTmp,src,2,ch_sep);
        m_uiId = atol(strTmp.c_str());
        return *this;
    }

#ifdef _XML_NODE_DEFINED_
    CObjCollection& operator=(XMLNode& xNode)
    {
        if (xNode.isEmpty())
            return *this;

        m_item1 = xNode.getAttributeStr("name");
        m_item2 = (char)NULL;
        m_uiId  = 0;
        return *this;
    }
#endif

    operator string() const
    {
        string strTmp;
        strTmp = string_helper::string_format("%s%c%s",(string)m_item1,ch_sep,(string)m_item2);
        return strTmp;
    }

    bool operator==(CObjCollection& src) 
    { 
        if (m_item1 == src.m_item1)
            if (m_item2 == src.m_item2)
                return true;
        return false;
    }

    T1    m_item1;
    T2    m_item2;
    unsigned int m_uiId;
};

#define _typeStringPair_DEFINED_

typedef CObjCollection<string,string>            typeStringPair;

inline void ToValue(const char* strValue,typeStringPair& pair)
{
    string strTmp;
    string_helper::string_substring(pair.m_item1,strValue,0,'¤');
    string_helper::string_substring(pair.m_item2,strValue,1,'¤');
    string_helper::string_substring(strTmp,strValue,2,'¤');
    pair.m_uiId = (unsigned int)strtol(strValue, NULL, (strchr(strValue,'x') != NULL)?16:10);
}

inline void ToValue(string& strValue, typeStringPair& pair)
{
    ToValue(strValue.c_str(), pair);
}

inline string ToString(typeStringPair& pair)
{
    return string_helper::string_format("%s¤%s¤%d",pair.m_item1.c_str(),pair.m_item2.c_str(),pair.m_uiId);
}

inline bool _AfxComparePath(typeStringPair lpszPath1, typeStringPair lpszPath2)
{
    return false;
}


#endif
