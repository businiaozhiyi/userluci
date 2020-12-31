#ifndef _CERTIFICATION_H
#define _CERTIFICATION_H

typedef struct cert_info
{
    unsigned char user_name[100];
    unsigned char user_password[100];
}cert_info;
int get_token_len(cert_info user_cert_info);
int get_token_value(unsigned char * token_buf);
int check_token(unsigned char * token_buf, const unsigned char token_len);

#endif