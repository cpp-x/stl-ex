#ifndef _CMAP_EX
#define _CMAP_EX

#include "list_ex.h"
#include "stl_carray.h"
#include "xml_parser.h"

////////////////////////////////////////////////////////////////////////////////////////////////
template<class TYPE>
class CArrayEx : public CArraySTL<TYPE>
{
    typedef CArraySTL<TYPE>    TBase;

    bool    m_bLock;
public:
    
    CArrayEx(char chSep = ',') :CArraySTL<TYPE>()
    {
        m_bLock = false;
        m_chSep = chSep;
    };
    CArrayEx(CArrayEx<TYPE>& src);
    
    void SetLock(bool bLock = true){m_bLock = bLock;};
    void ReleaseLock(){m_bLock = false;};
    bool IsLocked(){return m_bLock;};

    int    Find(TYPE value);

    bool Add(TYPE value);
    bool AddUnique(TYPE value);
    bool Remove(TYPE value);
    string ToString();
    void FromString(string& str, bool bInit = true);
    CArrayEx<TYPE>& operator=(string& str);
    CArrayEx<TYPE>& operator+=(string& str);
    CArrayEx<TYPE>& operator=(CArrayEx<TYPE>& src);
    CArrayEx<TYPE>& operator=(CListEx<TYPE>& src);
    
#ifdef _XML_NODE_DEFINED_
    int operator=(XMLNode& xRoot);
    int operator+=(XMLNode& xRoot);
    int operator<<(XMLNode& xRoot);
    void operator>>(XMLNode& xRoot);
    void ToXML(XMLNode& xParent);
    string ToXML();
#endif

    char m_chSep;

#ifdef _XML_NODE_DEFINED_
    string m_xmlName;
#endif
};

template<class TYPE>
CArrayEx<TYPE>::CArrayEx(CArrayEx<TYPE>& src)
{
    *this = src;
}

template<class TYPE>
int    CArrayEx<TYPE>::Find(TYPE value)
{
    for (int cnt = 0; cnt < TBase::GetCount(); cnt++)
        if (TBase::GetAt(cnt) == value)
            return cnt;
    return -1;
}

template<class TYPE>
bool CArrayEx<TYPE>::Add(TYPE value)
{
    if (m_bLock)
        return false;
    TBase::Add(value);
    return true;
};

template<class TYPE>
bool CArrayEx<TYPE>::AddUnique(TYPE value)
{
    if (Find(value) != -1)
        return false;
    Add(value);
    return true;
};

template<class TYPE>
bool CArrayEx<TYPE>::Remove(TYPE value)
{
    int posItem = Find(value);
    if (posItem != -1)
    {
        TBase::RemoveAt(posItem);
        return true;
    }
    
    return false;
};

template<class TYPE>
void CArrayEx<TYPE>::FromString(string& str, bool bInit)
{
    if (bInit)
        TBase::RemoveAll();
    string strTmp,strValue;
    long count = 0;
    TYPE value;
    while(true)
    {
        string_helper::string_substring(strTmp,str,count,m_chSep);
        if (!strTmp.size())
            break;
        ToValue(strTmp,value);
        Add(value);
        count++;
    }
}

template<class TYPE>
string CArrayEx<TYPE>::ToString()
{
    string str;
    TYPE value;
    for (int cnt = 0; cnt < TBase::GetCount(); cnt++)
    {
        value = TBase::GetAt(cnt);
        if (!str.size())
            str += ::ToString(value);
        else
            str +=  m_chSep + ::ToString(value);
    }
    return str;
}

template<class TYPE>
CArrayEx<TYPE>& CArrayEx<TYPE>::operator=(CArrayEx<TYPE>& src)
{
    TBase::RemoveAll();
    for (int cnt = 0; cnt < src.GetCount(); cnt++)
        Add(src.GetAt(cnt));
    return *this;
}

template<class TYPE>
CArrayEx<TYPE>& CArrayEx<TYPE>::operator=(CListEx<TYPE>& src)
{
    TBase::RemoveAll();

    typename CListEx<TYPE>::POSITION pos;
    pos = src.GetHeadPosition();

    while(pos)
        Add(src.GetNext(pos));
        
    return *this;
}

template<class TYPE>
CArrayEx<TYPE>& CArrayEx<TYPE>::operator=(string& src)
{
    FromString(src);
    return *this;
}

template<class TYPE>
CArrayEx<TYPE>& CArrayEx<TYPE>::operator+=(string& src)
{
    FromString(src,false);
    return *this;
}

#ifdef _XML_NODE_DEFINED_

template<class TYPE>
int CArrayEx<TYPE>::operator+=(XMLNode& xRoot)
{
    int nChildNode = xRoot.nChildNode();
    for (int nodeCnt = 0; nodeCnt < nChildNode; nodeCnt++)
    {
        XMLNode xEntry = xRoot.getChildNode(nodeCnt);
        if (strcmp(xEntry.getName(),"member"))
            continue;

        TYPE item = xEntry;
        Add(item);
    }
    m_xmlName = xRoot.getName();
    return TBase::GetCount();
}

template<class TYPE>
int CArrayEx<TYPE>::operator=(XMLNode& xRoot)
{
    if (xRoot.getChildNode("member").isEmpty())
        return 0;

    while(m_bLock);

    TBase::RemoveAll();

    (*this) += xRoot;

    return TBase::GetCount();
}

template<class TYPE>
int CArrayEx<TYPE>::operator<<(XMLNode& xParent)
{
    int nChildNode = xParent.nChildNode();

    if (!nChildNode)
        return 0;

    while(m_bLock);

    TBase::RemoveAll();
    
    for (int nodeCnt = 0; nodeCnt < nChildNode; nodeCnt++)
    {
        XMLNode xMember = xParent.getChildNode(nodeCnt);
        if (strcmp(xMember.getName(),"member"))
            continue;

        TYPE item;
        string strTmp = xMember.getAttributeStr("value");
        item = strTmp;
        Add(item);
    }
    m_xmlName = xParent.getName();

    return TBase::GetCount();
}

template<class TYPE>
void CArrayEx<TYPE>::operator>>(XMLNode& xParent)
{
    if (m_xmlName.size())
        xParent.setName(stringDup(m_xmlName.c_str()));

    for (int cnt = 0; cnt < TBase::GetCount(); cnt++)
        xParent.addChild(stringDup("member")).addAttribute(stringDup("value"),stringDup(::ToString((*this)[cnt])));
}

template<class TYPE>
void CArrayEx<TYPE>::ToXML(XMLNode& xParent)
{
    if (m_xmlName.size())
        xParent.setName(stringDup(m_xmlName.c_str()));

    for (int cnt = 0; cnt < TBase::GetCount(); cnt++)
        (*this)[cnt].ToXML(xParent.addChild(stringDup("member")));
}

template<class TYPE>
string CArrayEx<TYPE>::ToXML()
{
    XMLNode xSymDefRoot;
    xSymDefRoot = XMLNode::createXMLTopNode();    
    
    ToXML(xSymDefRoot);

    string xmlString;
    xSymDefRoot.CreateXMLString(xmlString);
    return xmlString;
}

#endif

class CStringArrayEx : public CArrayEx<string>
{
public:

    CStringArrayEx& operator=(CStringListEx& src)
    {
        RemoveAll();
        return (*this) = src;
    }

    CStringArrayEx& operator+=(CStringListEx& src)
    {
        CStringListEx::POSITION pos = src.GetHeadPosition();
        while(src.is_not_end(pos))
            Add(src.GetNext(pos));

        return *this;
    }
};

#endif