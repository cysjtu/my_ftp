
#include <stdio.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <netdb.h>
#include <errno.h>
#include <dirent.h>
#include "defines.h"
#include "cmdparser.h"
#include "fileutils.h"

int open_connections;

bool max_limit_notify;


int raiseerr(int err_code) {
	printf("Error %d\n",err_code);
	return -1;
}




/**
 * This is neccessary for future use of glib and gettext based localization.
 */
const char * _(const char* message) {
	return message;
}



/**
 * Guess the transfer type, given the client requested type.
 * Actually in unix there is no difference between binary and
 * ascii mode when we work with file descriptors.
 * If #type is not recognized as a valid client request, -1 is returned.
 */
int get_type(const char *type) {
	if(type==NULL)
		return -1;
	int len = strlen(type);
	if(len==0)
		return -1;
	switch(type[0]) {
		case 'I':  /*IMAGE*/
			return 1;
		case 'A': /*ASCII*/
			return 2;
		case 'L':
			if(len<3)
				return -1;
			if(type[2]=='7')
				return 3;
			if(type[2]=='8')
				return 4;
	}
	return -1;
}

/**
 * Create a new connection to a (address,port) tuple, retrieved from
 * the PORT command. This connection will be used for data transfers
 * in commands like "LIST","STOR","RETR"
 */
