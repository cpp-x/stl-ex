#ifndef _STL_CARRAY
#define _STL_CARRAY

#include <vector>

using namespace std;

template<class TYPE>
class CArraySTL
{
public:
    typedef CArraySTL<TYPE> thisClass;

    typedef std::vector<TYPE>                       STLArray_t;
    typedef typename std::vector<TYPE>::iterator    POSITION;
    typedef typename std::vector<TYPE>::reverse_iterator RPOSITION;
    
    CArraySTL()
    {
    }

    ~CArraySTL()
    {
        m_array.clear();
    }

    int GetCount()
    {
        return m_array.size();
    }

    int GetSize()
    {
        return m_array.size();
    }

    bool IsEmpty()
    {
        return (m_array.size() == 0);
    }

    void RemoveAll()
    {
        m_array.clear();
    }

    POSITION Add(TYPE& newElement)
    {
        m_array.push_back(newElement);
        POSITION pos = m_array.end();
        return --pos;
    }

    bool InsertAt(unsigned int index, TYPE& newElement)
    {
        POSITION pos = FindIndex(index);
        if (pos == m_array.end())
            return false;
        
        m_array.insert(pos, newElement);

        return true;
    }

    bool RemoveAt(unsigned int index)
    {
        POSITION pos = FindIndex(index);
        if (pos == m_array.end())
            return false;
        
        m_array.erase(pos);

        return true;
    }

    TYPE& operator[](unsigned int index)
    {
        if (index >= m_array.size())
            return m_end;

        return m_array[index];
    }

    POSITION FindIndex(unsigned int index)
    {
        if (index >= m_array.size())
            return m_array.end();

        POSITION pos = m_array.begin();
        std::advance(pos, index);
        return pos;
    }

    bool is_end(TYPE& element)
    {
        return ((&element) == (&m_end));
    }

    bool is_not_end(TYPE& element)
    {
        return ((&element) != (&m_end));
    }

    POSITION end()
    {
        return m_array.end();
    }

    POSITION begin()
    {
        return m_array.begin();
    }

private:
    TYPE m_end;

public:

    STLArray_t  m_array;
};

typedef CArraySTL<string>    CStringArray;

#endif
