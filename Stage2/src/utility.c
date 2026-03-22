#include "simpleshell.h"
#include <dirent.h>
#include <errno.h>

/* trim leading spaces */
char *skip_spaces(char *s) {
    while (*s == ' ' || *s == '\t') {
        s++;
    }
    return s;
}

/* print current dir prompt */
void print_prompt(void) {
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("simpleshell:%s ==> ", cwd);
    } else {
        printf("simpleshell ==> ");
    }
    fflush(stdout);
}

/* set shell environment variable to full path of executable */
void set_shell_env(char *argv0) {
    char fullpath[PATH_MAX];

    if (realpath(argv0, fullpath) != NULL) {
        setenv("shell", fullpath, 1);
    } else {
        setenv("shell", argv0, 1);
    }
}

/* dir command */
void cmd_dir(const char *path) {
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        execlp("ls", "ls", "-al", path, (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    waitpid(pid, &status, 0);
}

/* help command */
void cmd_help(void) {
    pid_t pid;
    int status;
    char manual_path[PATH_MAX];
    char shell_path[PATH_MAX];
    char *last_slash;

    if (realpath(getenv("shell"), shell_path) == NULL) {
        perror("help");
        return;
    }

    last_slash = strrchr(shell_path, '/');
    if (last_slash == NULL) {
        fprintf(stderr, "help: invalid shell path\n");
        return;
    }

    *last_slash = '\0';   /* remove /simpleshell */
    snprintf(manual_path, sizeof(manual_path), "%s/../manual/readme.txt", shell_path);

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        execlp("more", "more", manual_path, (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    waitpid(pid, &status, 0);
}

/* pause command */
void cmd_pause(void) {
    int c;

    printf("Press Enter to continue...");
    fflush(stdout);

    while ((c = getchar()) != '\n' && c != EOF) {
    /* wait until Enter */
    }
}

void init_command(Command *cmd) {
    int i;

    cmd->argc = 0;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append_output = 0;
    cmd->background = 0;

    for (i = 0; i < MAX_ARGS; i++) {
        cmd->args[i] = NULL;
    }
}


int parse_command(char *buf, Command *cmd) {
    char *token;

    init_command(cmd);

    token = strtok(buf, SEPARATORS);

    while (token != NULL) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, SEPARATORS);
            if (token == NULL) {
                fprintf(stderr, "Error: no input file given\n");
                return -1;
            }
            cmd->input_file = token;
        }


        else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, SEPARATORS);
            if (token == NULL) {
                fprintf(stderr, "Error: no output file given\n");
                return -1;
            }
            cmd->output_file = token;
            cmd->append_output = 0;
        }

        else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, SEPARATORS);
            if (token == NULL) {
                fprintf(stderr, "Error: no output file given\n");
                return -1;
            }
            cmd->output_file = token;
            cmd->append_output = 1;
        }

        else if (strcmp(token, "&") == 0) {
            cmd->background = 1;
        }

        else {
            if (cmd->argc < MAX_ARGS - 1) {
                cmd->args[cmd->argc] = token;
                cmd->argc++;
            } else {
                fprintf(stderr, "Error: too many arguments\n");
                return -1;
            }
        }

        token = strtok(NULL, SEPARATORS);
    }

    cmd->args[cmd->argc] = NULL;

    return 0;
}

void apply_redirection(Command *cmd) {
    int fd;

    if (cmd->input_file != NULL) {
        fd = open(cmd->input_file, O_RDONLY);

        if (fd < 0) {
            perror("input redirection");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDIN_FILENO) < 0) {
            perror("dup2 input");
            close(fd);
            exit(EXIT_FAILURE);
        }

        close(fd);
    }

    if (cmd->output_file != NULL) {
        if (cmd->append_output) {
            fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            fd = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }

        if (fd < 0) {
            perror("output redirection");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2 output");
            close(fd);
            exit(EXIT_FAILURE);
        }

        close(fd);
    }
}

/*
Name: Sujay Prasad
Student Number: 20530
I acknowledge the DCU Academic Integrity Policy.
*/
