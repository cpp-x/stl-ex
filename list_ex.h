#ifndef _STL_CLIST_EX
#define _STL_CLIST_EX

#include "obj_collection.h"
#include "stl_clist.h"
#include "lock_helper.h"
#include "xml_parser.h"

inline string ToString(string& str)
{
    return str;
}

inline string ToString(int val)
{
    char buf[32] = "";
    sprintf(buf,"%d",val);
    return buf;
}

inline string ToString(unsigned int val)
{
    char buf[32] = "";
    sprintf(buf,"%d",val);
    return buf;
}

inline void ToValue(string& strValue,int& value)
{
    value = atol(strValue.c_str());
}

inline void ToValue(string& strValue,unsigned int& value)
{
    value = atol(strValue.c_str());
}

inline void ToValue(string& strValue,string& value)
{
    value = strValue;
}

inline void ToValue(char* lpstrValue, int& value)
{
    value = atol(lpstrValue);
}

inline void ToValue(char* lpstrValue, unsigned int& value)
{
    value = atol(lpstrValue);
}

inline void ToValue(char* lpstrValue, string& value)
{
    value = lpstrValue;
}

inline void ToValue(string& strValue, bool& value)
{
    value = (atol(strValue.c_str()) != 0);
}

typedef int (lpfnCompareList)(void* p_item1,void* p_item2, bool bNoCase);

template<class TYPE>
class CListEx : public CListSTL<TYPE>
{
protected:

#ifdef _WIN32    
    lock_helper::CLockKey_CS         m_cs;
#else
    lock_helper::CLockKey_Thread     m_cs;
#endif

public:

    typedef CListSTL<TYPE>    baseClass;
    typedef CListEx<TYPE>     thisClass;

    typedef typename std::list<TYPE>::iterator POSITION;
    typedef typename std::list<TYPE>::reverse_iterator RPOSITION;
    
    CListEx(char chSep = '¤') :baseClass()
    {
        m_lpFnCompare = NULL;
        m_chSep = chSep;
    };
    CListEx(string& string)
    {
        m_lpFnCompare = NULL;
        m_chSep = '¤';
        *this = string;
    }
    CListEx(CListEx<TYPE>& src)
    {
        m_lpFnCompare = NULL;
        m_chSep = '¤';
        *this = src;
    }
    #ifdef _WIN32
    void SetLock(bool bLock = true){if (bLock) m_cs.lock(); else m_cs.unlock();};
    void ReleaseLock(){m_cs.unlock();};
    #else
    void SetLock(bool bLock = true){if (bLock) m_cs.lock(); else m_cs.unlock();};
    void ReleaseLock(){m_cs.unlock();};
    #endif

    POSITION AddTail(const TYPE& value, bool bLock = true)
    {
        lock_helper::CAutoLock lock(m_cs, bLock);

        thisClass* pT = static_cast<thisClass*>(this);
        
        POSITION pos = pT->m_list.end();
        try
        {
            pos = baseClass::AddTail((TYPE&)*(&value));
        }
        catch(...)
        {
        }
        return pos;
    };

    POSITION AddHead(const TYPE& value, bool bLock = true)
    {
        lock_helper::CAutoLock lock(m_cs, bLock);

        thisClass* pT = static_cast<thisClass*>(this);

        POSITION pos = pT->m_list.end();
        try
        {
            pos = baseClass::AddHead((TYPE&)*(&value));
        }
        catch(...)
        {
        }
        return pos;
    };