int make_client_connection(int sock_fd,int client_port,const char* client_addr) {

   // printf("make_client_connection : %s  %d",client_addr,client_port);

	if(client_port<1) {
		send_repl(sock_fd,REPL_425);
		return -1;
	}
	int sock=-1;
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(client_addr);
	servaddr.sin_port = htons (client_port);
	if ((sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) { /*AF_INET和PF_INET的值是相同的，混用也不会有太大的问题*/
		send_repl(sock,REPL_425);
		raiseerr(15);
		return -1;
	}
	int status = connect (sock, (struct sockaddr *)&servaddr, sizeof (servaddr));
	if(status!=0) {
		send_repl(sock,REPL_425);
		return -1;
	}
	return sock;
}

/**
 * Close the connection to the client and exit the child proccess.
 * Although it is the same as close(sock_fd), in the future it can be used for
 * logging some stats about active and closed sessions.
 */
void close_conn(int sock_fd) {
	if (close(sock_fd) < 0) {
		raiseerr (5);
	}
	exit(0);  /*结束进程*/
}

/**
 * Get the next command from the client socket.
 */
int get_command(int conn_fd,char *read_buff1,char *data_buff) {
	char read_buff[RCVBUFSIZE];
	memset((char *)&read_buff, 0, RCVBUFSIZE);
	read_buff[0]='\0';
	char *rcv=read_buff;
	int cmd_status = -1;
	int recvbuff = recv(conn_fd,read_buff,RCVBUFSIZE,0);
	if(recvbuff<1) {
		return CMD_CLOSE;
	}
	if(recvbuff==RCVBUFSIZE) {
		return CMD_UNKNOWN;
	}
	printf("Received:%s\n",rcv);
	cmd_status = parse_input(rcv,data_buff);
	return cmd_status;
}

/**
 * A handler, which is called on child proccess exit.
 */
void sig_chld_handler(void) {
	open_connections--;
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

/**
 * Send reply to the client socket, given the reply.
 */
int send_repl(int send_sock,char *msg) {

    printf("Sended  :%s\n",msg);
	if (send(send_sock, msg, strlen(msg),0) < 0) {
		raiseerr (4);
		close(send_sock);
		exit(0);
	}
	return 0;
}

/**
 * Send single reply to the client socket, given the reply and its length.
 */
int send_repl_client_len(int send_sock,char *msg,int len) {
     printf("Sended  :%s\n",msg);
	if (send(send_sock, msg, len,0) < 0) {
		raiseerr (4);
		close(send_sock);
	}
	return 0;
}

/*
Izprashtane na edinishen otgovor do dopulnitelnia socket za transfer
*/
int send_repl_client(int send_sock,char *msg) {
 printf("Sended :%s\n",msg);
	send_repl_client_len(send_sock,msg,strlen(msg));
	return 0;
}

/**
 * Send single reply to the additional transfer socket, given the raply and its length.
 */
int send_repl_len(int send_sock,char *msg,int len) {
 printf("Sended :%s\n",msg);
	if (send(send_sock, msg, len,0) < 0) {
		raiseerr (4);
		close(send_sock);
		exit(0);
	}
	return 0;
}

/**
 * Parses the results from the PORT command, writes the
 * address in "client_addt" and returnes the port
 */
int parse_port_data(char *data_buff,char *client_addr) {
	client_addr[0]='\0';
	int len=0;
	int port=0;
	int _toint=0;
	char *result;
	result = strtok(data_buff, PORTDELIM);
	_toint=toint(result,FALSE);
	if(_toint<1 || _toint>254)
		return -1;
	len += strlen(result);
	strcpy(client_addr,result);
	client_addr[len]='\0';
	strcat(client_addr,".");
	len++;

	result = strtok(NULL, PORTDELIM);
	_toint=toint(result,FALSE);
	if(_toint<0 || _toint>254)
		return -1;
	len += strlen(result);
	strcat(client_addr,result);
	client_addr[len]='\0';
	strcat(client_addr,".");
	len++;

	result = strtok(NULL, PORTDELIM);
	if(_toint<0 || _toint>254)
		return -1;
	len += strlen(result);
	strcat(client_addr,result);
	client_addr[len]='\0';
	strcat(client_addr,".");
	len++;

	result = strtok(NULL, PORTDELIM);
	if(_toint<0 || _toint>254)
		return -1;
	len += strlen(result);
	strcat(client_addr,result);
	client_addr[len]='\0';

	result = strtok(NULL, PORTDELIM);
	len = toint(result,FALSE);
	if(_toint<0 || _toint>255)
		return -1;
	port = 256*len;
	result = strtok(NULL, PORTDELIM);
	len = toint(result,FALSE);
	if(_toint<0 || _toint>255)
		return -1;
	port +=len;
    printf("PORT  :%d\n",port);

	return port;
}
void print_help(int sock) {
	send_repl(sock,"    help message.\r\n \r\n");
}
/**
 * Main cycle for client<->server communication.
 * This is done synchronously. On each client message, it is parsed and recognized,
 * certain action is performed. After that we wait for the next client message
 *
 */
int interract(int conn_fd,cmd_opts *opts) {
	static int BANNER_LEN = strlen(REPL_220);
	int userid = opts->userid;
	int client_fd=-1;
	int len;
	int _type ;
	int type = 2; // ASCII TYPE by default
	if(userid>0) {

		int status = setreuid(userid,userid);  /*setreuid - set real and effective user IDs  */
		if(status != 0) {
			switch(errno) {
				case EPERM:
					break;
				case EAGAIN:
					break;
				default:
					break;
			}
			close_conn(conn_fd);
		}

	}
	if(max_limit_notify) {   /*超过允许到最大连接数目，关闭连接，结束子进程*/
		send_repl(conn_fd,REPL_120);
		close_conn(conn_fd);
	}
	char current_dir[MAXPATHLEN];
	char parent_dir[MAXPATHLEN];
	char virtual_dir[MAXPATHLEN];
	char reply[SENDBUFSIZE];
	char data_buff[DATABUFSIZE];
	char read_buff[RCVBUFSIZE];
	char *str;
	bool is_loged = FALSE;
	bool state_user = FALSE;
	char rename_from[MAXPATHLEN];

	memset((char *)&current_dir, 0, MAXPATHLEN);
	strcpy(current_dir,opts->chrootdir);
	strcpy(parent_dir,opts->chrootdir);
	free(opts); /*子进程的一份拷贝，所以要释放掉*/
	chdir(current_dir);
	if((getcwd(current_dir,MAXPATHLEN)==NULL)) { /*获得“当前工作目录”完整的绝对路径名*/
		raiseerr(19);
		close_conn(conn_fd);
	}
	memset((char *)&data_buff, 0, DATABUFSIZE);
	memset((char *)&read_buff, 0, RCVBUFSIZE);

	reply[0]='\0';
	int client_port = 0;
	char client_addr[ADDRBUFSIZE];

	send_repl_len(conn_fd,REPL_220,BANNER_LEN);
	while(1) {
		data_buff[0]='\0';
		int result = get_command(conn_fd,read_buff,data_buff);
		if(result != CMD_RNFR && result != CMD_RNTO && result != CMD_NOOP)
			rename_from[0]='\0';
		switch(result) {
			case CMD_UNKNOWN:
			case -1:
				send_repl(conn_fd,REPL_500);
				break;
			case CMD_EMPTY:
			case CMD_CLOSE:
				close_conn(conn_fd);
				break;
			case CMD_USER:
				if(data_buff==NULL || strcmp(data_buff,ANON_USER)==0) {
					state_user = TRUE;
					send_repl(conn_fd,REPL_331_ANON);
				}
				else {
					send_repl(conn_fd,REPL_332);
				}
				break;
			case CMD_PASS:
				if(!state_user) {
					send_repl(conn_fd,REPL_503);
				}
				else {
					is_loged = TRUE;  /*标记为合法状态*/
					state_user = FALSE;
					send_repl(conn_fd,REPL_230);
				}
				break;
			case CMD_PORT:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					client_port = parse_port_data(data_buff,client_addr);
					if(client_port<0) {
						send_repl(conn_fd,REPL_501);
						client_port = 0;
					} else {
						send_repl(conn_fd,REPL_200);
					}
				}
				break;
			case CMD_PASV:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					send_repl(conn_fd,REPL_502);
				}
				break;
			case CMD_SYST:
				reply[0]='\0';
				len = sprintf(reply,REPL_215,"UNIX");
				reply[len] = '\0';
				send_repl(conn_fd,reply);
				break;
			case CMD_LIST:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					client_fd = make_client_connection(conn_fd, client_port,client_addr);
					if(client_fd!=-1){
						write_list(conn_fd,client_fd,current_dir);
					}
					client_fd = -1;
				}
				break;
			case CMD_RETR:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						client_fd = make_client_connection(conn_fd, client_port,client_addr);
						if(client_fd!=-1){
							retrieve_file(conn_fd,client_fd, type,data_buff);
						}
						client_fd = -1;
					}
				}
				break;
			case CMD_STOU:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
               /*
               function generates a unique temporary filename from template, creates and opens the file,
                and returns an open file descriptor for the file.
               */
					int fd = mkstemp("XXXXX"); /*create a unique temporary file*/
					client_fd = make_client_connection(conn_fd, client_port,client_addr);
					if(client_fd!=-1){
						stou_file(conn_fd,client_fd, type,fd);
					}
					client_fd = -1;
				}
				break;
			case CMD_STOR:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						client_fd = make_client_connection(conn_fd, client_port,client_addr);
						if(client_fd!=-1){
							store_file(conn_fd,client_fd, type,data_buff);
						}
						client_fd = -1;
					}
				}
				break;
			case CMD_SITE:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						send_repl(conn_fd,REPL_202);
					}
				}
				break;
			case CMD_PWD:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					reply[0]='\0';
					len = sprintf(reply,REPL_257_PWD,current_dir);
					reply[len] = '\0';
					send_repl(conn_fd,reply);
				}
				break;
			case CMD_CDUP:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					change_dir(conn_fd,parent_dir,current_dir,virtual_dir,"..");
				}
				break;
			case CMD_CWD:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						change_dir(conn_fd,parent_dir,current_dir,virtual_dir,data_buff);
					}
				}
				break;
			case CMD_QUIT:
				send_repl(conn_fd,REPL_221);
				if(client_fd!=-1){
					close_conn(client_fd);
				}
				close_conn(conn_fd);
				break;
			case CMD_TYPE:  /*设定传输类型*/
				_type = get_type(data_buff);
				if(_type ==-1) {
					send_repl(conn_fd,REPL_500);
				}
				else {
					type=_type;
					send_repl(conn_fd,REPL_200);
				}
				break;
			case CMD_STAT:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					}
					else {
						stat_file(conn_fd,data_buff,reply);
					}
				}
				break;
			case CMD_ABOR:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(client_fd!=-1){
						close_connection(client_fd);
					}
					send_repl(conn_fd,REPL_226);
				}
				break;
			case CMD_MKD:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						make_dir(conn_fd,data_buff,reply);
					}
				}
				break;
			case CMD_RMD:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						remove_dir(conn_fd,data_buff);
					}
				}
				break;
			case CMD_DELE:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						delete_file(conn_fd,data_buff);
					}
				}
				break;
			case CMD_RNFR:  /*准备重命名文件*/
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						strcpy(rename_from,data_buff);
						send_repl(conn_fd,REPL_350);
					}
				}
				break;
			case CMD_RNTO: /*开始重命名文件*/
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						if(rename_from==NULL || strlen(rename_from)==0 || rename_from[0]=='\0') {
							send_repl(conn_fd,REPL_501);
						} else {
							rename_fr(conn_fd,rename_from,data_buff);
						}
					}
					rename_from[0]='\0';
				}
				break;
			case CMD_NOOP:
				send_repl(conn_fd,REPL_200);
				break;
			case CMD_STRU: /*结构命令(STRU)文件结构就默认使用*/
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						switch(data_buff[0]) {
							case 'F':
								send_repl(conn_fd,REPL_200);
								break;
							case 'P':
							case 'R':
								send_repl(conn_fd,REPL_504);
								break;
							default:
								send_repl(conn_fd,REPL_501);

						}
					}
				}
				break;
			case CMD_HELP:
			//	if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
					send_repl(conn_fd,REPL_214);
					print_help(conn_fd);
					send_repl(conn_fd,REPL_214_END);
					send_repl(conn_fd,"\r\n");
			//	}
			// XXX separate HELP without arguments from HELP for a single command
				break;
			case CMD_MODE:
				if(!is_loged) send_repl(conn_fd,REPL_530);
				else {
					if(data_buff==NULL || strlen(data_buff)==0 || data_buff[0]=='\0') {
						send_repl(conn_fd,REPL_501);
					} else {
						switch(data_buff[0]) {
							case 'S':
								send_repl(conn_fd,REPL_200);
								break;
							case 'B':
							case 'C':
								send_repl(conn_fd,REPL_504);
								break;
							default:
								send_repl(conn_fd,REPL_501);

						}
					}
				}
				break;
			default:
				send_repl(conn_fd,REPL_502);
		}
	}

	free(data_buff);
	free(read_buff);
	free(current_dir);
	free(parent_dir);
	free(virtual_dir);
	free(rename_from);
	close_conn(conn_fd);  /*关闭连接，结束子进程*/
}

