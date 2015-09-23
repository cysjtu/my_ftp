

#include "myFtp.h"

using namespace std;

FTPclient::FTPclient(void)
{

}

FTPclient::~FTPclient(void)
{

}


int  FTPclient::parse_cmd( char *cmd)
{

        int err=0;
        char buf2[400]={0};
        int result=parse_input(cmd,buf2);



        char *p1,*p2,*p3;

		switch(result) {
			case CMD_UNKNOWN:
			case -1:
				printf("CMD_UNKNOWN\n");
				break;
			case CMD_EMPTY:
				printf("CMD_EMPTY\n");
				break;
			case CMD_CLOSE:
				err = ftp_quit();
				//assert(err==0);
				break;
			case CMD_USER:
				break;
			case CMD_PASS:
				break;
			case CMD_PORT:
				break;
			case CMD_PASV:
				break;
			case CMD_SYST:
				break;
			case CMD_LIST:
                err = ftp_list(cmd);
				//assert(err==0);
				break;
			case CMD_RETR:
			    p1=strtok(buf2," ");
			    //cout<<p1<<endl;
			    p2=strtok(NULL," ");
			    //cout<<p2<<endl;
			    err = ftp_download(p1,p2);
			    //assert(err==0);

				break;
			case CMD_STOU:
				break;
			case CMD_STOR:
                p1=strtok(buf2," ");//cout<<p1<<endl;
			    p2=strtok(NULL," ");//cout<<p2<<endl;
			    p3=strtok(NULL," ");//cout<<p3<<endl;
			    err=ftp_upload(p1,p2,p3);
			    //assert(err==0);
				break;
			case CMD_SITE:

				break;
			case CMD_PWD:
                err = ftp_pwd( cmd);
                //assert(err==0);
				break;
			case CMD_CDUP:
			    err = ftp_cdup();
			    //assert(err==0);
				break;
			case CMD_CWD:
				err = ftp_cd(buf2);
				//assert(err==0);
				break;
			case CMD_QUIT:
				err = ftp_quit();
				//assert(err==0);
				//state=false;
				break;
			case CMD_TYPE:  /*璁惧畾浼犺緭绫诲瀷*/

				break;
			case CMD_STAT:

				break;
			case CMD_ABOR:

				break;
			case CMD_MKD:
			    err = ftp_mkdir(buf2);
			    //assert(err==0);
				break;
			case CMD_RMD:
			    err = ftp_rmdir(buf2);
			    //assert(err==0);
				break;
			case CMD_DELE:
			      err = ftp_dele(buf2);
			      //assert(err==0);
				break;
			case CMD_RNFR:  /*鍑嗗閲嶅懡鍚嶆枃浠�*/

				break;
			case CMD_RNTO: /*寮�濮嬮噸鍛藉悕鏂囦欢*/

				break;
			case CMD_NOOP:

				break;
			case CMD_STRU: /*缁撴瀯鍛戒护(STRU)鏂囦欢缁撴瀯灏遍粯璁や娇鐢�*/
				break;
			case CMD_HELP:
			    print_help();
                err = ftp_help(buf2);
                //assert(err==0);
				break;
			case CMD_MODE:
				break;
			default:
				printf("CMD_UNKNOWN");

		}

		//assert(err==0);

      return err;


}



int FTPclient::ftp_checkresp(char expresp,bool need_print)
{
   // printf("enter checkresp\n");
    //int len = recv(m_sockctrl,m_resp,256,0);
    int len=get_line(m_sockctrl, m_resp,256);

    if(-1 == len)return -1;
    m_resp[len]=0;

    //printf("Server info : ");
    //puts(m_resp);//杈撳嚭杩斿洖淇℃伅
    if(need_print)
    {
    	printf("%s\n",m_resp);
    }
  //  printf("out checkresp\n");
   // assert(m_resp[0]==expresp);
    if(m_resp[0]!=expresp)return -1;
    else if(m_resp[3]=='-')
    {
    	ftp_checkresp(expresp,need_print);
    }
    else if(m_resp[3]==' ')
    {

    }
    else
    {
    	printf("171 error\n");
    	return -1;
    }
    return 0;
}



