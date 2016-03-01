#include "contiki.h"
//#include "Cabrio-conf.h"
#include <stdio.h> /* For printf() */
#include "console.h"

////Customer AT Command
typedef struct _stParam
{
	char	atCmdBuf[MAX_LEN];
	char*	argv[10];
	int	argc;
} stParam;

typedef int (*PF_handle) (stParam *param);
typedef struct _at_cmd_info
{
	char*		atCmd;
	PF_handle	pfHandle;
} at_cmd_info;

int At_ParserInd (EVENTMSG_ID msgid, unsigned char *buff, int len) ;
int At_Parser (char *buff, int len);

