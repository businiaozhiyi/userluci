#include <stdio.h>
#include <stdlib.h>
#include "user_uci_common.h"
#include "uci.h"
#include <string.h>

/*获取配置项的值*/

/**
 * @brief 获取配置项值
 * @param  o      	配置项
 * @param  out   	获取的配置内容
 * @return int		成功返回UCI_OK, 失败返回其它值
 */
static int uci_get_value(struct uci_option *o, char *out)
{
	struct uci_element *e;
    const char *delimiter = ';';	//值为列表时的分隔符
	bool sep = false;

	switch(o->type) {
	case UCI_TYPE_STRING:
        strcpy(out, o->v.string);
		break;
	case UCI_TYPE_LIST:
		uci_foreach_element(&o->v.list, e) {
            if(sep)
                strcat(out, delimiter);
            strcat(out, e->name);
			sep = true;
		}
		break;
	default:
		return UCI_ERR_INVAL;
		break;
	}

    return UCI_OK;
}

/**
 * @brief 获取uci配置项
 * @param  arg	获取该参数下的值
 *         eg: gateway.@interface[0]
 *             gateway.interface0.serverport
 * @param  out	获取的值存储区
 * @return int 	成功返回UCI_OK, 失败返回其它值
 */
int uci_get_str(const char *arg, char *out)
{
    struct uci_context *ctx;
    struct uci_element *e;
	struct uci_ptr ptr;
	int ret = UCI_OK;
    char *name = NULL;

    if(arg == NULL || out == NULL) return UCI_ERR_INVAL;
    name = strdup(arg);
    if(name == NULL) return UCI_ERR_INVAL;

    ctx = uci_alloc_context();
	if (!ctx) {
        free(name);
		return UCI_ERR_MEM;
	}

    if (uci_lookup_ptr(ctx, &ptr, name, true) != UCI_OK) {
        uci_free_context(ctx);
        free(name);
		return UCI_ERR_NOTFOUND;
	}

    if(UCI_LOOKUP_COMPLETE & ptr.flags)
    {
        e = ptr.last;
        switch(e->type)
        {
            case UCI_TYPE_SECTION:
                ret = UCI_OK;
            break;
            case UCI_TYPE_OPTION:
                ret = uci_get_value(ptr.o, out);
            break;
            default:
                ret = UCI_ERR_NOTFOUND;
            break;
        }
    }
    else
        ret = UCI_ERR_NOTFOUND;

    uci_free_context(ctx);
    free(name);
    return ret;
}
int fix_value(char *cfgflle, char * section, char* option ,char* value)
{
    if(cfgflle == NULL || section == NULL || option == NULL || value == NULL)
    {
        return -1;
    }
    struct  uci_context * ctx = uci_alloc_context(); //申请上下文
    
    struct uci_ptr ptr =
    {
        .package = cfgflle,
        .section = section,
        .option = option,
        .value = value,
    };
    
    uci_set(ctx,&ptr); //写入配置
    uci_commit(ctx,&ptr.p,false); //提交保存更改
    uci_unload(ctx,ptr.p);  //卸载包
    uci_free_context(ctx); //释放上下文
    return 0;
    
}
//新增一个setion
int uci_add_setion(char *cfgflle, const char * section)
{
    if(cfgflle == NULL || section == NULL)
    {
        return -1;
    }
    struct  uci_context * ctx = uci_alloc_context(); //申请上下文
    
    struct uci_ptr ptr =
    {
        .package = cfgflle,
        .section = section,
    };  
    uci_add_section(ctx,ptr.p,section,&ptr.s); //写入配置
    uci_commit(ctx,&ptr.p,false); //提交保存更改
    uci_unload(ctx,ptr.p);  //卸载包
    uci_free_context(ctx); //释放上下文
    return 0;
    
}
//删除某个option
int uci_delete_option(char *cfgflle, char * section, char* option)
{
    if(cfgflle == NULL || section == NULL || option == NULL)
    {
        return -1;
    }
    struct  uci_context * ctx = uci_alloc_context(); //申请上下文
    
    struct uci_ptr ptr =
    {
        .package = cfgflle,
        .section = section,
        .option = option,
    };
    
    uci_delete(ctx,&ptr); //写入配置
    uci_commit(ctx,&ptr.p,false); //提交保存更改
    uci_unload(ctx,ptr.p);  //卸载包
    uci_free_context(ctx); //释放上下文
    return 0;
    
}

