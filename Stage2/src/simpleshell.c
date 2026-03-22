#include "simpleshell.h"
#include <errno.h>

int is_internal_command(const char *cmd_name) {
    if (strcmp(cmd_name, "quit") == 0) return 1;
    if (strcmp(cmd_name, "clr") == 0) return 1;
    if (strcmp(cmd_name, "cd") == 0) return 1;
    if (strcmp(cmd_name, "dir") == 0) return 1;
    if (strcmp(cmd_name, "environ") == 0) return 1;
    if (strcmp(cmd_name, "echo") == 0) return 1;
    if (strcmp(cmd_name, "help") == 0) return 1;
    if (strcmp(cmd_name, "pause") == 0) return 1;

    return 0;
}


int run_internal_command(Command *cmd) {
    int i;
    if (cmd->argc == 0) {
        return 0;
    }

    /* quit */
    if (strcmp(cmd->args[0], "quit") == 0) {
        return 1;
    }

    /* clr */
    if (strcmp(cmd->args[0], "clr") == 0) {
        pid_t pid;
        int status;

        pid = fork();

        if (pid < 0) {
            perror("fork");
            return 0;
        }

        if (pid == 0) {
            execlp("clear", "clear", (char *)NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        }

        waitpid(pid, &status, 0);
        return 0;
    }

    /* cd */
    if (strcmp(cmd->args[0], "cd") == 0) {
        if (cmd->argc < 2) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            } else {
                perror("cd");
            }
            return 0;
        }

        if (chdir(cmd->args[1]) != 0) {
            perror("cd");
        } else {
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                setenv("PWD", cwd, 1);
            }
        }
        return 0;
    }

    /* dir */
    if (strcmp(cmd->args[0], "dir") == 0) {
        const char *path = (cmd->argc >= 2) ? cmd->args[1] : ".";
        cmd_dir(path);
        return 0;
    }

    /* environ */
    if (strcmp(cmd->args[0], "environ") == 0) {
        for (char **e = environ; *e != NULL; e++) {
        printf("%s\n", *e);
        }
        return 0;
    }

    /* echo */
    if (strcmp(cmd->args[0], "echo") == 0) {
        for (i = 1; i < cmd->argc; i++) {
            printf("%s", cmd->args[i]);
            if (i < cmd->argc - 1) {
                printf(" ");
            }
        }
        printf("\n");
        return 0;
    }

    /* help */
    if (strcmp(cmd->args[0], "help") == 0) {
        cmd_help();
        return 0;
    }

    /* pause */
    if (strcmp(cmd->args[0], "pause") == 0) {
        cmd_pause();
        return 0;
    }

    return 0;
}

int run_external_command(Command *cmd) {
    pid_t pid;
    int status;

    pid = fork();

    if (pid < 0) {
        perror("fork");
        return 0;
    }

    if (pid == 0) {
        char *shell_path = getenv("shell");

        if (shell_path != NULL) {
            setenv("parent", shell_path, 1);
        }

        apply_redirection(cmd);

        execvp(cmd->args[0], cmd->args);

        perror("execvp");
        exit(EXIT_FAILURE);
    }

    if (cmd->background) {
        printf("[background pid %d]\n", pid);
        return 0;
    }

    waitpid(pid, &status, 0);
    return 0;
}

/* execute_line is only parcing, checking if command is internal and runs it. */
int execute_line(char *buf) {
    Command cmd;
    int result;
    int saved_stdin = -1;
    int saved_stdout = -1;

    if (parse_command(buf, &cmd) != 0) {
        return 0;
    }

    if (cmd.argc == 0) {
        return 0;
    }

    if (is_internal_command(cmd.args[0])) {
        if (cmd.input_file != NULL || cmd.output_file != NULL) {
            saved_stdin = dup(STDIN_FILENO);
            saved_stdout = dup(STDOUT_FILENO);

            if (saved_stdin < 0 || saved_stdout < 0) {
            perror("dup");
            return 0;
            }

            apply_redirection(&cmd);
            result = run_internal_command(&cmd);
            fflush(stdout);

            if (dup2(saved_stdin, STDIN_FILENO) < 0) {
                perror("restore stdin");
            }
            if (dup2(saved_stdout, STDOUT_FILENO) < 0) {
                perror("restore stdout");
            }

            close(saved_stdin);
            close(saved_stdout);

            return result;
        }

        return run_internal_command(&cmd);
    }

    return run_external_command(&cmd);
}


int main(int argc, char **argv) {
    char buf[MAX_BUFFER];

    set_shell_env(argv[0]);

    if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        if (!fp) {
            perror("batch file");
            return 1;
        }

        while (fgets(buf, sizeof(buf), fp)) {
            char *line = skip_spaces(buf);
            if (execute_line(line)) {
            break;
            }
        }

        fclose(fp);
        return 0;
    }


    while (1) {
        print_prompt();

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("\n");
            break;
        }

        char *line = skip_spaces(buf);
        if (execute_line(line)) {
            break;
        }
    }

    return 0;
}

