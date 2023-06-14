#include <stdafx.h>
#include "text_helper.h"
#include "string_helper.h"
#include <time.h>

char* strnpattern(char* pInput, unsigned int szInput,const char* pPattern ,unsigned int szPattern, char chReplace)
{
    if (!(pInput && szInput))
        return NULL;

    if (!(pPattern && szPattern))
        return NULL;

    if (szInput < szPattern)
        return NULL;
    
    unsigned int dashPos = (unsigned int)-1; //If this is different, it means that we have to clear some chars
    
    for (unsigned int idxPat = 0; idxPat < szPattern; idxPat++)
    {
        if (szInput < szPattern)
            return NULL;

        char chPat = *(pPattern + idxPat);
        switch(chPat)
        {
            case '*': //any char is allowed
                      break;   
            case '$': //must be a number
                      if (!isdigit(*(pInput + idxPat)))
                          goto strnpattern_no_match;
                      break;   
            case '&': //must be a hexnumber
                      if (!isxdigit(*(pInput + idxPat)))
                          goto strnpattern_no_match;
                      break;   
            case '%': //must be a alpha character
                      if (!isalpha(*(pInput + idxPat)))
                          goto strnpattern_no_match;
                      break;   
            case '?': //must be a printable character
                      if (!isprint(*(pInput + idxPat)))
                          goto strnpattern_no_match;
                      break;   
            case '#': //any char is allowed, but replace on match with empty char
                      if (dashPos == (unsigned int)-1)
                         dashPos = idxPat;
                      break;
            default:  //failed on any differences
                      if ((*(pInput + idxPat)) != chPat)
            strnpattern_no_match:
                      {
                          szInput--;
                          pInput++;
                          idxPat = 0;                
                      }
                      break;
        }
    }
    //If we ended up here, it means that whe have a pattern match
    if (chReplace && (dashPos != (unsigned int)-1))
    {   //We have still some cleaning work to do
        for (unsigned int idxPat = dashPos; idxPat < szPattern; idxPat++)
            if ((*(pPattern + idxPat)) == '#')
                *(pInput + idxPat) = chReplace; 
    }
    
    return pInput;
}

char* strnstr(char * string, const char * strSearch, int len)
{
    char s1 = *strSearch++;
    
    while (len-- > 0)
        if (*string++ == s1) 
        {
            const char * s = strSearch;
            const char * m = string;
            
            while (*s && *s == *m)
                ++s, ++m;
            
            if (!*s)
                return string-1;
        }
    return NULL;
}

char* stristr(char *String, const char *Pattern)
{
    char *pptr, *sptr, *start;

    for (start = String; *start != 0; start++)
    {
        /* find start of pattern in string */
        for ( ; ((*start!=0) && (toupper(*start) != toupper(*Pattern))); start++)
              ;
        if (0 == *start)
              return NULL;

        pptr = (char *)Pattern;
        sptr = (char *)start;

        while (toupper(*sptr) == toupper(*pptr))
        {
            sptr++;
            pptr++;

            /* if end of pattern then pattern was found */

            if (0 == *pptr)
               return (start);
        }
    }
    return NULL;
}

char* strnistr(char *String, const char *Pattern, unsigned int len)
{
    char *pptr, *sptr, *start;
    char *end = String + str_min(len,strlen(String)); 

    for (start = String; (*start != 0) && (start < end); start++)
    {
        char P_char = toupper(*Pattern);
        char S_Char = 0;
        /* find start of pattern in string */
        for ( ; (((*start != 0) && (start < end)) && ((S_Char = toupper(*start)) != P_char)); start++)
              ;
        if (0 == *start)
           return NULL;

        pptr = (char *)Pattern;
        sptr = (char *)start;

        while (toupper(*sptr) == toupper(*pptr))
        {
            sptr++;
            pptr++;

            /* if end of pattern then pattern was found */

            if (0 == *pptr)
             return (start);
        }
    }
      return NULL;
}

