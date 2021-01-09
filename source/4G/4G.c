#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wificommon.h"
#include "user_config.h"
#include "cgic.h"
#include <string.h>
#include "user_uci_common.h"
#include "uci.h"
#include <iwinfo.h>
#include "cJSON.h"
#include "AppSerial.h"
#include  <syslog.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

char AT_Cmd[4][48]={{"AT\r\n"},{"AT^SYSINFO\r\n"},{"AT+CSQ\r\n"},{"AT+COPS?\r\n"}};

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	if  ( tcgetattr( fd,&oldtio)  !=  0) { 
		//syslog("SetupSerial 1");
        syslog(LOG_DEBUG,"SetupSerial\n");
		return -1;
	}
	bzero( &newtio, sizeof( newtio ) );
	newtio.c_cflag  |=  CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;
 
	switch( nBits )
	{
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
	}
 
	switch( nEvent )
	{
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E': 
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':  
		newtio.c_cflag &= ~PARENB;
		break;
	}
 
	switch( nSpeed )
	{
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		case 460800:
			cfsetispeed(&newtio, B460800);
			cfsetospeed(&newtio, B460800);
			break;
		default:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
	}
	if( nStop == 1 )
		newtio.c_cflag &=  ~CSTOPB;
	else if ( nStop == 2 )
		newtio.c_cflag |=  CSTOPB;
		newtio.c_cc[VTIME]  = 100;///* 设置超时10 seconds*/
		newtio.c_cc[VMIN] = 0;
		tcflush(fd,TCIFLUSH);
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		//perror("com set error");
        syslog(LOG_DEBUG,"com set error\n");
		return -1;
	}
	
	//	printf("set done!\n\r");
	return 0;
}
static int Get4Ginfo(cJSON *root_info)
{
    int fd,nByte,flag=1;
    char uart3 = "/dev/ttyUSB1";
    char buffer[512];
    int i = 0;
    cJSON *root = root_info;
    char temp[100]={0};
    char buf[512]={0};
    int code =0;
    int nread=0;
    char *ret=NULL;
    char *ret2=NULL;
    int rev_len = 0;
    char operator_name[64]={0};
    int operator_name_flag=0;
    int csq=0;
    int csq1=0;
    memset(temp,0,sizeof(temp));
    if(uci_get_str("network.wan.ifname",temp)==UCI_OK) //查看是否开启使能
    {
        if(!strcmp(temp,"usb0"))    //是4G模块的接口
        {
            syslog(LOG_DEBUG,"open %s \n","/dev/ttyUSB1");
            memset(buffer, 0, sizeof(buffer));

            if((fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY ))<0)//非阻塞读方式
            {
                syslog(LOG_DEBUG,"open %s is failed\n","/dev/ttyUSB1");
            }                           
            else{
                set_opt(fd, 115200, 8, 'N', 1);
                write(fd,AT_Cmd[0], strlen(AT_Cmd[0])+1);
                while(1){
                    while((nByte = read(fd, buffer, 512))>0){
                        if(i==0)
                        {
                            buffer[nByte+1] = '\0';
                            syslog(LOG_DEBUG,"rev %s \n",buffer);
                            ret=strstr(buffer,"OK\r\n");
                            if(ret)
                            {
                                syslog(LOG_DEBUG,"ret %s \n",ret);
                            }
                            nByte = 0;
                            memset(buffer, 0, strlen(buffer));
                            i++;
                            write(fd,AT_Cmd[i], strlen(AT_Cmd[i])+1);
                            usleep(100000);
                        }
                        else if(i==1)
                        {
                            buffer[nByte+1] = '\0';
                            syslog(LOG_DEBUG,"rev %s \n",buffer);
                            ret=strstr(buffer,"^SYSINFO:");
                            if(ret)
                            {
                                syslog(LOG_DEBUG,"ret %s \n",ret);
                                ret2 = strstr(ret,"\r\n");
                                rev_len=ret2 - ret;
                                syslog(LOG_DEBUG,"ret2 %s \n",ret2);
                                syslog(LOG_DEBUG,"rev_len %d \n",rev_len);
                                memcpy(buf,ret,rev_len);
                                buf[rev_len]='\0';
                                syslog(LOG_DEBUG,"buf %s \n",buf);
                                sprintf(buf,"^SYSINFO: 2,3,0,9,1",operator_name);
                                syslog(LOG_DEBUG,"operator_name %s \n",operator_name);
                                //cJSON_AddItemToObject(root, "card_state",cJSON_CreateString("0"));
                            }	
                            nByte = 0;
                            memset(buffer, 0, strlen(buffer));
                            i++;
                            write(fd,AT_Cmd[i], strlen(AT_Cmd[i])+1);
                            usleep(100000);
                        }
                        else if(i==2)
                        {
                            buffer[nByte+1] = '\0';
                            syslog(LOG_DEBUG,"rev %s \n",buffer);	
                            ret=strstr(buffer,"+CSQ:");
                            if(ret)
                            {
                                syslog(LOG_DEBUG,"ret %s \n",ret);
                                ret2 = strstr(ret,"\r\n");
                                rev_len=ret2 - ret;
                                syslog(LOG_DEBUG,"ret2 %s \n",ret2);
                                syslog(LOG_DEBUG,"rev_len %d \n",rev_len);
                                memcpy(buf,ret,rev_len);
                                buf[rev_len]='\0';
                                syslog(LOG_DEBUG,"buf %s \n",buf);
                                sprintf(buf,"+CSQ: %d,%d",csq,csq1);
                                cJSON_AddItemToObject(root, "operator_name",cJSON_CreateString(operator_name));

                            }	
                            nByte = 0;
                            memset(buffer, 0, strlen(buffer));
                            i++;
                            write(fd,AT_Cmd[i], strlen(AT_Cmd[i])+1);
                            usleep(100000);
                        }                        
                        else if(i==3)
                        {
                            buffer[nByte+1] = '\0';
                            syslog(LOG_DEBUG,"rev %s \n",buffer);	
                            ret=strstr(buffer,"+COPS:");
                            if(ret)
                            {
                                syslog(LOG_DEBUG,"ret %s \n",ret);
                                ret2 = strstr(ret,"\r\n");
                                rev_len=ret2 - ret;
                                syslog(LOG_DEBUG,"ret2 %s \n",ret2);
                                syslog(LOG_DEBUG,"rev_len %d \n",rev_len);
                                memcpy(buf,ret,rev_len);
                                buf[rev_len]='\0';
                                syslog(LOG_DEBUG,"buf %s \n",buf);
                                sprintf(buf,"+COPS: 0,0,%s,7",operator_name);
                                syslog(LOG_DEBUG,"operator_name %s \n",operator_name);
                                syslog(LOG_DEBUG,"rev_len %d \n",rev_len);
                                cJSON_AddItemToObject(root, "operator_name",cJSON_CreateString(operator_name));

                            }	
                            nByte = 0;
                            memset(buffer, 0, strlen(buffer)); 
                            code=1;
                            flag=1;    
                            break;                      
                        }  
                    }
   

                    if(flag)
                    {
                        break;
                    }
                                    
                }
                syslog(LOG_DEBUG,"open %s is success\n","/dev/ttyUSB1");
            }
            close(fd);
        }
     
    }
    
    return code;
}
static int Get4GSwitch(cJSON * root_info)
{
    char temp[100]={0};
    char file_name[]="/sys/devices/platform/leds-gpio/leds/ap147\:4g\:switch/brightness";
    int code =-1;
    FILE *pf=NULL;
    int len = 0;
    if(root_info!=NULL)
    {
        cJSON *root = root_info;
        pf = fopen(file_name, "r");
        if (pf) {
            if (fgets(temp, sizeof(temp), pf)) {
                len = strlen(temp);              
                code = 1;
                syslog(LOG_DEBUG,"tem %s",temp);
                if(!strcmp(temp,"1\n"))
                {
                    cJSON_AddItemToObject(root, "switch",cJSON_CreateString("1"));
                }
                else if(!strcmp(temp,"0\n"))
                {
                    cJSON_AddItemToObject(root, "switch",cJSON_CreateString("0"));
                }
                
                
            }
            fclose(pf);
        }
        
    }
    return code;
}
static int GetHandle(void)
{
    cJSON *root = NULL, *data = NULL;
    char *out;
    unsigned char code = 0;
    char message[100] = {0};
    char cmd[30]={0};
    root = cJSON_CreateObject();
    data = cJSON_CreateObject();
    
    //获取cmd的值
    if(cgiFormString("cmd",cmd,sizeof(cmd))==cgiFormSuccess)
    {   
        if(strcmp(cmd,"info")==0) //获取4g参数
        {
            code = Get4Ginfo(data);
            if(code>0) 
            {
                memcpy(message,"获取4G参数成功",strlen("获取4G参数成功"));
                //syslog(LOG_INFO,"PID Information,pid %d",getpid());
                syslog(LOG_DEBUG,"get 4g parm success");

            }
            else if(code==0)
            {
                memcpy(message,"4G功能尚未打开",strlen("4G功能尚未打开")); 
                syslog(LOG_DEBUG,"4g funtion disable ");
                goto End;
            }           
            else            
            {
                memcpy(message,"获取4G参数失败",strlen("获取4G参数失败"));
                syslog(LOG_DEBUG,"get 4g parm err");
                goto End;
            }
            
        }   
        else if(strcmp(cmd,"getswitch")==0) //获取4g参数
        {
            code = Get4GSwitch(data);
            if(code>0) 
            {
                memcpy(message,"获取4G开关参数成功",strlen("获取4G开关参数成功"));
                syslog(LOG_DEBUG,"get 4g switch success");

            }       
            else            
            {
                memcpy(message,"获取4G开关参数失败",strlen("获取4G开关参数失败"));
                syslog(LOG_DEBUG,"get 4g switch err");
                goto End;
            }
            
        }        
       
    }  
    else{
        memcpy(message,"无cmd参数",strlen("无cmd参数"));
        goto End;
    } 

    code = 1;   
    
End:
    cJSON_AddNumberToObject(root,"code",code);
    cJSON_AddStringToObject(root,"message",message);
    cJSON_AddItemToObject(root,"data",data);
    out = cJSON_PrintUnformatted(root);

    cgiHeaderContentType("application/json;charset=utf-8");
    fprintf(cgiOut, "%s",out);
    cJSON_Delete(root);
    free(out);
    return 1;
    
}