int FTPclient::ftp_sendcmd()
{
    int ret = send(m_sockctrl,m_cmd,strlen(m_cmd),0);

   //assert(ret!=-1);
    if(-1 == ret)return -1;
    return 0;
}

int FTPclient::ftp_connect(const char* ip)
{
    m_sockctrl = socket(AF_INET,SOCK_STREAM,0);

     //assert(m_sockctrl!=0);
    if(0==m_sockctrl)return -1;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(21);
    addr.sin_addr.s_addr = inet_addr(ip);
    int err = connect(m_sockctrl,(sockaddr*)&addr,sizeof(addr));

     //assert(err==0);
    if(err)return -1;

    err = ftp_checkresp('2');
    //assert(err==0);
    if(err)return -1;
    return 0;
}

int FTPclient::ftp_login(char* user,char* pass)
{
    sprintf(m_cmd,"USER %s\r\n",user);
    int err = ftp_sendcmd();
    if(err)return -1;
    err = ftp_checkresp('3');
    if(err)return -1;
    sprintf(m_cmd,"PASS %s\r\n",pass);
    err = ftp_sendcmd();
    if(err)return -1;
    err = ftp_checkresp('2');
    if(err)return -1;
    return 0;
}

int FTPclient::ftp_quit()
{
    sprintf(m_cmd,"QUIT\r\n");
    int err = ftp_sendcmd();
    if(err)return -1;
    err = ftp_checkresp('2');
    if(err)return -1;
    return 0;
}
int FTPclient::ftp_cd(char* dir)
{
    sprintf(m_cmd,"CWD %s\r\n",dir);
    int err = ftp_sendcmd();

    //assert(err==0);
    if(err)return -1;
    err = ftp_checkresp('2');
    //assert(err==0);
    if(err)return -1;
    return 0;
}

int FTPclient::ftp_cdup()
{
    sprintf(m_cmd,"CDUP\r\n");
    int err = ftp_sendcmd();
    //assert(err==0);
    if(err)return -1;
    err = ftp_checkresp('2');
    //assert(err==0);
    if(err)return -1;
    return 0;
}

int FTPclient::ftp_pwd(char * _buff)
{
    sprintf(m_cmd,"PWD\r\n");
    int err = ftp_sendcmd();
    //assert(err==0);
    if(err)return -1;
    err = ftp_checkresp('2');
    //assert(err==0);
    if(err)return -1;
    char* p=m_resp;

    char *buff=_buff;


    while(*p!='\0')
    {
       if(*p++ == '"')
       {
          while(*p!='"')
             *buff++=*p++;
          p++;
        }
    }
    *buff=0;

    printf("%s\n",_buff);
    return 0;
}



int FTPclient::ftp_help(char * _buff)
{
    sprintf(m_cmd,"HELP\r\n");
    int err = ftp_sendcmd();
    //assert(err==0);
    if(err)return -1;
    err = ftp_checkresp('2',true);
    //assert(err==0);
    if(err)return -1;

    return 0;


}

int FTPclient::ftp_list(char* buff)
{

    ftp_setpasv();
    sprintf(m_cmd,"LIST \r\n");
    int err = ftp_sendcmd();
    //assert(err==0);
    if(err)return -1;

    err = ftp_checkresp('1'); //150
    //assert(err==0);
    if(err)return -1;


    char outbuf[3000];
    char *tmp=outbuf;

/*
    int tmpsock=-1;
    if (   ( tmpsock = accept(m_sockdata, NULL, NULL)   ) < 0) {
			return -1;
    }
*/

    char recvbuf[256];
    int len = 0;
    while((len = recv(m_sockdata,recvbuf,255,0))>0)
    {
        //err = fwrite(recvbuf,len,1,STDOUT_FILENO);
        recvbuf[len]=0;
        //err=sprintf(tmp,"%s",recvbuf);
        strcpy(tmp,recvbuf);
        tmp+=len;
       // if(len<0)return -1;
    }

    *tmp=0;

    printf("%s",outbuf);
    //close(tmpsock);
    close(m_sockdata);

    err = ftp_checkresp('2'); //226
    //assert(err==0);
    if(err)return -1;
    return 0;


}



