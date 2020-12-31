#ifndef _USER_UCI_COMMON_H
#define _USER_UCI_COMMON_H

int uci_get_str(const char *arg, char *out);
int fix_value(char *cfgflle, char * section, char* option ,char* value);
int uci_set_str(const char *arg);
int uci_delete_option(char *cfgflle, char * section, char* option);
int uci_add_setion(char *cfgflle, const char * section);
int uci_delete_section(char *cfgflle, char * section);
int uci_del_str(const char *arg);
#endif