int strtrimeol(char* p_str, int len)
{
    if (!len)
        return len;

    int len_trim = len;
    char* p_str_end = p_str + len - 1;
    char ch;
    while(len-- && (ch=*p_str_end))
    {
        if ((ch == '\n') || (ch == '\r'))
        {
            p_str_end--,len_trim--;
            continue;
        }
        break;
    }
    return len_trim;
}

int strnchr(char* string, char ch)
{
    char* p_str = string;
    int cnt = 0;
    char _ch;
    while((_ch = *p_str))
    {
        p_str++;
        if (_ch == ch)
            cnt++;
    }
    return cnt;
}

int stricmp(char* p_str_1, char* p_str_2)
{
    if (!(p_str_1 || p_str_2)) return 0;
    if (!p_str_1) return -1;
    if (!p_str_2) return 1;
    char ch_1, ch_2;
    while( (ch_1=tolower(*p_str_1)) && (ch_2=tolower(*p_str_2)) )
    {
        if (ch_1 < ch_2) return -1;
        if (ch_1 > ch_2) return 1;
        p_str_1++;
        p_str_2++;
    }
    return 0;
}

//2022 Aug 13 00:19:21.752 agib kernel: Booting Linux on physical CPU 0x0

int validate_syslog_pdu(char* p_pdu, unsigned int sz_pdu)
{

    char* p_ts = NULL;
    
    if (sz_pdu >= (20+4))                     //2022 Aug 13 00:19:21
        if ((p_ts = strnpattern(p_pdu, sz_pdu, "**** *** ** **:**:**", 20, 0)) == p_pdu)
            return 0;

    if (sz_pdu >= (15+4))                     //Aug 13 00:19:21
        if ((p_ts = strnpattern(p_pdu, sz_pdu, "*** ** **:**:**", 15, 0)) == p_pdu)
            return 2;

    return -1;
}

ts_decode_t _ts_decode[] = 
{
    //2022 Aug 13 00:19:21.123
    {"$$$$ %%% $$ $$:$$:$$.$$$" ,24, 7/*,"%4s %3s %2s %2s:%2s:%2s.%3s"*/},
    //2022 Aug  3 00:19:21.123
    {"$$$$ %%%  $ $$:$$:$$.$$$" ,24, 7/*,"%4s %3s  %1s %2s:%2s:%2s.%3s"*/},
    //2022 Aug 13 00:19:21
    {"$$$$ %%% $$ $$:$$:$$"     ,20, 6/*,"%4s %3s %2s %2s:%2s:%2s"*/},
    //2022 Aug  3 00:19:21
    {"$$$$ %%%  $ $$:$$:$$"     ,20, 6/*,"%4s %3s  %1s %2s:%2s:%2s"*/},
    //Aug 13 00:19:21.123
    {"%%% $$ $$:$$:$$.$$$"      ,19, 5/*,"%3s %2s %2s:%2s:%2s.%3s"*/},
    //Aug 13 00:19:21.123
    {"%%%  $ $$:$$:$$.$$$"      ,19, 5/*,"%3s  %1s %2s:%2s:%2s.%3s"*/},
    //Aug 13 00:19:21
    {"%%% $$ $$:$$:$$"          ,15, 4/*,"%3s %2s %2s:%2s:%2s.%3s"*/},
    //Aug  3 00:19:21
    {"%%%  $ $$:$$:$$"          ,15, 4/*,"%3s  %1s %2s:%2s:%2s.%3s"*/},
    {"", 0, 0/*, ""*/},
};

ts_decode_t* _p_ts_decode = &_ts_decode[0];

char* next_token_ts(char* p_str, char* p_token)
{
    bool b_do = true;
    while(b_do)
    {
        char ch = *p_str++;
        if (!ch) break;
        switch(ch)
        {
            case ' ':
            case ':':
            case '/':
            case '.':
                 b_do = false;
                 break;

            default:
                  *p_token = ch;
                   p_token++;
                   break;
        }
    }
    return p_str;
}