    bool AddTailUnique(TYPE& value, bool bLock = true, bool bPathCompare = false);
    bool AddTailUnique(CListEx<TYPE>& src, bool bLock = true, bool bPathCompare = false);
    bool AddHeadUnique(TYPE& value, bool bLock = true, bool bPathCompare = false);
    bool AddHeadUnique(CListEx<TYPE>& src, bool bLock = true, bool bPathCompare = false);
    bool Remove(TYPE& value, bool bLock = true);
    TYPE RemoveHead(bool bLock = true);
    void Swap(POSITION& pos1, POSITION& pos2);
    string ToString();
    void FromString(string& str, bool bInit = true);
    TYPE GetAtIndex(long index);
    int  GetIndex(POSITION& pos);
    void Sort(bool bAsc = true, bool bNoCase = false);
    TYPE operator[](long index) { return GetAtIndex(index);};
    CListEx<TYPE>& operator=(char ch) {return *this;};
    CListEx<TYPE>& operator=(string& str);
    CListEx<TYPE>& operator+=(string& str);
    CListEx<TYPE>& operator=(CListEx<TYPE>& src);
    CListEx<TYPE>& operator+=(CListEx<TYPE>& src);

#ifdef _XML_NODE_DEFINED_
    int operator=(XMLNode& xRoot);
    int operator+=(XMLNode& xRoot);
    int operator<<(XMLNode& xRoot);
    void operator>>(XMLNode& xRoot);
    void ToXML(XMLNode& xParent);
    string ToXML();
#endif

    char m_chSep;
    lpfnCompareList*    m_lpFnCompare;

    void Trim(char ch = ' ');

    bool Push(TYPE& value);
    bool Pop(TYPE& value);
    bool Top(TYPE& value);

    POSITION FindPath(TYPE& value, bool bLock = true)
    {
        thisClass* pT = static_cast<thisClass*>(this);
        return pT->m_list.end();
    }

    RPOSITION ReverseFind(TYPE& searchValue, bool bLock = true)
    {
        lock_helper::CAutoLock lock(m_cs, bLock);

        thisClass* pT = static_cast<thisClass*>(this);
        
        for (RPOSITION rpos = pT->m_list.rbegin(); rpos != pT->m_list.rend(); rpos++)
            if (*rpos == searchValue)
                return rpos;

        return pT->m_list.rend();
    }

    RPOSITION ReverseFind(TYPE& searchValue, RPOSITION& startBefore, bool bLock = true)
    {
        lock_helper::CAutoLock lock(m_cs, bLock);

        thisClass* pT = static_cast<thisClass*>(this);

        for (RPOSITION rpos = startBefore; rpos != pT->m_list.rend(); rpos++)
            if (*rpos == searchValue)
                return rpos;

        return pT->m_list.rend();
    }

    //POSITION FindId(int id);
    
protected:
#ifdef _XML_NODE_DEFINED_
    string m_xmlName;
#endif
};

template<class TYPE>
bool CListEx<TYPE>::AddTailUnique(TYPE& value, bool bLock, bool bPathCompare)
{
    lock_helper::CAutoLock lock(m_cs, bLock);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION posItem = pT->m_list.end();
    if (!bPathCompare)
        posItem = pT->Find(value);
    else
        posItem = FindPath(value, false);

    if (pT->is_not_end(posItem))
        return false;
    
    AddTail(value, false);
    
    return true;
};

template<class TYPE>
bool CListEx<TYPE>::AddTailUnique(CListEx<TYPE>& src, bool bLock, bool bPathCompare)
{
    lock_helper::CAutoLock lock(m_cs, bLock);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION posSrc = src.GetHeadPosition();
    while(is_not_end(posSrc))
    {
        TYPE* p_value = &src.GetNext(posSrc);
        POSITION posItem = pT->m_list.end();
        if (!bPathCompare)
            posItem = pT->Find(*p_value);
        else
            posItem = FindPath(*p_value, false);

        if (pT->is_not_end(posItem))
            continue;

        AddTail(*p_value, false);
    }
    
    return true;
};

template<class TYPE>
bool CListEx<TYPE>::AddHeadUnique(TYPE& value, bool bLock, bool bPathCompare)
{
    lock_helper::CAutoLock lock(m_cs, bLock);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION posItem = pT->m_list.end();
    if (!bPathCompare)
        posItem = pT->Find(value);
    else
        posItem = FindPath(value, false);

    if (pT->is_not_end(posItem))
        return false;

    AddHead(value,false);
    
    return true;
};