//删除某个section
int uci_delete_section(char *cfgflle, char * section)
{
    if(cfgflle == NULL || section == NULL)
    {
        return -1;
    }
    struct  uci_context * ctx = uci_alloc_context(); //申请上下文
    
    struct uci_ptr ptr =
    {
        .package = cfgflle,
        .section = section,
    };
    
    uci_delete(ctx,&ptr); //写入配置
    uci_commit(ctx,&ptr.p,false); //提交保存更改
    uci_unload(ctx,ptr.p);  //卸载包
    uci_free_context(ctx); //释放上下文
    return 0;
    
}
/**
 * @brief 设置uci配置项 , 保存并且提交更改到文件
 * @param  arg	设置参数
 *         eg: gateway.@interface[0]=wifi-iface
 *             gateway.interface0.serverip=10.99.20.100
 *             gateway.interface0.serverport=8000
 * @return int 	成功返回UCI_OK, 失败返回其它值
 */
int uci_set_str(const char *arg)
{
    struct uci_context *ctx;
    struct uci_element *e;
	struct uci_ptr ptr;
	int ret = UCI_OK;
    char *name = NULL;

    if(arg == NULL) return UCI_ERR_INVAL;
    name = strdup(arg);
    if(name == NULL) return UCI_ERR_MEM;

    ctx = uci_alloc_context();
	if (!ctx) {
        free(name);
		return UCI_ERR_MEM;
	}

    if (uci_lookup_ptr(ctx, &ptr, name, true) != UCI_OK) {
        uci_free_context(ctx);
        free(name);
		return UCI_ERR_NOTFOUND;
	}

    ret = uci_set(ctx, &ptr);
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        free(name);
        return ret;
    }

    ret = uci_save(ctx, ptr.p);
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        free(name);
        return ret;
    }

    ret = uci_commit(ctx, &ptr.p, false);
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        free(name);
        return ret;
    }

    uci_free_context(ctx);
    free(name);
    return ret;
}
/**
 * @brief 设置uci配置项 , 保存并且提交更改到文件
 * @param  arg	设置参数
 *         eg: gateway.@interface[0]=wifi-iface
 *             gateway.interface0.serverip=10.99.20.100
 *             gateway.interface0.serverport=8000
 * @return int 	成功返回UCI_OK, 失败返回其它值
 */
int uci_del_str(const char *arg)
{
    struct uci_context *ctx;
    struct uci_element *e;
	struct uci_ptr ptr;
	int ret = UCI_OK;
    char *name = NULL;

    if(arg == NULL) return UCI_ERR_INVAL;
    name = strdup(arg);
    if(name == NULL) return UCI_ERR_MEM;

    ctx = uci_alloc_context();
	if (!ctx) {
        free(name);
		return UCI_ERR_MEM;
	}

    if (uci_lookup_ptr(ctx, &ptr, name, true) != UCI_OK) {
        uci_free_context(ctx);
        free(name);
		return UCI_ERR_NOTFOUND;
	}

    ret = uci_delete(ctx, &ptr);
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        free(name);
        return ret;
    }

    ret = uci_save(ctx, ptr.p);
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        free(name);
        return ret;
    }

    ret = uci_commit(ctx, &ptr.p, false);
    if(ret != UCI_OK)
    {
        uci_free_context(ctx);
        free(name);
        return ret;
    }

    uci_free_context(ctx);
    free(name);
    return ret;
}