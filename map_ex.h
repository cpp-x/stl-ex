#ifndef _STL_CMAP_EX
#define _STL_CMAP_EX

#include "stl_clist.h"
#include "stl_cmap.h"
#include "xml_parser.h"

inline void ToKey(string& strKey,long& key)
{
    key = atol(strKey.c_str());
}

template<class KEY, class VALUE>
class CMapEx : public CMapSTL<KEY, VALUE>
{
public:
    typedef CMapSTL<KEY, VALUE> baseClass;
    typedef CMapEx<KEY, VALUE>  thisClass;
    
    typedef typename std::map<KEY,VALUE>::iterator POSITION;
    typedef typename std::map<KEY,VALUE>::reverse_iterator RPOSITION;

    bool LookupValue(VALUE val,KEY& rKey);

    CMapEx<KEY,VALUE>& operator+=(CMapEx<KEY,VALUE>& src);
    CMapEx<KEY,VALUE>& operator=(CMapEx<KEY,VALUE>& src);

    string ToString();
    void FromString(string& strMap);

    /*
    ARG_VALUE* LookupPtr(ARG_KEY key);

    bool RemoveKeyPtr(ARG_KEY key);
    void RemoveAllPtr();
    */

#ifdef _XML_NODE_DEFINED_
    int operator=(XMLNode& xRoot);
    int operator+=(XMLNode& xRoot);
    int operator<<(XMLNode& xRoot);
    void operator>>(XMLNode& xRoot);
    void ToXML(XMLNode& xParent);
    string ToXML();

    string m_xmlName;
#endif

};

template<class KEY, class VALUE>
bool CMapEx<KEY,VALUE>::LookupValue(VALUE val,KEY& rKey)
{
    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = pT->GetStartPosition();
    while(pos)
    {
        KEY key;
        VALUE value;
        GetNextAssoc(pos, key,value);
        if (value == val)
        {
            rKey = key;
            return true;
        }
    }
    return false;
}

template<class KEY, class VALUE>
string CMapEx<KEY,VALUE>::ToString()
{
    thisClass* pT = static_cast<thisClass*>(this);

    string strMap;
    POSITION pos = pT->GetStartPosition();
    while(pos)
    {
        KEY key;
        VALUE value;
        GetNextAssoc(pos, key,value);
        strMap = strMap + "," + ::ToString(key)+"="+::ToString(value);        
    }
    return strMap;
}

template<class KEY, class VALUE>
void CMapEx<KEY,VALUE>::FromString(string& strMap)
{
    thisClass* pT = static_cast<thisClass*>(this);

    string strMapLoc = strMap;
    if (strMapLoc.size())
        if (strMapLoc.at(0) != ',')
            strMapLoc = ','+strMapLoc;
    string strTmp,strKey,strValue;
    long count = 1;
    KEY key;
    VALUE value;
    while(true)
    {
        string_helper::string_substring(strTmp,strMapLoc,count,',');
        if (!strTmp.size())
            break;
        strTmp = "="+strTmp;
        string_helper::string_substring(strKey,strTmp,1,'=');
        string_helper::string_substring(strValue,strTmp,2,'=');
        ToKey(strKey,key);
        ToValue(strValue,value);
        pT->SetAt(key,value);
        count++;
    }
}

template<class KEY, class VALUE>
CMapEx<KEY,VALUE>& CMapEx<KEY,VALUE>::operator+=(CMapEx<KEY,VALUE>& src)
{
    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = src.GetStartPosition();
    while(src.is_not_end(pos))
    {
        KEY key;
        VALUE value;
        src.GetNextAssoc(pos, key, value);
        pT->SetAt(key,value);
    }
    return *this;
}

template<class KEY, class VALUE>
CMapEx<KEY,VALUE>& CMapEx<KEY,VALUE>::operator=(CMapEx<KEY,VALUE>& src)
{
    thisClass* pT = static_cast<thisClass*>(this);

    pT->RemoveAll();
    (*this) += src;
    return *this;
}

