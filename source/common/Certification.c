#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "user_config.h"
#include "Certification.h"
//unsigned char default_token[]={0xba,0xff,0x5d,0xdd,0x78,0x89,0x45,0x57,0x78};
unsigned char default_token[] = "eyJhbGciOiJIUzI1NiJ9.eyJleHAiOjE1MDAwMDcyMDAsImlhdCI6MjAwMDAwNzIwMCwiaXNzIjoiaHR0cHM6Ly9pZHAuZXhhbXBsZS5jb20vIiwiYXVkIjoiNjM2QzY5NjU2RTc0NUY2OTY0In0.YrrT1Ddp1ampsDd2GwYZoTz_bUnLt_h--f16wsWBedk";
/*检验 1 成功 0 失败 */
int check_token(unsigned char * token_buf, const unsigned char token_len)
{
    int code = 0;

    code = 1;
    return code;
}

/*获取token 的长度*/
int get_token_len(cert_info user_cert_info)
{
    int token_len = sizeof(default_token);
      
    return token_len;

}

/*获取token的值,返回值为token的长度*/
int get_token_value(unsigned char * token_buf)
{
    int token_len = strlen(default_token);
    
    if(token_buf == NULL)
    {
        return -2;
    }

    memcpy(token_buf,default_token,token_len);

    return 1;

}


