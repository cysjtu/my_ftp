
#include <stdio.h>
#include <stdlib.h>
#include "defines.h"
#include "fileutils.h"

/**
 * Read the command line options and if
 * they are correct create a listenning server socket.
 *
 */
int main(int argc,char *argv[])
{
	struct cmd_opts *copts= malloc(sizeof(struct cmd_opts));
	int result = pars_cmd_args(copts,argc,argv);
	switch(result) {
		case 0:
			return create_socket(copts);
		case -1:
			break;
		default:
			return 1;
	}
	return 0;

}

