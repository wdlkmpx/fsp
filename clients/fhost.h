struct fsp_host {
    char *hostname;
    char *hostaddr; /* decimal ip address */
    char **alias;
    int port;
    int local_port;
    char *dir;
    char *local_dir;
    int delay;
    int timeout;
    int trace;
    char *password;
};

struct fsp_host * init_host(void);
void add_host(struct fsp_host *h);
void add_host_alias(struct fsp_host *h, const char *name);

/* lex parser */
extern FILE *yyin;
int yylex(void);
int yywrap(void);