/*
template<class KEY, class VALUE>
bool CMapEx<KEY,VALUE>::RemoveKeyPtr(KEY key)
{
    VALUE val;
    if (Lookup(key,val))
        if (val)
        {
            try{
            delete val;
            }catch(...){}
        }
    return RemoveKey(key);
}

template<class KEY, class VALUE>
ARG_VALUE* CMapEx<KEY,VALUE>::LookupPtr(ARG_KEY key)
{
    CPair* pPair = PLookup(key);
    if (pPair)
        return &pPair->value;
    return NULL;
}

template<class KEY, class VALUE>
void CMapEx<KEY,VALUE>::RemoveAllPtr()
{
    POSITION pos = GetStartPosition();
    while(pos)
    {
        KEY key;
        VALUE value;
        GetNextAssoc(pos, key, value);
        if (value)
        {
            try{
            delete value;
            }catch(...){}
        }
    }
    RemoveAll();
}
*/
#ifdef _XML_NODE_DEFINED_

template<class KEY, class VALUE>
int CMapEx<KEY,VALUE>::operator+=(XMLNode& xRoot)
{
    int nChildNode = xRoot.nChildNode();
    for (int nodeCnt = 0; nodeCnt < nChildNode; nodeCnt++)
    {
        XMLNode xEntry = xRoot.getChildNode(nodeCnt);
        if (strcmp(xEntry.getName(),"member"))
            continue;

        KEY key;
        VALUE value;
        string sKey = xEntry.getAttributeStr("key");
        string sValue = xEntry.getAttributeStr("value");
        ToKey(sKey,key);
        ToValue(sValue,value);
        SetAt(key,value);

    }
    m_xmlName = xRoot.getName();

    thisClass* pT = static_cast<thisClass*>(this);
    return pT->GetCount();
}

template<class KEY, class VALUE>
int CMapEx<KEY,VALUE>::operator=(XMLNode& xRoot)
{
    if (xRoot.getChildNode("member").isEmpty())
        return 0;

    thisClass* pT = static_cast<thisClass*>(this);

    //while(m_bLock);

    pT->RemoveAll();

    (*this) += xRoot;

    return pT->GetCount();
}

template<class KEY, class VALUE>
void CMapEx<KEY,VALUE>::ToXML(XMLNode& xParent)
{
    if (m_xmlName.size())
        xParent.setName(stringDup(m_xmlName.c_str()));

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = pT->GetStartPosition();
    while(pos)
    {
        KEY key;
        VALUE value;
        GetNextAssoc(pos, key, value);
        XMLNode xNodeMember = xParent.addChild(stringDup("member"));
        xNodeMember.addAttribute(stringDup("key"),stringDup(::ToString(key)));
        xNodeMember.addAttribute(stringDup("value"),stringDup(::ToString(value)));
    }
}

template<class KEY, class VALUE>
int CMapEx<KEY,VALUE>::operator<<(XMLNode& xParent)
{
    thisClass* pT = static_cast<thisClass*>(this);

    (*this) = xParent;
    return pT->GetCount();
}

template<class KEY, class VALUE>
void CMapEx<KEY,VALUE>::operator>>(XMLNode& xParent)
{
    ToXML(xParent);
}

template<class KEY, class VALUE>
string CMapEx<KEY,VALUE>::ToXML()
{
    XMLNode xSymDefRoot;
    xSymDefRoot = XMLNode::createXMLTopNode();    

    ToXML(xSymDefRoot);

    string xmlString;
    xSymDefRoot.CreateXMLString(xmlString);
    return xmlString;
}

#endif

#ifndef _WIN32
 #define lstrcmpi       strcasecmp
#endif

template<typename T>
int CompareItems(T* p_item1,T* p_item2, bool bNoCase = false)
{
    if (bNoCase)
        return lstrcmpi((LPCSTR)(*p_item1), (LPCSTR)(*p_item2));
            
    return (*p_item1) > (*p_item2);
}

typedef CMapEx<string,string>   CMapExString;
typedef CMapEx<string,void*>    CMapExString2Ptr;

#endif
