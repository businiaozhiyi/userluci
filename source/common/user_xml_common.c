#include "libxml/parser.h"
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include "user_xml_common.h"
#include "cgic.h"
//创建xml_file
int create_xml_file(const char *xml_file)
{
    //assert(xml_file);

    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;

    //创建一个xml 文档
    doc = xmlNewDoc(BAD_CAST"1.0");
    if (doc == NULL) {
        fprintf(stderr, "Failed to new doc.\n");
        return -1;
    }

    //创建根节点
    root_node = xmlNewNode(NULL, BAD_CAST"root");
    if (root_node == NULL) {
        fprintf(stderr, "Failed to new root node.\n");
        goto FAILED;
    }

    //将根节点添加到文档中
    xmlDocSetRootElement(doc, root_node);

    //将文档保存到文件中，按照utf-8编码格式保存
    xmlSaveFormatFileEnc(xml_file, doc, "UTF-8", 1);
    xmlFreeDoc(doc);

    return 0;
    FAILED:
    if (doc) {
        xmlFreeDoc(doc);
    }
    return -1;
}

//向根节点添加一个节点
int add_node_info(const char* file_name, xmlNodePtr node)
{
    //获取根节点
    xmlDocPtr doc;  //整个xml文档的树形结构
    xmlNodePtr root_node;     //xml节点
    
    //获取树形结构
    doc = xmlParseFile(file_name);
    if(doc == NULL)
    {
        fprintf(stderr, "Failed to parse xml file:%s \n",file_name);
        goto FAILED;
    }

    //获取根节点
    root_node = xmlDocGetRootElement(doc);
    if(root_node ==NULL)
    {
        fprintf(stderr, "Failed to get root node.\n");
        goto FAILED;
    }

    //向根节点添加一个子节点
    xmlAddChild(root_node,node);

    xmlSaveFormatFileEnc(file_name,doc,"UTF-8",1);
    xmlFreeDoc(doc);

    return 0;

    FAILED:
    if (doc) {
        xmlFreeDoc(doc);
    }
    return -1;
}
//删除一个节点文件
int del_xml_node(const char * file_name, int number,const char * node_nmae)
{
    xmlNodePtr root_node = NULL;

    xmlNodePtr cur_node = NULL;
    xmlDocPtr doc;  //整个xml文档的树形结构
    int code =1;
    int i;
    //获取树形结构
    doc = xmlParseFile(file_name);
    if(doc == NULL)
    {
        fprintf(cgiOut, "Failed to parse xml file:%s \n",file_name);
        goto FAILED;
    }

    //获取根节点
    root_node = xmlDocGetRootElement(doc);
    if(root_node ==NULL)
    {
        fprintf(cgiOut, "Failed to get root node.\n");
        goto FAILED;
    }
    
    ///判断根节点是否符合预先输入的内容
    if (xmlStrcmp(root_node->name, (const xmlChar *)"root")) {
        fprintf(cgiOut, "The root is not root.\n");
        goto FAILED;
    }
    
    //遍历处理根节点的每一个子节点
    cur_node = root_node->xmlChildrenNode;
    while (cur_node != NULL) 
    {  
        if (!xmlStrcmp(cur_node->name, (const xmlChar *)node_nmae))
        {
            i++;
            if(number==i)
            {
                xmlNodePtr tempNode;
                tempNode = cur_node->next;
                xmlUnlinkNode(cur_node);
                xmlFreeNode(cur_node);
                cur_node=tempNode;
                continue;
            }

            
        }
        cur_node = cur_node->next;      
    }
    xmlSaveFormatFileEnc(file_name, doc, "UTF-8", 1);
    //xmlSaveFile(IP_GROUP_FILE,doc);
    xmlFreeDoc(doc);
    return code;
FAILED:
    if (doc) {
        xmlFreeDoc(doc);
    } 
    return 0;
}