bool stroneof(char ch, char* p_chars)
{
    char chs;
    while((chs = *p_chars))
    {
        p_chars++;
        if (ch == chs)
            return true;
    }
    return false;
}

char* strfirstof(char* p_str, char* p_chars)
{
    if (!p_str) return NULL;
    if (!*p_str) return NULL;

    char ch;
    while((ch = *p_str))
    {
        p_str++;
        char chs;
        char* _p_chars = p_chars;
        while((chs = *_p_chars))
        {
            _p_chars++;
            if (ch == chs)
                return --p_str;
        }
    }
    
    return NULL;
}

bool is_num_string(const char* p_str)
{
    if (!p_str) return false;
    if (!*p_str) return false;
    
    char ch;
    while((ch = *p_str))
    {
        p_str++;
        if (!stroneof(ch, (char*)"01234567890abcdefABCDEFxX+-"))
            return false;
    }
    return true;
}

bool is_byte_string(const char* p_str)
{
    if (!p_str) return false;
    if (!*p_str) return false;
    
    char ch;
    while((ch = *p_str))
    {
        p_str++;
        if (!stroneof(ch, (char*)"01234567890.kMGTbB"))
            return false;
    }
    return true;
}

bool is_decimal(const char* p_str)
{
    if (!p_str) return false;
    if (!*p_str) return false;
    
    char ch;
    while((ch = *p_str))
    {
        p_str++;
        if (!stroneof(ch, (char*)"01234567890+-"))
            return false;
    }
    return true;
}

bool is_hex(const char* p_str)
{
    if (!p_str) return false;
    if (!*p_str) return false;
    
    char ch;
    while((ch = *p_str))
    {
        p_str++;
        if (!stroneof(ch, (char*)"abcdefABCDEFxX"))
            return false;
    }
    return true;
}

int str_to_int(const char* p_str)
{
    if (!p_str) return 0;
    if (!*p_str) return 0;

    char* p_hex = strstr((char*)p_str, "0x");
    /*
    char* p_hex = strfirstof(p_str, "xX");
    */
    if (p_hex == p_str)
        return (int)strtol(p_hex, NULL, 16);

    return (int)strtol(p_str, NULL, 10);

}

int byte_str_to_int(const char* p_str)
{
    if (!p_str) return 0;
    if (!*p_str) return 0;
    int len = strlen(p_str);
    if (len < 2)
        return 0;
    
    int mult = 1;
    int value = 0;
    
    while(--len)
    {
        char ch = *(p_str + len);
        switch(ch)
        {
            case 'b':
            case 'B': break;
            case 'k': mult = 1024;break;
            case 'M': mult = 1024*1024;break;
            case 'G': mult = 1024*1024*1024;break;
            default:  if (!(('0' <= ch) && (ch <= '9')))
                          return 0;
                      value = atoi(p_str); 
                      len = 1;
                      break;
                return 0;

        }
    }
    return value * mult;
}

bool str_to_bool_validate(std::string& str)
{
    string_helper::string_trim(str);
    string_helper::string_tolower(str);

    if (str == "true")
        return true;
    else
    if (str == "yes")
        return true;
    else
    if (str == "on")
        return true;
    else
    if (str == "1")
        return true;
    else
    if (str == "false")
        return true;
    else
    if (str == "no")
        return true;
    else
    if (str == "off")
        return true;
    else
    if (str == "0")
        return true;

    return false;
}

bool str_to_bool(std::string& str)
{
    string_helper::string_trim(str);
    string_helper::string_tolower(str);
    if (str == "true")
        return true;
    else
    if (str == "yes")
        return true;
    else
    if (str == "on")
        return true;
    else
    if (atoi(str.c_str()) != 0)
        return true;
    
    return false;
}

std::string bool_to_string(bool bVal)
{
    return (bVal?"yes":"no");
}