int FTPclient::ftp_mkdirSingle(char* dir)
{
    sprintf(m_cmd,"MKD %s\r\n",dir);
    int err = ftp_sendcmd();
   // assert(err==0);
    if(err)return -1;
    err = ftp_checkresp('2');
   // assert(err==0);
  //  if(err)return -1;
    return 0;
}
int FTPclient::ftp_mkdir(char* dir)
{
    char path[300];
    int err = ftp_cd("/");
    if(err)return -1;
    int i,j;

    for(i=1,j=0;i<strlen(dir);i++)   //绗竴涓瓧鑺傛槸鏍圭洰褰�
    {
        path[j++] = dir[i];
        if(dir[i]=='/'){
            path[j++]='\0';
            printf("create :%s\n",path);
            err = ftp_mkdirSingle(path);
            //assert(err==0);
            err = ftp_cd(path);
            //assert(err==0);
            if(err)return -1;
            j=0;
        }
    }

    if(dir[i-1]!='/')
    {
    path[j++]='\0';
    err = ftp_mkdirSingle(path);
    //assert(err==0);
    err = ftp_cd(path);
    //assert(err==0);
    if(err)return -1;
    }

    return 0;
}

int FTPclient::ftp_rmdir(char* dir)
{
    sprintf(m_cmd,"RMD %s\r\n",dir);
    int err = ftp_sendcmd();
    //assert(err==0);
    if(err)return -1;
    err = ftp_checkresp('2');
    //assert(err==0);
    if(err)return -1;
    return 0;
}

int FTPclient::ftp_dele(char* dir)
{
    sprintf(m_cmd,"DELE %s\r\n",dir);
    int err = ftp_sendcmd();
    //assert(err==0);
    if(err)return -1;
    err = ftp_checkresp('2');
    //assert(err==0);
    if(err)return -1;
    return 0;
}



int FTPclient::ftp_upload(char* localfile,char* remotepath,char* remotefilename)
{

    ftp_mkdir(remotepath);
    int err = ftp_cd(remotepath);
    if(err)return -1;
    ftp_setpasv();
    sprintf(m_cmd,"STOR %s\r\n",remotefilename);
    err = ftp_sendcmd();
    //assert(err==0);
    if(err)return -1;
    err = ftp_checkresp('1');
    //assert(err==0);
    if(err)return -1;
/*
      int tmpsock=-1;


     if (   ( tmpsock = accept(m_sockdata, NULL, NULL)   ) < 0) {
            assert(0);
			return -1;
     }
    assert(tmpsock!=-1);
*/


    FILE* pf = fopen(localfile,"r");

    //assert(pf!=NULL);
    if(NULL==pf)return -1;
    char sendbuf[256];
    size_t len = 0;
    while((len = fread(sendbuf,1,255,pf))>0)
    {
        err = send(m_sockdata,sendbuf,len,0);
        //assert(err==0);
        if(err<0)return -1;
    }

    //close(tmpsock);
    close(m_sockdata);
    fclose(pf);

    err = ftp_checkresp('2');
    //assert(err==0);
    if(err)return -1;
    return 0;
}


