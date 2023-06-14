#pragma once

#include "obj_collection.h"
#include "xml_parser.h"
#include "stl_carray.h"
#include "list_ex.h"

inline bool IsNull(string& str) { return !str.size();}
inline bool IsNull(int val) { return val == 0; }
inline bool IsNull(void* ptr) { return ptr == NULL; }

template <class K,class T, bool B = true>
class CItemStack
{
public:
    
    typedef CObjCollection<K,T*> typeItemStack;

    CItemStack(string keyword = "");
    virtual ~CItemStack(void);

    T* SetAt(K& key, T* pItemStack, unsigned int uiId = 0);
    T* Replace(K& key, T* pItemStack);
    bool InsertAt(int idx, K& key, T* pItemStack);
    T* Remove(K& key);
    T* RemoveAt(int idx);

    T*    operator[](int idx);
    T*    operator[](K& name);
    T*  operator[](unsigned int uiId);
    int operator=(XMLNode& xRoot);
    int operator+=(XMLNode& xRoot);

    CItemStack<K,T,B>& operator=(CItemStack<K,T,B>& src);
    CItemStack<K,T,B>& operator<=(CItemStack<K,T,B>& src);

    bool GetAt(int idx, K& key);
    bool GetAt(int idx, T& value);
    bool GetAt(int idx, K& key, T*& value);

    int    GetCount();
    unsigned int GetUIID(int idx);

    T*    SetDefault(K& defItem);
    T*    GetDefault();

    int    Find(K& key);
    K*    FindKey(T* val);
    bool Swap(int idx1, int idx2);
    
    T* FindById(int id);
    T* FindByName(K& name);

    bool IsDefault(K test);

    void        RemoveAll() { Cleanup();};

    string        m_keyword;

    K             m_defItem;

    CArraySTL<typeItemStack>    m_arStack;

    CStringListEx*    m_pConfigSpec;
    CStringListEx*    m_pListLabels;

    bool    m_bAutoDelete;

protected:
    virtual void    Init();
    virtual void    Cleanup();

    bool    IsSet(K& item) {return !IsNull(item); }

    virtual bool IsSeparator(K& key) {return false;};

};

template <class K,class T, bool B>
CItemStack<K,T,B>::CItemStack(string keyword)
{
    m_bAutoDelete = B;
    m_keyword = keyword;
    m_pConfigSpec = NULL;
    m_pListLabels = NULL;
    Init();
}

template <class K,class T, bool B>
CItemStack<K,T,B>::~CItemStack(void)
{
    Cleanup();
}


template <class K,class T, bool B>
void CItemStack<K,T,B>::Init()
{
}

template <class K,class T, bool B>
void CItemStack<K,T,B>::Cleanup()
{
    try
    {
        if (m_bAutoDelete)
            for (int cnt=0; cnt < m_arStack.GetCount(); cnt++)
                delete m_arStack[cnt].m_item2;
    }
    catch(...)
    {
    }
    m_arStack.RemoveAll();

}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::SetAt(K& key, T* pItemStack, unsigned int uiId)
{
    T* pItemStackOld = NULL;
    int idxFind = Find(key);
    if (idxFind >= 0)
    {
        typeItemStack* pItem = &m_arStack[idxFind];    
        pItemStackOld = pItem->m_item2;
        if (m_bAutoDelete && pItemStackOld)
        {
            try{
            delete pItemStackOld;
            pItemStackOld = NULL;
            }catch(...){}
        }
        
        pItem->m_item2 = pItemStack;
    }
    else
    {
        typeItemStack item(key, pItemStack, uiId);
        m_arStack.Add(item);
    }
    
    if (!IsSet(m_defItem))
        m_defItem = key;
    
    return pItemStackOld;
}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::Replace(K& key, T* pItemStack)
{
    T* pItemStackOld = NULL;
    int idxFind = Find(key);

    if (idxFind >= 0)
    {
        typeItemStack* pItem = &m_arStack[idxFind];    
        pItemStackOld = pItem->m_item2;
        pItem->m_item2 = pItemStack;
        if (m_bAutoDelete && pItemStackOld)
        {
            try{
            delete pItemStackOld;
            pItemStackOld = NULL;
            }catch(...){}
        }
    }
    return pItemStackOld;
}