const char* bool_to_str(bool bVal)
{
    return (bVal?"yes":"no");
}

void string_to_char(std::string& str)
{
    if (!str.size()) 
        return;
    if (!strfirstof((char*)str.c_str(),(char*)"\\<>"))
        return;
    string_helper::string_replace(str, "\\a",    "\a");  
    string_helper::string_replace(str, "\\b",    "\b");
    string_helper::string_replace(str, "\\f",    "\f");
    string_helper::string_replace(str, "\\n",    "\n");
    string_helper::string_replace(str, "\\r",    "\r"); 
    string_helper::string_replace(str, "\\t",    "\t");
    string_helper::string_replace(str, "\\v",    "\v");
    string_helper::string_replace(str, "\\^",    "\x1b");
    string_helper::string_replace(str, "<bel>","\a");
    string_helper::string_replace(str, "<bs>", "\b"); 
    string_helper::string_replace(str, "<ff>",    "\f"); 
    string_helper::string_replace(str, "<nl>",    "\n");
    string_helper::string_replace(str, "<cr>",    "\r");
    string_helper::string_replace(str, "<lf>", "\n");
    string_helper::string_replace(str, "<tab>","\t");
    string_helper::string_replace(str, "<vtab>","\v");
    string_helper::string_replace(str, "<esc>","\01b");
    string_helper::string_replace(str, "<stx>","\x02");
    string_helper::string_replace(str, "<etx>","\x03");
    string_helper::string_replace(str, "<null>","\x00");
    string_helper::string_replace(str, "<sp>"," ");
    int xPos = 0;
    while((xPos = str.find("\\x")) != std::string::npos)
    {
        if ((int)str.size() < (xPos + 4))
            break;
        std::string sVal,strTmp;
        sVal = string_helper::string_format("0x%c%c",str.at(xPos + 2),str.at(xPos + 3));
        char cVal = (char)strtol(sVal.c_str(),NULL,16);
        if (cVal != 0)
        {
            strTmp = str.substr(0,xPos) + cVal + str.substr(xPos+4,str.size()-4);
            str = strTmp;
        }
        else
            string_helper::string_replace(str, "\\x00","/x00");
        
    }
    string_helper::string_replace(str, "/x00","\\x00");
}

void char_to_string(std::string& str)
{
    if (!str.size())
        return;
    string_helper::string_replace(str, "\a",    "<bel>");
    string_helper::string_replace(str, "\b",    "<bs>");
    string_helper::string_replace(str, "\f",    "<ff>");
    string_helper::string_replace(str, "\n",    "<nl>");
    string_helper::string_replace(str, "\r",    "<cr>");
    string_helper::string_replace(str, "\t",    "<tab>");
    string_helper::string_replace(str, "\v",    "<vtab>");
    string_helper::string_replace(str, "\x1b", "<esc>");
    string_helper::string_replace(str, "\x02", "<stx>");
    string_helper::string_replace(str, "\x03", "<etx>");
}


/**/
char* next_token(char* p_str, char* p_token, char* p_chars)
{
    bool b_do = true;
    while(b_do)
    {
        char ch = *p_str++;
        if (!ch) break;
        if (stroneof(ch, p_chars))
        {
            b_do = false;
        }
        else
        {
          *p_token = ch;
           p_token++;
        }
    }
    return p_str;

}


int validate_timestamp(char* p_str, unsigned int sz_str)
{
    ts_decode_t* p_ts_decode = _p_ts_decode;
    int idx = 0;
    while(p_ts_decode->length)
    {
        if (sz_str >= (unsigned int)p_ts_decode->length)                     
            if ((strnpattern(p_str, sz_str, p_ts_decode->pattern, 20, p_ts_decode->length)) == p_str)
                return idx;
        
        p_ts_decode++;
        idx++;
    }
    return -1;
}


inline int atomonth(char* p_month)
{
    static char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char* p_months = &months[0];
    char* p_found = strstr(p_months, p_month);
    if (p_found)
        return ((p_found - p_months) / 3);
    return 0;
}

