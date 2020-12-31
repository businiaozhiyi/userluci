#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <unistd.h>

#include "utils.h"



char *strtrim_left(char *buf)
{
    char *p = buf;
    while (*p != '\0' && *p <= ' ')
        p++;
    return p;
}

void strtrim_right(char *buf)
{
    size_t sz = strlen(buf);
    char *ps = buf, *pe = buf + sz - 1;

    if (!sz)
        return;
    if (*pe <= ' ') {
        while (pe >= ps && *pe <= ' ')
            pe--;
        *(pe + 1) = '\0';
    }
}

char *strtrim(char *buf)
{
    char *p = strtrim_left(buf);
    strtrim_right(p);
    return p;
}


int strsplit(char *buf, char **token, size_t token_sz, const char *delim)
{
    char *p = buf, *pctx;
    size_t i;
    int tokens = 0;

    for (i = 0; i < token_sz; i++) {
        token[i] = strtok_r(p, delim, &pctx);
        if (!token[i])
            break;
        p = NULL;
        tokens++;
    }
    return tokens;
}

void bytes_swap(void *bytes, size_t sz)
{
    int i;
    unsigned char *d = (unsigned char *)bytes, tmp;
    for (i = 0; i < (int)sz; i++) {
        tmp = d[i*2];
        d[i*2] = d[(i*2)+1];
        d[(i*2)+1] = tmp;
    }
}

int conf_parse(char *line, char **name, char **value)
{
    char *d, *n, *v;

    n = strtrim_left(line);
    if (*n == '#')
        return -1;
    d = strchr(n, '=');
    if (!d)
        return -1;
    *d = '\0';
    v = strtrim_left(d+1);
    strtrim_right(n);
    strtrim_right(v);
    if (name)
        *name = n;
    if (value)
        *value = v;
    return 0;
}

int conf_get(const char *filename, const char *name, char *value, int value_sz)
{
    int err = -1;
    char buf[64], *nam, *val;
    FILE *pf;
    int str_len =0;
    int value_len=value_sz;
    pf = fopen(filename, "r");
    if (!pf)
        return -2;
    while (fgets(buf, sizeof(buf), pf)) {
/*        if (conf_parse(buf, &nam, &val) < 0)
            continue;
        if (!strcmp(name, nam)) {
            if (strlen(val) < value_sz) {
                strcpy(value, val);
                err = 0;
            } else {
                err = -1;
            }
            break;
        }
*/
        if(strstr(buf,name)!=NULL)
        {
            str_len=strlen(buf)-strlen(name)-1;
            if(str_len > value_len)
            {                
                err=-3; 
            }
            else
            {
                memcpy(value,buf,str_len);  
                err=0; 
            }  
                 
        }
    }
    
    fclose(pf);
    return err;
}

int conf_set_crontaba(const char *filename, const char *name, const char *value)
{
    char buf[512], tmp[512], *nam, *val;
    FILE *pfin = NULL, *pfout = NULL;
    int found = 0,  result = -1;

    if ((strlen(filename) + 1) >= sizeof(buf))
        return -1;

    snprintf(buf, sizeof(buf), "%s~", filename);
    unlink(buf);

    pfin = fopen(filename, "r");
    pfout = fopen(buf, "w");
    if (!pfout)
        goto clean;
    if (!pfin)
        goto notfound;
    while (fgets(buf, sizeof(buf), pfin)) {
        strcpy(tmp, buf);
     /*   if (!conf_parse(tmp, &nam, &val)) {
            if (!strcmp(name, nam)) {
                found = 1;
                if (value)
                    snprintf(buf, sizeof(buf), "%s=%s\n", name, value);
                else
                    continue;
            }
        }*/
        if(strstr(tmp,name))
        {
            found =1;
            if(value)
            {
                snprintf(buf, sizeof(buf), "%s %s\n", value, name);
            }
        }
        fwrite(buf, 1, strlen(buf), pfout);
    }
    fclose(pfin);
    pfin = NULL;
    if (found)
        goto writedone;
notfound:
    if (value) {
        snprintf(buf, sizeof(buf), "%s %s\n", value, name);
        fwrite(buf, 1, strlen(buf), pfout);
    }
writedone:
    fclose(pfout);
    pfout = NULL;
    snprintf(buf, sizeof(buf), "%s~", filename);
    rename(buf, filename);
    result = 0;
clean:
    if (pfin)
        fclose(pfin);
    if (pfout)
        fclose(pfout);
    if (result >= 0) {
        system("sync");
    }
    return result;
}
int conf_del_crontaba(const char *filename, const char *name)
{
    char buf[512], tmp[512], *nam, *val;
    FILE *pfin = NULL, *pfout = NULL;
    int found = 0,  result = -1;

    if ((strlen(filename) + 1) >= sizeof(buf))
        return -1;

    snprintf(buf, sizeof(buf), "%s~", filename);
    unlink(buf);

    pfin = fopen(filename, "r");
    pfout = fopen(buf, "w");
    if (!pfout)
        goto clean;
    if (!pfin)
        goto notfound;
    while (fgets(buf, sizeof(buf), pfin)) {
        strcpy(tmp, buf);
     /*   if (!conf_parse(tmp, &nam, &val)) {
            if (!strcmp(name, nam)) {
                found = 1;
                if (value)
                    snprintf(buf, sizeof(buf), "%s=%s\n", name, value);
                else
                    continue;
            }
        }*/
        if(strstr(tmp,name))
        {
            found =1;
        }
        else
        {
          fwrite(buf, 1, strlen(buf), pfout);          
        }

    }
    fclose(pfin);
    pfin = NULL;
    if (found)
        goto writedone;
notfound:

writedone:
    fclose(pfout);
    pfout = NULL;
    snprintf(buf, sizeof(buf), "%s~", filename);
    rename(buf, filename);
    result = 0;
clean:
    if (pfin)
        fclose(pfin);
    if (pfout)
        fclose(pfout);
    if (result >= 0) {
        system("sync");
    }
    return result;
}
int read_file_string(const char *filename, char *buf, size_t sz)
{
    int err = -1;
    FILE *pf = fopen(filename, "r");
    if (pf) {
        if (fgets(buf, sz, pf)) {
            sz = strlen(buf);
            if (sz > 0 && buf[sz-1] == '\n') {
                buf[sz-1] = '\0';
            }
            err = 0;
        }
        fclose(pf);
    }
    return err;
}

int write_file_string(const char *filename, const char *val)
{
    int err = -1;
    FILE *pf = fopen(filename, "w");
    if (pf) {
        if (fputs(val, pf) >= 0) {
            err = 0;
        }
        fclose(pf);
    }
    return err;
}
int crontabas_parse(char* crontabs_str,char * minute, char * hour, char * date, char *month, char * week)
{
    char space[]=" ";
    char * ret=NULL;
    char * temp=NULL;

    int i = 0;
    int str_len = 0;
    temp=crontabs_str;
    while ((ret=strstr(temp,space)))
    {
        i++;
        str_len=ret-temp; //获取长度,采用地址递增的规则
        switch (i)
        {
            case 1:
                memcpy(minute,temp,str_len);
                break;
            case 2:
                memcpy(hour,temp,str_len);
                break;
            case 3:
                memcpy(date,temp,str_len);
                break;                            
            case 4:
                memcpy(month,temp,str_len);
                break;
            case 5:
                memcpy(week,temp,str_len);
                break;                            
            default:
                break;
        }
        temp=ret+1;      
    }

}