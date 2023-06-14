#ifndef __STRING_HELPER_H__
#define __STRING_HELPER_H__

#include <string.h>
#include <stdio.h>
#include <string>
#include <stdarg.h> 
#include <vector>
#include <algorithm>

#ifdef _WIN32
    #define PATH_SEP_STR    "\\"
    #define PATH_SEP_CHAR   '\\'
#else
    #define PATH_SEP_STR    "/"
    #define PATH_SEP_CHAR   '/'
#endif

namespace string_helper
{

using namespace std;

inline std::string string_trim(std::string& str, const char ch = ' ') 
{ 
    str.erase(0, str.find_first_not_of(ch));       //prefixing spaces 
    str.erase(str.find_last_not_of(ch) + 1);         //surfixing spaces 
    return str; 
} 

inline std::string string_replace(std::string& str, const char ch, std::string with) 
{ 
    int idx = 0;
    while((idx = str.find_first_of(ch, idx)) >= 0 ) 
    {
        str.replace(idx,1,with);
        idx += with.size();
    }
    return str; 
} 

inline std::string string_replace(std::string& str, std::string oldstr, std::string with) 
{ 
    int idx = 0;
    while((idx = str.find(oldstr, idx)) >= 0 ) 
    {
        str.erase(idx,oldstr.size());
        str.insert(idx,with);
        idx += with.size();
    }
    return str; 
} 

inline std::string string_remove(std::string& str, const char ch) 
{ 
    int idx = 0;
    while((idx = str.find_first_of(ch, idx)) >= 0 ) 
        str.replace(idx,1,"");
    return str; 
} 

inline int string_parse(std::vector <std::string>& vector, std::string str, const char ch_sep = ' ') 
{ 

    std::string  temp;
    size_t ch_sep_pos = std::string::npos;

    while ((ch_sep_pos = str.find(ch_sep, 0)) != std::string::npos)
    { 
        temp = str.substr(0, ch_sep_pos);      //get the token
        str.erase(0, ch_sep_pos + 1);          //erase it from the source 
        vector.push_back(temp);                //and put it into the array
    }

    if (str.size() > 0)
        vector.push_back(str);         

    return vector.size();
}

inline std::string string_tolower(std::string& str) 
{ 
    std::transform(str.begin(), str.end(), str.begin(), ::tolower); 
    return str; 
} 

inline bool string_substring(std::string& str, const char* pszFullString, int iSubString, char chSep)
{
    str.erase();

	if (pszFullString == NULL)
		return false;

	while (iSubString--)
	{
		pszFullString = strchr(pszFullString, chSep);
		if (pszFullString == NULL)
			return false;

        pszFullString++;       // point past the separator
	}
	char* pchEnd = strchr((char*)pszFullString, chSep);
	int nLen = (pchEnd == NULL) ? strlen(pszFullString) : (int)(pchEnd - pszFullString);
    if (nLen <= 0)
        return false;

    str.append(pszFullString,nLen);    
	return true;
}

inline bool string_substring(std::string& str, const std::string& strFullString, int iSubString, char chSep)
{
    return string_substring(str, strFullString.c_str(), iSubString, chSep);
}

inline bool string_substring(std::string& str, std::string& strFullString, int iSubString, char chSep)
{
    return string_substring(str, strFullString.c_str(), iSubString, chSep);
}

inline std::string string_format(const char* fmt, ...)
{
    char buffer[1024] = "";
    int size = sizeof(buffer) - 1;
    int nsize = 0;
    char* p_buffer = &buffer[0];
    char* p_buffer_new = NULL;
    char* p_buffer_use = p_buffer;
    va_list vl;
    va_start(vl, fmt);

string_format_try_chr_again:
    nsize = vsnprintf(p_buffer_use, size, fmt, vl);
    if (nsize == -1)
    {
        if (p_buffer_new) delete []p_buffer_new;
        size = 2*size;
        p_buffer_use = p_buffer_new = new char[size];
        goto string_format_try_chr_again;
    }
    else
    if (size<=nsize)
    { //fail delete buffer and try again
        if (p_buffer_new) delete []p_buffer_new;
        p_buffer_use = p_buffer_new = new char[nsize+1];
        goto string_format_try_chr_again;
    }
    va_end(vl);

    std::string ret;
    ret = p_buffer_use;
    
    if (p_buffer_new)
        delete[] p_buffer_new;
    return ret;
}

inline std::string string_append(std::string& str, char ch, int to_length) 
{ 
    if ((int)str.size() < to_length)
        str += string(to_length - str.size(),ch);
        
    return str;
}

inline std::string string_append_spaces(std::string& str, int to_length) 
{ 
    if ((int)str.size() < to_length)
        str += string(to_length - str.size(),' ');
        
    return str;
}

inline std::string string_between(const std::string &s, char ch_start, char ch_stop, bool b_incl = false)
{
    int pos_start = s.find(ch_start,0);
    if (pos_start == string::npos)
        return "";
    int pos_end = s.find(ch_stop,pos_start + 1);
    if (pos_end == string::npos)
        return "";
    if (!b_incl) pos_start++;
    if (b_incl) pos_end++;
    return s.substr(pos_start, pos_end - pos_start);
}

inline std::string string_outside(const std::string &s, char ch_start, char ch_stop, bool b_incl = false)
{
    int pos_start = s.find(ch_start,0);
    if (pos_start == string::npos)
        return "";
    int pos_end = s.find(ch_stop,pos_start + 1);
    if (pos_end == string::npos)
        return "";
    if (b_incl) pos_start++;
    if (!b_incl) pos_end++;
    return s.substr(0, pos_start) + s.substr(pos_end);
}

/**
*  Get Directory from path
*
*/
inline std::string string_get_dir(const char* p_path, char ch_path = PATH_SEP_CHAR)
{
    if (!p_path)
        return "";
    if (!*p_path)
        return "";
        
    char* p_file = NULL;
    char* p_ch = (char*)p_path;
    char ch = 0;
    while((ch = *p_ch))
    {
        if (ch == ch_path)
            p_file = p_ch;
        p_ch++;
    }
    if (!p_file)
        return "";
    if (*p_file == ch_path)
        p_file++;
    
    if (p_path < p_file)
        return std::string(p_path, p_file - p_path - 1);
        
    return "";
}

/**
*  Get file name from path
*
*/
inline std::string string_get_filename(const char* p_path, char ch_path = PATH_SEP_CHAR)
{
    if (!p_path)
        return "";
    if (!*p_path)
        return "";
        
    char* p_file = NULL;
    char* p_ch = (char*)p_path;
    char ch = 0;
    while((ch = *p_ch))
    {
        if (ch == ch_path)
            p_file = p_ch;
        p_ch++;
    }
    if (!p_file)
        p_file = (char*)p_path;
    
    if (p_path < p_file)
        return std::string(p_file + 1);
    else
    if (p_path == p_file)
        return std::string(p_file);
        
    return "";
}


};

#endif /*__STRING_UTIL_H__*/
