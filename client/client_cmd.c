#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "message.h"
#include "client_cmd.h"

extern int global_fd;

void cmd_help(char **);

void cmd_quit(char **cmd)
{
	if (global_fd != 0) {
		close(global_fd);
	}	
	exit(0);
}

int connect_to_server(char *serip)
{
	if (serip == NULL)
		return -1;
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr(serip);
	
	global_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	int reuse = 1;
	setsockopt(global_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
	
	if (connect(global_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		printf ("Open: %s Failed\n", serip);
		return -1;
	}

	
	return 0;
}
void cmd_open(char **cmd)
{
	if (cmd[1] == NULL) {
		printf ("Server's ip requested\n");
		return;
	}
	connect_to_server(cmd[1]);
}
static void cmd_shell(char **cmd, int type)
{
	int i;

	if (global_fd == 0) {
		printf("You don't seem to connect with a server\n");
		return;
	}
	
	struct message *msg = (struct message *)malloc(32);
	memset(msg, 0, 32);
	
	msg->type = type;
	for (i = 0; cmd[i] != NULL; i++) {
		strcat(msg->data, cmd[i]);
		strcat(msg->data, " ");
	}
	
	int len = strlen(msg->data);
	send(global_fd, msg, len + sizeof(msg->type) -1, 0);
}
void cmd_cd(char **cmd)
{
	cmd_shell(cmd, MSG_CD);
}
void cmd_ls(char **cmd)
{
	cmd_shell(cmd, MSG_LS);
}
void cmd_get(char **cmd)
{
	struct message *msg = (struct message *)malloc(32);
	memset(msg, 0, 32);
	
	msg->type = MSG_GET;
	strcpy(msg->data, cmd[1]);
	
	send(global_fd, msg, strlen(cmd[1]) + sizeof(msg->type), 0);
}

struct cmd cmd_list[] = {
	[0] = {
		.name = "ls",
		.help_info = "",
		.cmd_func = cmd_ls,
	},[1] = {
		.name = "cd",
		.help_info = "",
		.cmd_func = cmd_cd,
	},[2] = {
		.name = "open",
		.help_info = "",
		.cmd_func = cmd_open,
	},[3] = {
		.name = "get",
		.help_info = "",
		.cmd_func = cmd_get,
	},[4] = {
		.name = "h",
		.help_info = "",
		.cmd_func = cmd_help,
	},[5] = {
		.name = "quit",
		.help_info = "",
		.cmd_func = cmd_quit,
	}
};

void cmd_help(char **cmd)
{
	int cmd_cnt = sizeof(cmd_list) / sizeof(cmd_list[0]);

	int i;
	for (i = 0; i < cmd_cnt; i++)
		printf("%d. %s%c %s\n", i, cmd_list[i].name, 9, cmd_list[i].help_info);
}
/* the command is blankspace-terminated or null-terminated*/
static int is_cmd(const char *cmd)
{
	int i, j;
	for (i = 0; cmd[i] != ' ' && cmd[i] != '\0'; i++);

	for (j = 0; j < sizeof(cmd_list) / sizeof(cmd_list[0]); j++) {
		if (!strncmp(cmd, cmd_list[j].name, i))
			return 1; 
	}
	
	return 0;
}

char **cmdstr_to_list(char *raw_cmd)
{
	int i;

	i = strlen(raw_cmd);
	raw_cmd[i - 1] = '\0';

	for (i = 0; raw_cmd[i] == ' '; i++);
	raw_cmd += i;
	
	if (*raw_cmd == '\0')
		goto out2;
	
	char **args_list = (char **)malloc(sizeof(char **) * 8);
	memset(args_list, 0, sizeof(char **) * 8);
	
	if (!is_cmd(raw_cmd))
		goto out1; 
	int k = 0;
	for (;;) {
		for (i = 0; raw_cmd[i] != ' ' && raw_cmd[i] != '\0'; i++);
		
		args_list[k] = (char *)malloc(i + 1);
		strncpy(args_list[k], raw_cmd, i);
		args_list[k][i] = '\0';
		
		raw_cmd += i;
		for (; *raw_cmd == ' '; raw_cmd++);
		
		if (*raw_cmd == '\0')
			return args_list;
		k++;
	}
out1:
	free(args_list);
out2:
	return NULL;
}
static void free_cmd(char **cmd)
{
	if (cmd == NULL)
		return;
	
	int i;
	for (i = 0; cmd[i] != NULL; i++)
		free(cmd[i]);
}
int cmd_handler(char **cmd)
{
	int i;
	for (i = 0; i < sizeof(cmd_list) / sizeof(cmd_list[0]); i++)
		if (!strcmp(cmd[0], cmd_list[i].name)) {
			cmd_list[i].cmd_func(cmd);
			free_cmd(cmd);
			return 0;
		}
	return -1;
}