/**
 * Close a socket and return a statsu of the close operation.
 * Although it is equivalent to close(connection) in the future it can be used
 * for writing logs about opened and closed sessions.
 */
int close_connection(int connection) {
	return close(connection);
}

/**
 * Creates new server listening socket and make the main loop , which waits
 * for new connections.
 */
int create_socket(struct cmd_opts *opts) {
	if(opts==NULL)
		return 10;
	int status = chdir(opts->chrootdir);
	if(status!=0) {
		raiseerr(15);
	}
	int servaddr_len =  0;
	int connection = 0;
	int sock = 0;
	int pid  = 0;
	open_connections=0;

	struct sockaddr_in servaddr;
	pid = getuid();
	if(pid != 0 && opts->port <= 1024)
	{
		printf(_(" Access denied:\n     Only superuser can listen to ports (1-1024).\n You can use \"-p\" option to specify port, greater than 1024.\n"));
		exit(1);
	}
	memset((char *)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = PF_INET;
	if(opts->listen_any==TRUE) {
		servaddr.sin_addr.s_addr =  htonl(INADDR_ANY);
	}
	else if(opts->listen_addr==NULL) {
		return 9;
	} else {
		struct hostent *host = gethostbyname(opts->listen_addr);
		if(host==NULL) {
			printf(_("Cannot create socket on server address: %s\n"),opts->listen_addr);
			return 11;
		}
		bcopy(host->h_addr, &servaddr.sin_addr, host->h_length);
	}
	servaddr.sin_port = htons (opts->port);
	servaddr_len = sizeof(servaddr);
	if ((sock = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		raiseerr(ERR_CONNECT);
		return 1;
	}
	int flag = 1;
	setsockopt(sock, SOL_SOCKET,SO_REUSEADDR,(char *) &flag, sizeof(int));

	// remove the Nagle algorhytm, which improves the speed of sending data.
	setsockopt(sock, IPPROTO_TCP,TCP_NODELAY,(char *) &flag, sizeof(int));

	if(bind (sock, (struct sockaddr *)&servaddr, sizeof(servaddr))<0) {
		if(opts->listen_any==FALSE) {
			printf(_("Cannot bind address: %s\n"),opts->listen_addr);
		}else {
			printf(_("Cannot bind on default address\n"));
		}
		return raiseerr(8);
	}
	if(listen(sock,opts->max_conn) <0) {
		return raiseerr(2);
	}
	#ifdef __USE_GNU
		signal(SIGCHLD, (sighandler_t )sig_chld_handler);
	#endif
	#ifdef __USE_BSD
		signal(SIGCHLD, (sig_t )sig_chld_handler);
	#endif

	for (;;) {
		max_limit_notify = FALSE;

		if ((connection = accept(sock, (struct sockaddr *) &servaddr, &servaddr_len)) < 0) {
			raiseerr(3);
			return -1;
		}

		pid = fork();
		if(pid==0) {
			if(open_connections >= opts->max_conn)
				max_limit_notify=TRUE;

			interract(connection,opts);  /*开始处理用户的命令*/
		} else if(pid>0) {
			open_connections++;    /*连接数增加*/
			assert(close_connection(connection)>=0);  /*父进程不会用到这个socket所以要关闭*/
		}
		else {  /*出现错误，关闭所有连接*/

			close(connection);
			close(sock);
			assert(0);
		}
	}
}
