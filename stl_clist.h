#ifndef _STL_CLIST
#define _STL_CLIST

#include <list>

using namespace std;

#ifndef LPCTSTR
    #define LPCTSTR const char*
#endif

#ifndef LPCSTR
    #define LPCSTR const char*
#endif

#ifndef LPSTR
    #define LPSTR char*
#endif

template<typename TYPE>
class CListSTL
{
public:
    typedef CListSTL<TYPE> thisClass;

    typedef std::list<TYPE>     STLList_t;
    typedef typename std::list<TYPE>::iterator         POSITION;
    typedef typename std::list<TYPE>::reverse_iterator RPOSITION;
    
    CListSTL()
    {
    }

    ~CListSTL()
    {
        m_list.clear();
    }

    int GetCount()
    {
        return m_list.size();
    }

    int GetSize()
    {
        return m_list.size();
    }

    bool IsEmpty()
    {
        return (m_list.size() == 0);
    }

    bool GetHead(TYPE& value)
    {
        if (!m_list.size())
            return false;

        value = *m_list.begin();
        
        return true;
    }

    bool GetHead(TYPE*& p_value)
    {
        p_value = NULL;

        if (!m_list.size())
            return false;

        p_value = &m_list.begin();
        
        return true;
    }

    bool GetTail(TYPE& value)
    {
        if (!m_list.size())
            return false;

        value = m_list.back();
        
        return true;
    }

    bool GetTail(TYPE*& p_value)
    {
        p_value = NULL;

        if (!m_list.size())
            return false;

        p_value = &m_list.back();
        
        return true;
    }

    POSITION GetHeadPosition()
    {
        if (m_list.size())
            return m_list.begin();
        
        return m_list.end();
    }

    RPOSITION GetTailPosition()
    {
        if (!m_list.size())
            return m_list.rend();
        
        RPOSITION rpos = m_list.rbegin();
        return rpos;
    }

    TYPE& GetNext(POSITION& pos)
    {
        /*
        if (m_list.size() <= 1)
            return *m_list.end();
        */
        return *pos++;
    }

    TYPE& GetPrev(RPOSITION& pos)
    {
        /*
        if (m_list.size() <= 1)
            return *m_list.end();
        */
        return *pos++;
    }

    TYPE& GetAt(POSITION& pos)
    {
        if (!m_list.size())
            return *m_list.end();
        
        return *(pos);
    }

    TYPE& GetAt(RPOSITION& rpos)
    {
        if (!m_list.size())
            return *m_list.end();
        
        return *(rpos);
    }

    POSITION AddTail(TYPE& newElement)
    {
        m_list.push_back(newElement);
        POSITION pos = m_list.end();
        return --pos;
    }

    POSITION AddHead(TYPE& newElement)
    {
        m_list.push_front(newElement);
        return m_list.begin();
    }

    POSITION InsertBefore(POSITION& pos, TYPE& newElement)
    {
        if (pos != m_list.end())
            return m_list.insert(pos, newElement);
        
        return AddTail(newElement);
    }

    POSITION InsertAfter(POSITION& pos, TYPE& newElement)
    {
        if (++pos != m_list.end())
            return m_list.insert(pos, newElement);
        
        return AddTail(newElement);
    }

    void SetAt(POSITION& pos, TYPE& newElement)
    {
        *pos = newElement;
    }

    void RemoveAt(POSITION& pos)
    {
        m_list.erase(pos);
    }

    bool RemoveHead(TYPE& value)
    {
        if (!m_list.size())
            return false;
       
        GetHead(value);

        m_list.pop_front();
        
        return true;
    }

    bool RemoveTail(TYPE& value)
    {
        if (!m_list.size())
            return false;

        GetTail(value);

        m_list.pop_back();
        
        return true;
    }

    void RemoveAll()
    {
        m_list.clear();
    }

    POSITION Find(TYPE& searchValue)
    {
        for (POSITION pos = m_list.begin(); pos != m_list.end(); pos++)
            if (*pos == searchValue)
                return pos;

        return m_list.end();
    }

    bool Exists(TYPE& searchValue)
    {
        for (POSITION pos = m_list.begin(); pos != m_list.end(); pos++)
            if (*pos == searchValue)
                return true;

        return false;
    }

    POSITION Find(TYPE& searchValue, POSITION& startAfter)
    {
        for (POSITION pos = startAfter; pos != m_list.end(); pos++)
            if (*pos == searchValue)
                return pos;

        return m_list.end();
    }

    POSITION FindIndex(int index)
    {
        if (index >= m_list.size())
            return m_list.end();

        return std::advance(m_list.begin(), index);
    }

    POSITION end()
    {
        return m_list.end();
    }

    bool is_end(POSITION& pos)
    {
        return (pos == m_list.end());
    }

    bool is_not_end(POSITION& pos)
    {
        return (pos != m_list.end());
    }

    bool is_rend(RPOSITION& rpos)
    {
        return (rpos == m_list.rend());
    }
    bool is_not_rend(RPOSITION& rpos)
    {
        return (rpos != m_list.rend());
    }

public:
    STLList_t  m_list;

};

typedef CListSTL<string>    CStringList;

#endif