//设置4G模块开关
static int Post4gSwitch(void)
{
    FILE *fp = NULL;
    char *value_srting = NULL;
    char file_name[]="/sys/devices/platform/leds-gpio/leds/ap147\:4g\:switch/brightness";
    /*获取post的数据长度*/
    int i_len = atoi(getenv("CONTENT_LENGTH"));
    char temp[100]={0};
    char buf[512];
    /*获取post的缓冲区数据*/
    char * buff=(char *) malloc(i_len+1);
    int read_len = fread(buff,1,i_len,stdin);
    int code = 1;

    if(read_len != i_len)
    {
        free(buff);
        return -1;
    }

     //解析json数据
    char cmd_string[128]={0};    //命令缓冲区
    cJSON * getroot = cJSON_Parse(buff);
    if(getroot != NULL)
    {
        cJSON *value;
        value = cJSON_GetObjectItem(getroot,"switch"); 
        if(value != NULL)
        {
            value_srting = value->valuestring;
            syslog(LOG_DEBUG,"switch");
            if(access(file_name,F_OK)==0)//判断文件是否存在
            {
                syslog(LOG_DEBUG,"file_name sccuess");
                if(!strcmp(value_srting,"1"))
                {
                    fp =popen("echo 1 > /sys/devices/platform/leds-gpio/leds/ap147\:4g\:switch/brightness","r");
                }
                else if(!strcmp(value_srting,"0"))
                {
                    fp =popen("echo 0 > /sys/devices/platform/leds-gpio/leds/ap147\:4g\:switch/brightness","r");
                }
                           
                fgets(buf,sizeof(buf),fp);
                syslog(LOG_DEBUG,"%s",buf);
                pclose(fp);
                code =1;

            }
            else
            {
                code = -1;
            }
            
        }
    }

End:
    return code;

}