template <class K,class T, bool B>
bool CItemStack<K,T,B>::InsertAt(int idx, K& key, T* pItemStack)
{
    typeItemStack item(key, pItemStack);
    if (!m_arStack.InsertAt(idx,item))
    {
        if (m_bAutoDelete && pItemStack)
        {
            try{
            delete pItemStack;
            }catch(...){}
        }
        return false;
    }
    
    if (!IsSet(m_defItem))
        m_defItem = key;
    
    return true;
}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::Remove(K& key)
{
    K keySave = key;
    int idx = Find(key);
    if (idx == -1)
        return false;
    T* pItem = (*this)[idx];
    if (m_bAutoDelete && pItem)
    {
        try{
        delete pItem;
        }catch(...){}
    }
    m_arStack.RemoveAt(idx);
    if (m_defItem == keySave)
        m_defItem = (char)NULL;
    
    return m_bAutoDelete?NULL:pItem;
}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::RemoveAt(int idx)
{
    typeItemStack* pItem = &(m_arStack[idx]);
    if (m_arStack.is_end(*pItem))
        return NULL;
    K keySave = pItem->m_item1;
    if (m_bAutoDelete && pItem->m_item2)
    {
        try{
        delete pItem->m_item2;
        }catch(...){}
    }
    m_arStack.RemoveAt(idx);
    if (m_defItem == keySave)
        m_defItem = (char)NULL;
    
    return m_bAutoDelete?NULL:pItem->m_item2;
}

template <class K,class T, bool B>
int    CItemStack<K,T,B>::GetCount()
{
    return m_arStack.GetCount();
}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::operator[](int idx)
{
    if (idx >= m_arStack.GetCount())
        return NULL;
    
    return m_arStack[idx].m_item2;
}

template <class K,class T, bool B>
unsigned int CItemStack<K,T,B>::GetUIID(int idx)
{
    if (idx >= m_arStack.GetCount())
        return NULL;
    
    return m_arStack[idx].m_uiId;
}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::operator[](K& key)
{
    for (int cnt=0; cnt < m_arStack.GetCount(); cnt++)
    {
        typeItemStack* pItem = &(m_arStack[cnt]);    
        if (pItem->m_item1 == key)
            return pItem->m_item2;
    }
    return NULL;
}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::operator[](unsigned int uiId)
{
    for (int cnt=0; cnt < m_arStack.GetCount(); cnt++)
    {
        typeItemStack* pItem = &(m_arStack[cnt]);    
        if (pItem->m_uiId == uiId)
            return pItem->m_item2;
    }
    return NULL;
}

template <class K,class T, bool B>
int CItemStack<K,T,B>::Find(K& key)
{
    for (int cnt=0; cnt < m_arStack.GetCount(); cnt++)
        if (m_arStack[cnt].m_item1 == key)
            return cnt;
    return -1;
}

template <class K,class T, bool B>
K*    CItemStack<K,T,B>::FindKey(T* val)
{
    for (int cnt=0; cnt < m_arStack.GetCount(); cnt++)
        if (m_arStack[cnt].m_item2 == val)
            return &m_arStack[cnt].m_item1;
    return NULL;
}

template <class K,class T, bool B>
bool CItemStack<K,T,B>::Swap(int idx1, int idx2)
{
    if (m_arStack.GetCount() <= 1)
        return false;

    if (idx1 >= m_arStack.GetCount())
        return false;
    
    if (idx2 >= m_arStack.GetCount())
        return false;

    typeItemStack item1 = m_arStack[idx1];
    typeItemStack item2 = m_arStack[idx2];

    m_arStack[idx1] = item2;
    m_arStack[idx2] = item1;

    return true;
}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::GetDefault()
{
    T* pDefItem = (*this)[m_defItem];
    if (!pDefItem)
        if (GetCount())
            pDefItem =  (*this)[0];

    return pDefItem;
}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::SetDefault(K& defItem)
{
    T* pDefItem = (*this)[defItem];
    if (pDefItem)
        m_defItem = defItem;
    return pDefItem;
}

template <class K,class T, bool B>
bool CItemStack<K,T,B>::IsDefault(K item)
{
    return (m_defItem == item);
}