template<class TYPE>
bool CListEx<TYPE>::AddHeadUnique(CListEx<TYPE>& src, bool bLock, bool bPathCompare)
{
    lock_helper::CAutoLock lock(m_cs, bLock);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION posSrc = src.GetHeadPosition();
    while(is_not_end(posSrc))
    {
        TYPE* p_value = &src.GetNext(posSrc);
        POSITION posItem = pT->m_list.end();
        if (!bPathCompare)
            posItem = pT->Find(*p_value);
        else
            posItem = FindPath(*p_value, false);

        if (pT->is_not_end(posItem))
            continue;

        AddHead(*p_value, false);
    }

    return true;
};

#if 0
/*
template<class TYPE>
POSITION CListEx<TYPE>::ReverseFind(TYPE& searchValue, POSITION startBefore) const
{
    ASSERT_VALID(this);

    CNode* pNode = (CNode*) startBefore;
    if (pNode == NULL)
    {
        pNode = m_pNodeTail;  // start at tail
    }
    else
    {
        ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));
        pNode = pNode->pPrev;  // start after the one specified
    }

    for (; pNode != NULL; pNode = pNode->pPrev)
        if (CompareElements<TYPE>(&pNode->data, &searchValue))
            return (POSITION)pNode;
    return NULL;
}


template<class TYPE>
POSITION CListEx<TYPE>::FindPath(TYPE& value, bool bLock)
{
    lock_helper::CAutoLock lock(m_cs, bLock);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = pT->GetHeadPosition();
    while(pos)
    {
        POSITION currPos = pos;
        if (_AfxComparePath(value,GetNext(pos)))
            return currPos;
    }
    return NULL;
}

template<class TYPE>
POSITION CListEx<TYPE>::FindId(int id)
{
    lock_helper::CAutoLock lock(m_cs);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = pT->GetHeadPosition();
    while(pos)
    {
        POSITION currPos = pos;
        if (GetNext(pos).m_id == id)
            return currPos;
    }
    return NULL;
}
*/
#endif

template<class TYPE>
bool CListEx<TYPE>::Remove(TYPE& value, bool bLock)
{
    lock_helper::CAutoLock lock(m_cs, bLock);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION posItem = pT->m_list.end();
    try
    {
        posItem = pT->Find(value);
        if (posItem)
            RemoveAt(posItem);
    }
    catch(...){}
    
    return posItem != pT->m_list.end();;
};

template<class TYPE>
TYPE CListEx<TYPE>::RemoveHead(bool bLock)
{
    lock_helper::CAutoLock lock(m_cs, bLock);

    TYPE headItem;
    try
    {
        baseClass::RemoveHead(headItem);
    }
    catch(...){}

    return headItem;
}

template<class TYPE>
void CListEx<TYPE>::Swap(POSITION& pos1, POSITION& pos2)
{
    thisClass* pT = static_cast<thisClass*>(this);

    if (pT->GetCount() < 2) 
        return;

    /*
    CNode* pNode1 = (CNode*)pos1;
    ASSERT(AfxIsValidAddress(pNode1, sizeof(CNode)));
    CNode* pNode2 = (CNode*)pos2;
    ASSERT(AfxIsValidAddress(pNode2, sizeof(CNode)));

    TYPE argTmp = pNode1->data;
    pNode1->data = pNode2->data;
    pNode2->data = argTmp;
    */
}

template<class TYPE>
string CListEx<TYPE>::ToString()
{
    lock_helper::CAutoLock lock(m_cs);

    string str;
    TYPE& value;

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = pT->GetHeadPosition();
    while (pos)
    {
        value = GetNext(pos);
        if (str.size() == 0)
            str += ::ToString(value);
        else
            str +=  m_chSep + ::ToString(value);
    }
    return str;
}

