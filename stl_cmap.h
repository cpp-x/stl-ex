#ifndef _STL_CMAP
#define _STL_CMAP

#include <map>

using namespace std;

template<class KEY, class VALUE>
class CMapSTL
{
public:
    typedef CMapSTL<KEY,VALUE> thisClass;

    typedef std::map<KEY,VALUE>                    STLMap_t;
    typedef typename std::map<KEY,VALUE>::iterator POSITION;
    typedef typename std::map<KEY,VALUE>::reverse_iterator RPOSITION;
    
    CMapSTL()
    {
    }

    ~CMapSTL()
    {
        m_map.clear();
    }

    int GetCount()
    {
        return m_map.size();
    }

    int GetSize()
    {
        return m_map.size();
    }

    bool IsEmpty()
    {
        return (m_map.size() == 0);
    }

    POSITION GetStartPosition()
    {
        if (m_map.size())
            return m_map.begin();
        
        return m_map.end();
    }

	void GetNextAssoc(POSITION& pos, KEY& key, VALUE& value)
    {
        if (pos == m_map.end())
            return;
        key = (*pos).first;
        value = (*pos).second;
        pos++;
    }

    void SetAt(KEY& key, VALUE& newValue)
    {
        m_map[key] = newValue;
    }

    bool Lookup(KEY& key, VALUE& value)
    {
        POSITION pos = m_map.find(key);
        if (pos == m_map.end())
            return false;

        value = (*pos).second;
        return true;
    }

	VALUE& operator[](KEY key)
    {
        POSITION pos = m_map.find(key);
        if (pos == m_map.end())
            return m_end;

         return (*pos).second;
    }

    bool RemoveKey(KEY key)
    {
        POSITION pos = m_map.find(key);
        if (pos == m_map.end())
            return false;
        
        m_map.erase(pos);
        return true;
    }

    void RemoveAll()
    {
        m_map.clear();
    }

    POSITION begin()
    {
        return m_map.begin();
    }

    POSITION end()
    {
        return m_map.end();
    }

    bool is_end(POSITION& pos)
    {
        return (pos == m_map.end());
    }

    bool is_not_end(POSITION& pos)
    {
        return (pos != m_map.end());
    }

    bool is_end(VALUE& value)
    {
        return ((&value) == (&m_end));
    }

    bool is_not_end(VALUE& value)
    {
        return ((&value) != (&m_end));
    }

private:
    VALUE     m_end;

public:
    STLMap_t  m_map;

};

typedef CMapSTL<string,string>    CStringMap;

#endif