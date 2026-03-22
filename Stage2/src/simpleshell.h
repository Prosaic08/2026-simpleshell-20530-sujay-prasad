#ifndef SIMPLESHELL_H
#define SIMPLESHELL_H

#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define MAX_BUFFER 1024
#define MAX_ARGS   64
#define SEPARATORS " \t\r\n"

extern char **environ;

/*
 * Command structure used for Stage 2 parsing.
* It stores:
* - normal command arguments
* - optional input redirection file
* - optional output redirection file
* - whether output should append
* - whether command should run in background
*/
typedef struct {
    char *args[MAX_ARGS];
    int argc;
    char *input_file;
    char *output_file;
    int append_output;
    int background;
} Command;

/* existing utility functions */
char *skip_spaces(char *s);
void print_prompt(void);
void set_shell_env(char *argv0);
void cmd_dir(const char *path);
void cmd_help(void);
void cmd_pause(void);

/* Stage 2 parsing / execution helpers */
void init_command(Command *cmd);
int parse_command(char *buf, Command *cmd);
int is_internal_command(const char *cmd_name);
int run_internal_command(Command *cmd);
int run_external_command(Command *cmd);
void apply_redirection(Command *cmd);
int execute_line(char *buf);

#endif

/*
Name: Sujay Prasad
Student Number: 20530
I acknowledge the DCU Academic Integrity Policy.
*/
