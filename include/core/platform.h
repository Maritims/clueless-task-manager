#ifndef CTM_CTM_PLATFORM_H
#define CTM_CTM_PLATFORM_H
#include <stddef.h>

int   ctm_snprintf(char* buffer, size_t size, const char* format, ...);
char* ctm_strtok_r(char* str, const char* delim, char** saveptr);
char* ctm_getusername_by_uid(unsigned int uid);
void  ctm_sleep_ms(unsigned int milliseconds);

#endif