static int PostHandle(void)
{
    cJSON *root = NULL, *data = NULL;
    char *out;
    unsigned char code = 0;
    char message[100] = {0};
    root = cJSON_CreateObject();
    data = cJSON_CreateObject();
    
    //获取cmd的值 
    if(strcmp(cgiQueryString,"cmd=setswitch")==0) //4G开关
    {   
        if(Post4gSwitch()>0)
        {
            memcpy(message,"设置4G开关成功",strlen("设置4G开关成功"));
        }
        else
        {
            memcpy(message,"设置4G开关失败",strlen("设置4G开关失败"));
            goto End; 
        }
        
    }  
    else{
        memcpy(message,"无cmd参数",strlen("无cmd参数"));
        goto End;
    }
    code = 1;   
    

End:
    cJSON_AddNumberToObject(root,"code",code);
    //cJSON_AddItemToObject((root,"message",cJSON_CreateString(message));
    cJSON_AddStringToObject(root,"message",message);
    cJSON_AddItemToObject(root,"data",data);
    out = cJSON_PrintUnformatted(root);

    cgiHeaderContentType("application/json;charset=utf-8");

    fprintf(cgiOut, "%s",out);
    cJSON_Delete(root);
    free(out);
    return code;
}

int cgiMain()
{
    //cgiHeaderContentType("application/json;charset=utf-8");

    /*获取cookies的值*/
    openlog("messages",LOG_PID|LOG_CONS,LOG_USER);
    char **array,**arrayStep;
    
    if(cgiCookies(&array)!= cgiFormSuccess)
    {
        return 0;
    }

    arrayStep = array;
    int count=0;
    char sysauth[100];
    while (*arrayStep)
    {
        char value[1024]={0};
        cgiCookieString(*arrayStep,value,sizeof(value));
        //fprintf(cgiOut, "<BODY><H1>Hello CGIC %s </H1></BODY>\n",*array);
        if (count == 0)
        {
            strcpy(sysauth,value);
           // fprintf(cgiOut, "<BODY><H1>Hello CGIC %s </H1></BODY>\n",sysauth);
        }       
        arrayStep++;
        count++;
       // fprintf(cgiOut, "<BODY><H1>Hello CGIC %d </H1></BODY>\n",count);
    }
    cgiStringArrayFree(array);

    /*校验tocken值*/

    /*判断请求方式*/
    if(cgiRequestMethod !=NULL)
    {
        /*进入相应的逻辑处理*/
        if(strcmp(cgiRequestMethod,"GET")==0){
            GetHandle();
        }
        else if(strcmp(cgiRequestMethod,"POST")==0){
            PostHandle();
        }
        else{
                
        }    
    } 
    closelog();
    return 0;
}