template<class TYPE>
void CListEx<TYPE>::FromString(string& str, bool bInit)
{
    lock_helper::CAutoLock lock(m_cs);
        
    thisClass* pT = static_cast<thisClass*>(this);

    if (bInit)
        pT->RemoveAll();

    TYPE value;

    char* lpEntry = (char*)str.c_str();
    char* lpStrEnd = lpEntry + str.size(); 
    while((*lpEntry) && (lpEntry < lpStrEnd))
    {
        char* lpStrSep = strchr(lpEntry,m_chSep);
        if (!lpStrSep) 
        {
            ToValue(lpEntry,value);
            AddTail(value, false);   
            break;
        }
        
        char ch = *lpStrSep;
        *lpStrSep = 0;          
        ToValue(lpEntry,value);
        *lpStrSep = ch;          
        AddTail(value, false);        
        lpEntry = lpStrSep + 1;
    }
}

template<class TYPE>
TYPE CListEx<TYPE>::GetAtIndex(long index)
{
    lock_helper::CAutoLock lock(m_cs);

    thisClass* pT = static_cast<thisClass*>(this);

    TYPE val = (char)NULL;
    if (index < pT->GetCount())
        return pT->GetAt(pT->FindIndex(index));
    return val;
}

template<class TYPE>
int CListEx<TYPE>::GetIndex(POSITION& pos)
{
    if (!pos)
        return -1;

    lock_helper::CAutoLock lock(m_cs);

    thisClass* pT = static_cast<thisClass*>(this);

    int idx = 0;
    POSITION posSearch = pT->GetHeadPosition();
    while(posSearch)
    {
        if (posSearch == pos)
            return idx;
        idx++;
        GetNext(posSearch);
    }

    return -1;
}

template<class TYPE>
void CListEx<TYPE>::Sort(bool bAsc,bool bNoCase)
{
    lock_helper::CAutoLock lock(m_cs);

    thisClass* pT = static_cast<thisClass*>(this);

    int size = pT->GetCount();
    for ( int pass = 1; pass < size; ++pass )
        for ( int j = 0; j < (size - pass); ++j )
        {
            POSITION pos_item1 = NULL;
            POSITION pos_item2 = NULL;
            TYPE* p_item1 = &GetAt(pos_item1 = pT->FindIndex(j));
            TYPE* p_item2 = &GetAt(pos_item2 = pT->FindIndex(j+1));
            int comp = 0;
            if (m_lpFnCompare)                
                comp = ((*m_lpFnCompare)((void*)p_item1,(void*)p_item2,bNoCase)) * (bAsc?1:-1);
            else
                comp = CompareItems(p_item1,p_item2,bNoCase) * (bAsc?1:-1) ;
            if (comp > 0)
            {
                TYPE item2 = GetAt(pos_item2);
                SetAt(pos_item2,*p_item1);
                SetAt(pos_item1,item2);
            }
        }
}

template<class TYPE>
CListEx<TYPE>& CListEx<TYPE>::operator=(CListEx<TYPE>& src)
{
    lock_helper::CAutoLock lock(m_cs);
    lock_helper::CAutoLock lock_src(src.m_cs);

    thisClass* pT = static_cast<thisClass*>(this);

    pT->RemoveAll();
    POSITION pos = src.GetHeadPosition();
    while (pT->is_not_end(pos))
        AddTail(src.GetNext(pos), false);
    
    return *this;
}

template<class TYPE>
CListEx<TYPE>& CListEx<TYPE>::operator+=(CListEx<TYPE>& src)
{
    thisClass* pT = static_cast<thisClass*>(this);

    lock_helper::CAutoLock lock(m_cs);
    lock_helper::CAutoLock lock_src(src.m_cs);

    POSITION pos = src.GetHeadPosition();
    while (pT->is_not_end(pos))
        AddTail(src.GetNext(pos), false);
    
    return *this;
}

template<class TYPE>
CListEx<TYPE>& CListEx<TYPE>::operator=(string& src)
{
    FromString(src);
    return *this;
}

template<class TYPE>
CListEx<TYPE>& CListEx<TYPE>::operator+=(string& src)
{
    FromString(src,false);
    return *this;
}

#ifdef _XML_NODE_DEFINED_

