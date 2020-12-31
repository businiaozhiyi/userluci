#ifndef _USER_XML_COMMON_H
#define _USER_XML_COMMON_H

int create_xml_file(const char *xml_file);
int add_node_info(const char* file_name, xmlNodePtr node);
int del_xml_node(const char * file_name, int number,const char * node_nmae);

#endif
