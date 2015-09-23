
#ifndef _CMDPARSER_H
#define _CMDPARSER_H

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stddef.h>
#include "defines.h"

enum {
	CMD_USER,
	CMD_PASS,
	CMD_ACCT,
	CMD_CWD,
	CMD_CDUP,
	CMD_SMNT,
	CMD_QUIT,
	CMD_REIN,
	CMD_PORT,
	CMD_PASV,
	CMD_TYPE,
	CMD_STRU,
	CMD_MODE,
	CMD_RETR,
	CMD_STOR,
	CMD_STOU,
	CMD_APPE,
	CMD_ALLO,
	CMD_REST,
	CMD_RNFR,
	CMD_RNTO,
	CMD_ABOR,
	CMD_DELE,
	CMD_RMD,
	CMD_MKD,
	CMD_PWD,
	CMD_LIST,
	CMD_NLST,
	CMD_SITE,
	CMD_SYST,
	CMD_STAT,
	CMD_HELP,
	CMD_NOOP,
	CMD_UNKNOWN,
	CMD_EMPTY,
	CMD_CLOSE
};


static int parse_input(char *input_buff,char *data_buff) {
	if(input_buff==NULL)
		return CMD_EMPTY;

	int len = strlen(input_buff);
	if(len<3)
		return CMD_UNKNOWN;

	char cmd[10];
	char *p=input_buff;
	while(*p!='\0' && *p!=' ')
	{
		++p;
	}

	if(*p)
	{
		*p++=0;
		strcpy(cmd,input_buff);
		while(*p!=0 && *p==' ') p++;
		strcpy(data_buff,p);
	}
	else
	{
		strcpy(cmd,input_buff);
	}


	if(!strcmp(cmd,"ABOR"))
	{
		return CMD_ABOR;

	}
	else if(!strcmp(cmd,"ACCT"))
	{
		return CMD_ACCT;

	}
	else if(!strcmp(cmd,"ALLO"))
	{
		return CMD_ALLO;

	}
	else if(!strcmp(cmd,"APPE"))
	{
		return CMD_APPE;

	}
	else if(!strcmp(cmd,"CWD"))
	{
		return CMD_CWD;

	}
	else if(!strcmp(cmd,"CDUP"))
	{
		return CMD_CDUP;

	}
	else if(!strcmp(cmd,"DELE"))
	{
		return CMD_DELE;

	}
	else if(!strcmp(cmd,"HELP"))
	{
		return CMD_HELP;

	}
	else if(!strcmp(cmd,"LIST"))
	{
		return CMD_LIST;

	}
	else if(!strcmp(cmd,"MKD"))
	{
		return CMD_MKD;

	}
	else if(!strcmp(cmd,"MODE"))
	{
		return CMD_MODE;

	}
	else if(!strcmp(cmd,"NLST"))
	{
		return CMD_NLST;

	}
	else if(!strcmp(cmd,"NOOP"))
	{
		return CMD_NOOP;

	}
	else if(!strcmp(cmd,"PASS"))
	{
		return CMD_PASS;

	}
	else if(!strcmp(cmd,"PASV"))
	{
		return CMD_PASV;

	}
	else if(!strcmp(cmd,"PORT"))
	{
		return CMD_PORT;

	}
	else if(!strcmp(cmd,"PWD"))
	{
		return CMD_PWD;

	}
	else if(!strcmp(cmd,"QUIT"))
	{
		return CMD_QUIT;

	}
	else if(!strcmp(cmd,"REIN"))
	{
		return CMD_REIN;

	}
	else if(!strcmp(cmd,"REST"))
	{
		return CMD_REST;

	}
	else if(!strcmp(cmd,"RETR"))
	{
		return CMD_RETR;

	}
	else if(!strcmp(cmd,"RMD"))
	{
		return CMD_RMD;

	}
	else if(!strcmp(cmd,"RNFR"))
	{
		return CMD_RNFR;

	}
	else if(!strcmp(cmd,"RNTO"))
	{
		return CMD_RNTO;

	}
	else if(!strcmp(cmd,"SITE"))
	{
		return CMD_SITE;

	}
	else if(!strcmp(cmd,"SMNT"))
	{
		return CMD_SMNT;

	}
	else if(!strcmp(cmd,"STAT"))
	{
		return CMD_STAT;

	}
	else if(!strcmp(cmd,"STOR"))
	{
		return CMD_STOR;

	}
	else if(!strcmp(cmd,"STRU"))
	{
		return CMD_STRU;

	}
	else if(!strcmp(cmd,"SYST"))
	{
		return CMD_SYST;

	}
	else if(!strcmp(cmd,"TYPE"))
	{
		return CMD_TYPE;

	}
	else if(!strcmp(cmd,"USER"))
	{
		return CMD_USER;
	}
	else
	{
		return CMD_UNKNOWN;
	}


}
#ifdef __cplusplus
}
#endif

#endif /* _CMDPARSER_H */