template<class TYPE>
int CListEx<TYPE>::operator+=(XMLNode& xRoot)
{
    thisClass* pT = static_cast<thisClass*>(this);

    int nChildNode = xRoot.nChildNode();
    for (int nodeCnt = 0; nodeCnt < nChildNode; nodeCnt++)
    {
        XMLNode xEntry = xRoot.getChildNode(nodeCnt);
        if (strcmp(xEntry.getName(),"member"))
            continue;

        TYPE item = xEntry;
        AddTail(item);
    }
    m_xmlName = xRoot.getName();
    return pT->GetCount();
}

template<class TYPE>
int CListEx<TYPE>::operator=(XMLNode& xRoot)
{
    if (xRoot.getChildNode("member").isEmpty())
        return 0;

    lock_helper::CAutoLock lock(m_cs);
    
    thisClass* pT = static_cast<thisClass*>(this);

    pT->RemoveAll();

    (*this) += xRoot;

    return pT->GetCount();
}

template<class TYPE>
int CListEx<TYPE>::operator<<(XMLNode& xParent)
{
    int nChildNode = xParent.nChildNode();

    if (!nChildNode)
        return 0;

    lock_helper::CAutoLock lock(m_cs);
    
    thisClass* pT = static_cast<thisClass*>(this);

    pT->RemoveAll();
    
    for (int nodeCnt = 0; nodeCnt < nChildNode; nodeCnt++)
    {
        XMLNode xMember = xParent.getChildNode(nodeCnt);
        if (strcmp(xMember.getName(),"member"))
            continue;

        TYPE item;
        string strTmp = xMember.getAttributeStr("value");
        item = strTmp;
        AddTail(item, false);
    }
    m_xmlName = xParent.getName();

    return pT->GetCount();
}

template<class TYPE>
void CListEx<TYPE>::operator>>(XMLNode& xParent)
{
    if (m_xmlName.size())
        xParent.setName(stringDup(m_xmlName.c_str()));

    lock_helper::CAutoLock lock(m_cs);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = pT->GetHeadPosition();    
    while (is_not_end(pos))
        xParent.addChild(stringDup("member")).addAttribute(stringDup("value"),stringDup(::ToString(GetNext(pos))));
}

template<class TYPE>
void CListEx<TYPE>::ToXML(XMLNode& xParent)
{
    if (m_xmlName.size())
        xParent.setName(stringDup(m_xmlName.c_str()));

    lock_helper::CAutoLock lock(m_cs);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = pT->GetHeadPosition();
    while (is_not_end(pos))
        GetNext(pos).ToXML(xParent.addChild(stringDup("member")));
}

template<class TYPE>
string CListEx<TYPE>::ToXML()
{
    XMLNode xSymDefRoot;
    xSymDefRoot = XMLNode::createXMLTopNode();    

    ToXML(xSymDefRoot);

    string xmlString;
    xSymDefRoot.CreateXMLString(xmlString);
    return xmlString;
}

#endif

template<class TYPE>
bool CListEx<TYPE>::Push(TYPE& value)
{
    AddHead(value);
    return true;
}

template<class TYPE>
bool CListEx<TYPE>::Pop(TYPE& value)
{
    lock_helper::CAutoLock lock(m_cs);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = pT->GetHeadPosition();
    if (pT->is_not_end(pos))
    {
        value = RemoveHead(false);
        return true;
    }
    return false;
}

template<class TYPE>
bool CListEx<TYPE>::Top(TYPE& value)
{
    lock_helper::CAutoLock lock(m_cs);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = pT->GetHeadPosition();
    if (pT->is_not_end(pos))
    {
        pT->GetHead(value);
        return true;
    }
    return false;
}

template<class TYPE>
void CListEx<TYPE>::Trim(char ch)
{
    lock_helper::CAutoLock lock(m_cs);

    thisClass* pT = static_cast<thisClass*>(this);

    POSITION pos = pT->GetHeadPosition();
    while (pos)
        GetNext(pos).Trim(ch);
}

//////////////////////////////////////////////////////////////////////////////////////////////
typedef CListEx<string>                    CStringListEx;

