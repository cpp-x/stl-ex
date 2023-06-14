#pragma once

#include <stdint.h>
#include <string>
#include <regex>

char* strnpattern(char* pInput, unsigned int szInput, const char* pPattern ,unsigned int szPattern, char chReplace);
char* strnstr(char* string, const char* strSearch, int len);
char* stristr(char* String, const char* Pattern);
char* strnistr(char* String, const char* Pattern, unsigned int len);
bool  stroneof(char ch, char* p_chars);
char* strfirstof(char* p_str, char* p_chars);
int   strnchr(char* string, char ch);
int   stricmp(char* p_str_1, char* p_str_2);

int strtrimeol(char* p_str, int len);

int validate_syslog_pdu(char* p_pdu, unsigned int sz_pdu);
int validate_timestamp(char* p_str, unsigned int sz_str);

bool is_num_string(const char* p_str);
bool is_decimal(const char* p_str);
bool is_hex(const char* p_str);
bool is_byte_string(const char* p_str);

typedef struct
{
    char pattern[64];
    int  length;
    int  formats;
    //char format[64];
}ts_decode_t;

extern ts_decode_t* _p_ts_decode;

void to_timeval(uint64_t tv64, timeval& tv);
bool timestamp_to_timeval(uint64_t& tv, int ts_format_idx, char* p_pdu, int len, int& ts_size);

char* next_token(char* p_str, char* p_token, char* p_chars);

int  str_to_int(const char* p_str);
int byte_str_to_int(const char* p_str);

bool str_to_bool_validate(std::string& str);
bool str_to_bool(std::string& str);
std::string bool_to_string(bool bVal);
const char* bool_to_str(bool bVal);
void string_to_char(std::string& str);
void char_to_string(std::string& str);

void xmlize(std::string& strTmp);
void dexmlize(std::string& strTmp);

#define str_min(a,b) ((a) < (b) ? (a) : (b))