int FTPclient::ftp_setactv()
{

   char *ip=GetLocalIp();

 	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip);
	//cout<<ip<<endl;


   char *p1,*p2,*p3,*p4;

    p1=strtok(ip,".");
    p2=strtok(NULL,".");
    p3=strtok(NULL,".");
    p4=strtok(NULL,".");
    sprintf(m_cmd,"PORT %s,%s,%s,%s,08,08\r\n",p1,p2,p3,p4);
    int err = ftp_sendcmd();
    //assert(err==0);
    if(err)return -1;

    err = ftp_checkresp('2');
    //assert(err==0);
    if(err)return -1;


	//servaddr.sin_addr.s_addr =  htonl("59.78.47.102");
	servaddr.sin_port = htons (2056);
	if ((m_sockdata = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) { /*AF_INET鍜孭F_INET鐨勫�兼槸鐩稿悓鐨勶紝娣风敤涔熶笉浼氭湁澶ぇ鐨勯棶棰�*/
	    //assert(0);
		return -1;
	}

	int flag = 1;
	setsockopt(m_sockdata, SOL_SOCKET,SO_REUSEADDR,(char *) &flag, sizeof(int));

	// remove the Nagle algorhytm, which improves the speed of sending data.
	setsockopt(m_sockdata, IPPROTO_TCP,TCP_NODELAY,(char *) &flag, sizeof(int));


	if(bind (m_sockdata, (struct sockaddr *)&servaddr, sizeof(servaddr))<0) {
	   // assert(0);
        return -1;
	}


	if(listen(m_sockdata,5) <0) {
	  // assert(0);
		return -1;
	}

	return 0;
}

int FTPclient::ftp_setpasv()
{
    sprintf(m_cmd,"PASV\r\n");
    int err = ftp_sendcmd();
    //assert(err==0);
    if(err)return -1;
    err = ftp_checkresp('2');
    //assert(err==0);
    if(err)return -1;
    m_sockdata = socket(AF_INET,SOCK_STREAM,0);
    unsigned int v[6];
    union {
        struct sockaddr sa;
        struct sockaddr_in in;
    } sin;
    sscanf(m_resp,"%*[^(](%u,%u,%u,%u,%u,%u",&v[2],&v[3],&v[4],&v[5],&v[0],&v[1]);
    sin.sa.sa_family = AF_INET;
    sin.sa.sa_data[2] = v[2];
    sin.sa.sa_data[3] = v[3];
    sin.sa.sa_data[4] = v[4];
    sin.sa.sa_data[5] = v[5];
    sin.sa.sa_data[0] = v[0];
    sin.sa.sa_data[1] = v[1];

    int on =1;
    if (setsockopt(m_sockdata,SOL_SOCKET,SO_REUSEADDR,
        (const char*) &on,sizeof(on)) == -1)
    {
        perror("setsockopt");
        close(m_sockdata);
        return -1;
    }
    struct linger lng = { 0, 0 };

    if (setsockopt(m_sockdata,SOL_SOCKET,SO_LINGER,
        (const char*) &lng,sizeof(lng)) == -1)
    {
        perror("setsockopt");
        close(m_sockdata);
        return -1;
    }

    err = connect(m_sockdata,(sockaddr*)&sin,sizeof(sin));
    //assert(err==0);
    if(err)return -1;
    return 0;
}

int FTPclient::ftp_download(char* localfile,char* remotefile)
{

	ftp_setpasv();


    sprintf(m_cmd,"RETR %s\r\n",remotefile);
    int err = ftp_sendcmd();
    //assert(err==0);
    if(err)return -1;

    err = ftp_checkresp('1'); //150
    //assert(err==0);
    if(err)return -1;

/*
    int tmpsock=-1;
    if (   ( tmpsock = accept(m_sockdata, NULL, NULL)   ) < 0) {
			return -1;
    }

    assert(tmpsock!=-1);
*/

    FILE* pf = fopen(localfile,"wb+");

   // assert(pf!=NULL);
   if(NULL==pf)
   {
   printf("娌℃湁鏉冮檺鍒涘缓鏂囦欢锛岃淇濆瓨鍒颁綘鐨刪ome鐩綍浠ュ強瀛愮洰褰曚笅\n");
   return -1;
   }



    char recvbuf[256];
    int len = 0;
    while((len = recv(m_sockdata,recvbuf,256,0))>0)
    {
        err = fwrite(recvbuf,len,1,pf);
        if(len<0)return -1;
    }
    //close(tmpsock);
    close(m_sockdata);
    fclose(pf);

    err = ftp_checkresp('2');
    //assert(err==0);
    if(err)return -1;
    return 0;
}