#ifdef _typeStringPair_DEFINED_
class CStringPairList : public CListEx<typeStringPair>
{
public:

    typedef  CListEx<typeStringPair> baseClass;
    CStringPairList(char chSep = '¤') : baseClass(chSep)
    {
    }
    CStringPairList(string& string) : baseClass(string)
    {
    }
    CStringPairList& operator=(string& str)
    {
        baseClass::operator=(str);
        return *this;
    }
    CStringPairList& operator+=(string& str)
    {
        baseClass::operator+=(str);
        return *this;
    }
    CStringPairList& operator=(CStringPairList& src)
    {
        baseClass::operator=(src);
        return *this;
    }

    typeStringPair* FindByItem1(string find)
    {
        lock_helper::CAutoLock lock(m_cs);

        POSITION pos = GetHeadPosition();
        while (is_not_end(pos))
        {
            typeStringPair* pPair = &GetNext(pos);
            if (pPair->m_item1 == find)
                return pPair;
        }
        return NULL;
    }
    
    POSITION FindPosByItem1(string find)
    {
        lock_helper::CAutoLock lock(m_cs);

        POSITION pos = GetHeadPosition();
        while (is_not_end(pos))
        {
            POSITION posLast = pos;
            typeStringPair* pPair = &GetNext(pos);
            if (pPair->m_item1 == find)
                return posLast;
        }
        return end();
    }
    typeStringPair* FindByItem2(string find)
    {
        lock_helper::CAutoLock lock(m_cs);

        POSITION pos = GetHeadPosition();
        while (is_not_end(pos))
        {
            typeStringPair* pPair = &GetNext(pos);
            if (pPair->m_item2 == find)
                return pPair;
        }
        return NULL;
    }

    typeStringPair* FindById(unsigned int uiId)
    {
        lock_helper::CAutoLock lock(m_cs);

        POSITION pos = GetHeadPosition();
        while (is_not_end(pos))
        {
            typeStringPair* pPair = &GetNext(pos);
            if (pPair->m_uiId == uiId)
                return pPair;
        }
        return NULL;
    }

#ifdef _XML_NODE_DEFINED_
    void ToXML(XMLNode& xParent)
    {
        lock_helper::CAutoLock lock(m_cs);

        try
        {
            POSITION pos = GetHeadPosition();
            while (is_not_end(pos))
            {
                typeStringPair* pPair = &GetNext(pos);
                XMLNode xNodeMember = xParent.addChild(stringDup("member"));
                xNodeMember.addAttribute(stringDup("item1"),stringDup(pPair->m_item1.c_str()));
                xNodeMember.addAttribute(stringDup("item2"),stringDup(pPair->m_item2.c_str()));
                string uid;
                uid = string_helper::string_format("%d",pPair->m_uiId);
                xNodeMember.addAttribute(stringDup("uid"),stringDup(uid.c_str()));
            }
        }
        catch(...)
        {
        }
    }

    int operator+=(XMLNode& xRoot)
    {
        try
        {
            int nChildNode = xRoot.nChildNode();
            for (int nodeCnt = 0; nodeCnt < nChildNode; nodeCnt++)
            {
                XMLNode xEntry = xRoot.getChildNode(nodeCnt);
                if (strcmp(xEntry.getName(),"member"))
                    continue;

                typeStringPair item;
                item.m_item1 = xEntry.getAttributeStr("uid");
                if (item.m_item1.size())
                {
                    item.m_uiId = atoi(item.m_item1.c_str());
                    item.m_item1.clear();
                }
                item.m_item1 = xEntry.getAttributeStr("item1");
                item.m_item2 = xEntry.getAttributeStr("item2");
                AddTail(item);
            }
        }
        catch(...)
        {
        }
        return GetCount();
    }
    
    int operator=(XMLNode& xRoot)
    {
        try
        {
            RemoveAll();
            (*this) += xRoot;
        }
        catch(...)
        {
        }
        return GetCount();
    }
#endif

};

#endif

#endif
