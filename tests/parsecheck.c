#include "tweak.h"
#include "server_def.h"
#include "s_extern.h"
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include "my-string.h"

int dbug=0;

const char *testcases[]={ "", ".","filename","/filename","//filename","//dirname/filename","//dirname//filename","dirname//dir3name//","filename\npasswd",
    "file/.dir","directory.ext/filename.","/",
    NULL};

static void print_path(PPATH *pp)
{
    printf("fullpath: %s ",pp->fullp);
    if(strcmp(pp->fullp,pp->d_ptr))
    {
	printf("d_ptr: %s (%d) ",pp->d_ptr,pp->d_len);
    } else
	printf("(%d) ",pp->d_len);

    printf("f_ptr: %s (%d) ",pp->f_ptr,pp->f_len);
    if(pp->passwd)
	printf("passwd: %s",pp->passwd);
}

static void runtestcase(void)
{
    int i=0;
    PPATH pp;
    const char *err;
    const char *test;

    for(;testcases[i];i++)
    {
	test=strdup(testcases[i]);
       	err=parse_path(test,strlen(test)+1,&pp);
	printf("parsing: '%s'",test);
	if(err) 
	{
	    printf(" err: %s\n",err);
	    free(test);
	    continue;
	} else
	    printf(" okay.\n");
        printf("  ");
	print_path(&pp);
        printf("\n");
	free(test);
    }
}

int main(int argc,const char *argv[])
{
    runtestcase();
    return 0;
}
