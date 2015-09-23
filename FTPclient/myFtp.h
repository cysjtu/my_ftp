

#ifndef FTP_H_
#define FTP_H_

#include <sys/ioctl.h>
#include <net/if.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
//#include<assert.h>
#include <netinet/tcp.h>
#include"cmdparser.h"
#include <netdb.h>
#include<vector>
#include<string>

using namespace std;


class  FTPclient
{
public:
    FTPclient(void);
    ~FTPclient(void);

    int parse_cmd( char *cmd);

    //杩炴帴ftp鏈嶅姟鍣�
    int ftp_connect( const char* ip);
    //鐧诲綍ftp鏈嶅姟鍣�
    int ftp_login(char* user,char* pass);
    //鏄剧ず褰撳墠鐩綍
    int ftp_pwd(char* buff);
    //鏄剧ず鍒楄〃
    int ftp_list(char* buff);
    //鏇存敼鐩綍
    int ftp_cd(char* dir);
    //杩斿洖涓婂眰鐩綍
    int ftp_cdup();
    //鍒涘缓鐩綍
    int ftp_mkdir(char* dir);
    //鍒犻櫎鐩綍
    int ftp_rmdir(char* dir);
    //DELETE
    int ftp_dele(char* dir);
    //鏁版嵁浼犺緭妯″紡
    int ftp_setpasv();

    int ftp_setactv();

    int ftp_help(char * _buff);

    //涓婁紶鏂囦欢
    int ftp_upload(char* localfile,char* remotepath,char* remotefilename);
    //涓嬭浇鏂囦欢
    int ftp_download(char* localfile,char* remotefile);
    //閫�鍑虹櫥褰�
    int ftp_quit();

    void print_help() {
	printf("FTPclient 1.0  ,enjoy it !\n");
	printf("Develop by CY\n");
	printf("user ,pass ,cwd ,cdup ,quit ,retr ,stor ,dele ,rmd ,mkd ,pwd ,list ,help .\n\n");

    }

//鑾峰彇鏈満IP
char* GetLocalIp()
{
    int MAXINTERFACES=16;
    char *ip = NULL;
    int fd, intrface, retn = 0;
    struct ifreq buf[MAXINTERFACES];
    struct ifconf ifc;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
        {
            intrface = ifc.ifc_len / sizeof(struct ifreq);

            while (intrface-- > 0)
            {
                if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
                {
                    ip=(inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
                    break;
                }
            }
        }
        close (fd);
        return ip;
    }
}




void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
  std::string::size_type pos1, pos2;
  pos2 = s.find(c,0);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.push_back(s.substr(pos1, pos2-pos1));

    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.push_back(s.substr(pos1));
}




/**********************************************************************/

int get_line(int sock, char *buf, int size)
{
 int i = 0;
 char c = '\0';
 int n;

 while ((i < size - 1) && (c != '\n'))
 {
  n = recv(sock, &c, 1, 0);
  /* DEBUG printf("%02X\n", c); */
  if (n > 0)
  {
      //  \r\n 鏍煎紡
   if (c == '\r')
   {
    n = recv(sock, &c, 1, MSG_PEEK); //锛侊紒锛侊紒锛侊紒 peek 妯″紡锛� 妫�鏌ユ暟鎹絾鏄笉璇诲彇
    /* DEBUG printf("%02X\n", c); */
    if ((n > 0) && (c == '\n'))
     recv(sock, &c, 1, 0);
    else
     c = '\n';
   }
   buf[i] = c;
   i++;
  }
  else
   c = '\n';
 }
 buf[i] = '\0';

 return(i);
}





private:
    int m_sockctrl;//control connection
    int m_sockdata;//data connection
    char m_cmd[256];//command buffer
    char m_resp[256];//response buffer
    char m_ip[64];//ip address

    int ftp_sendcmd();//
    int ftp_checkresp(char expresp,bool need_print=false);//
    int ftp_mkdirSingle(char* dir);


};

#endif