inline time_t utc_mktime(struct tm *t)
{
    return (mktime(t) - timezone) - ((t->tm_isdst > 0) * 3600);
}

void to_timeval(uint64_t tv64, timeval& tv)
{
    tv.tv_sec =  (long) (tv64 / 1000000);
    tv.tv_usec = (long) (tv64 % 1000000);
}

bool timestamp_to_timeval(uint64_t& tv, int ts_format_idx, char* p_date_time, int len, int& ts_size)
{
    if (ts_format_idx < 0) return false;
    if (!p_date_time) return false;
    if (!*p_date_time) return false;

    if (len == -1)
        len = strlen(p_date_time);
    if (!len) return false;

    tv = 0;
    ts_size = 0;

    if (len < _ts_decode[ts_format_idx].length)
        return false;

    /*
    if (!*_ts_decode[ts_format_idx].format)
        return false;
    */
    tm tm_time = {0};
    
    char ts_buf[8][8] = {0};
    int  ts_val[8] = {0};

    char* p_next_token = p_date_time;
    char* p_ts_item = ts_buf[0];
    for (int idx = 0; idx < _ts_decode[ts_format_idx].formats; idx++, p_ts_item+=8)
        while(true)
        {
            p_next_token = next_token(p_next_token, p_ts_item, (char*)" :/.\n\r\t");
            if (!p_next_token) break;
            if ((*p_next_token) != ' ') break;
        }

    ts_size = p_next_token - p_date_time;
    
    /*
    int n = sscanf(p_date_time, _ts_decode[ts_format_idx].format,
                   ts_buf[0],ts_buf[1],ts_buf[2],ts_buf[3],
                   ts_buf[4],ts_buf[5],ts_buf[6],ts_buf[7]);
    */
    p_ts_item = ts_buf[0];
    for (int idx = 0; idx < 8; idx++)
    {
        char ch = *p_ts_item;
        if (!ch) break;
        if (isdigit(ch))
            ts_val[idx] = atoi(p_ts_item);
        else
        if (isalpha(ch))
            ts_val[idx] = atomonth(p_ts_item);
        p_ts_item += 8;
    }
    
    int idx = 0;

    if (ts_val[idx] >= 2000)
        tm_time.tm_year  = ts_val[idx++];
    tm_time.tm_mon   = ts_val[idx++];
    tm_time.tm_mday  = ts_val[idx++];
    tm_time.tm_hour  = ts_val[idx++];
    tm_time.tm_min   = ts_val[idx++];
    tm_time.tm_sec   = ts_val[idx++];

    if (tm_time.tm_year >= 2000)
        tm_time.tm_year -= 1900;
    
    tv = utc_mktime(&tm_time);
    tv *= 1000000;
    tv += ts_val[idx++] * 1000;

    return tv != 0;
}

/*
int    tm_sec   seconds [0,61]
int    tm_min   minutes [0,59]
int    tm_hour  hour [0,23]
int    tm_mday  day of month [1,31]
int    tm_mon   month of year [0,11]
int    tm_year  years since 1900
int    tm_wday  day of week [0,6] (Sunday = 0)
int    tm_yday  day of year [0,365]
int    tm_isdst daylight savings flag
*/
void xmlize(std::string& strTmp)
{
    string_helper::string_replace(strTmp," ","[sp]");
    string_helper::string_replace(strTmp,"\t","[tab]");
    string_helper::string_replace(strTmp,"\r","[cr]");
    string_helper::string_replace(strTmp,"\n","[nl]");
}

void dexmlize(std::string& strTmp)
{
    string_helper::string_replace(strTmp,"[sp]"," ");
    string_helper::string_replace(strTmp,"[tab]","\t");
    string_helper::string_replace(strTmp,"[cr]","\r");
    string_helper::string_replace(strTmp,"[nl]","\n");
}

