#ifndef _UTILS_H
#define _UTILS_H
int conf_get(const char *filename, const char *name, char *value, int value_sz);
int crontabas_parse(char* crontabs_str,char * minute, char * hour, char * date, char *month, char * week);
int conf_set_crontaba(const char *filename, const char *name, const char *value);
int conf_del_crontaba(const char *filename, const char *name);
#endif