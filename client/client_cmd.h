#ifndef __CLIENT_CMD_H
#define __CLIENT_CMD_H

struct cmd {
	char *name;
	char *help_info;
	void (*cmd_func)(char **);
};

int connect_to_server(char *);
void cmd_open(char **cmd);
char **cmdstr_to_list(char *);
int cmd_handler(char **);

#endif