template <class K,class T, bool B>
int CItemStack<K,T,B>::operator+=(XMLNode& xRoot)
{
    if (!m_keyword.size())
        return 0;

    int nChildNode = xRoot.nChildNode();

    for (int nodeCnt = 0; nodeCnt < nChildNode; nodeCnt++)
    {
        XMLNode xEntry = xRoot.getChildNode(nodeCnt);
        if (strcmp(xEntry.getName(),m_keyword.c_str()))
            continue;

        if (m_pConfigSpec || m_pListLabels)
        {
            string label = xEntry.getAttributeStr("label");
            if (label.size())
            {
                bool b_no_label = false;
                if ((b_no_label = (label.at(0)=='-')))
                    label = label.substr(1);

                if (m_pListLabels)
                    m_pListLabels->AddTailUnique(label);

                if (!m_pConfigSpec)
                    continue;

                if (b_no_label)
                {
                    if (m_pConfigSpec->Exists(label))
                        continue;
                }
                else
                if (!m_pConfigSpec->Exists(label))
                    continue;
            }
        }

        T* pItem = new T;
        *pItem = xEntry;

        if (!pItem->m_name.size())
            continue;

        SetAt(pItem->m_name,pItem);
    }
    return GetCount();
}

template <class K,class T, bool B>
int CItemStack<K,T,B>::operator=(XMLNode& xRoot)
{
    if (!m_keyword.size())
        return 0;

    if (xRoot.getChildNode(m_keyword.c_str()).isEmpty())
        return 0;

    RemoveAll();

    (*this) += xRoot;

    return GetCount();
}

template <class K,class T, bool B>
CItemStack<K,T,B>& CItemStack<K,T,B>::operator=(CItemStack<K,T,B>& src)
{
    RemoveAll();
    m_keyword = src.m_keyword;
    m_defItem = src.m_defItem;

    for (int cnt=0; cnt < src.m_arStack.GetCount(); cnt++)
        m_arStack.Add(src.m_arStack[cnt]);
    return *this;
}

template <class K,class T, bool B>
CItemStack<K,T,B>& CItemStack<K,T,B>::operator<=(CItemStack<K,T,B>& src)
{
    RemoveAll();
    m_keyword = src.m_keyword;
    m_defItem = src.m_defItem;

    for (int cnt=0; cnt < src.m_arStack.GetCount(); cnt++)
        SetAt(src.m_arStack[cnt].m_item1,new T(*src.m_arStack[cnt].m_item2));
    return *this;
}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::FindById(int id)
{
    for (int cnt=0; cnt < m_arStack.GetCount(); cnt++)
    {
        T* pItem = (*this)[cnt];
        if (!pItem)
            continue;
        if (pItem->m_id == id)
            return pItem;
    }
    return NULL;
}

template <class K,class T, bool B>
T* CItemStack<K,T,B>::FindByName(K& name)
{
    for (int cnt=0; cnt < m_arStack.GetCount(); cnt++)
    {
        T* pItem = (*this)[cnt];
        if (!pItem)
            continue;

        if (_AfxComparePath(pItem->m_name,name))
            return pItem;
    }
    return NULL;
}

template <class K,class T, bool B>
bool CItemStack<K,T,B>::GetAt(int idx, K& key, T*& value)
{
    if (idx >= m_arStack.GetCount())
        return false;
    
    typeItemStack* p_item = &m_arStack[idx];
    key = p_item->m_item1;
    value = p_item->m_item2;

    return true;
}

//////////////////////////////////////////////////////////////////////
template <class T, bool B = true>
class CStringStack: public CItemStack<string,T,B> 
{
public:
    CStringStack(string keyword = "") : CItemStack<string,T,B>(keyword)
    {
    }
    int operator=(XMLNode& xRoot)
    {
        return CItemStack<string,T,B>::operator=(xRoot);
    }
    CItemStack<string,T,B>& operator=(CItemStack<string,T,B>& src)
    {
        CItemStack<string,T,B>::operator =(*((CItemStack<string,T,B>*)&src));
        return *this;
    }

    virtual bool IsSeparator(string& key) {return (key.find("separator") != string::npos);};
};

