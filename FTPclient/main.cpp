#include "myFtp.h"

#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace std;

char * getHostIp(const char *host)
{
    struct hostent *hent;
    hent = gethostbyname(host);
    return inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0] )   );

}


static int g_state1=1;

void strUpper(char *str)
{
	char *p=str;
	while(*p && isalpha(*p))
	{
		*p=toupper(*p);
		p++;
	}
}

int main(int argc, char* argv[])
{

    FTPclient ftpclient;
    int err;
    char buf1[300]={0};
    char buf2[300]={0};

    char *str;

    while(1)
    {
    	switch(g_state1)
    	{
    	case 1 :
    		cout<<"====================================================\n";
    		cout<<"enter the ftp server url :";
    		cin.getline(buf1,300);
    		str=getHostIp(buf1);
    		printf("connecting %s\n",str);
    		err = ftpclient.ftp_connect(str);
    		if(err)
    		{
    			printf("connect fail \n");
    			g_state1=1;
    			break;
    		}
    		printf("connect success !\n");
    		g_state1=2;
    		break;
    	case 2 :
    		cout<<"====================================================\n";
    		cout<<"\n>>>";
    		cin.getline(buf1,300);
    		strUpper(buf1);
    		if(!strcmp(buf1,"QUIT"))
    		{
    			ftpclient.ftp_quit();
    			g_state1=1;
    			break;
    		}
    		else if(!strcmp(buf1,"LOGIN"))
    		{
        		cout<<"enter your account:";
        		cin.getline(buf1,300);
        		cout<<"enter your passwd :";
        		cin.getline(buf2,300);
        	    if(strlen(buf1)==0) sprintf(buf1,"anonymous");
        	    if(strlen(buf2)==0) sprintf(buf2,"public");

        	    err = ftpclient.ftp_login(buf1,buf2);
        	    if(err)
        	    {
        	    	printf("login fail\n");
        	    	g_state1=2;
        	    	break;
        	    }
        	    printf("login success \n");
        	    g_state1=3;
        	    break;
    		}
    		else
    		{
    			printf("unkown command\n");
    		}
    		break;
    	case 3 :
    		printf("\n>>>");
    		cin.getline(buf1,300);
    		strUpper(buf1);
    		if(!strcmp(buf1,"LOGOUT") || !strcmp(buf1,"QUIT") || !strcmp(buf1,"CLOSE") )
    		{
    			err=ftpclient.ftp_quit();
    			g_state1=2;
    			break;
    		}
    		else
    		{
    			ftpclient.parse_cmd(buf1);
    		}
    		break;

    	default:
    		printf("fatal error \n");
    		exit(0);




    	}
    }



    return 0;
}


