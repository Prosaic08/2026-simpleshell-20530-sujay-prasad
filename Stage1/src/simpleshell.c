#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // chdir, getcwd
#include <dirent.h>     // opendir, readdir, closedir
#include <limits.h>     // PATH_MAX
#include <errno.h>
#include <linux/limits.h>

#define MAX_BUFFER 1024
#define MAX_ARGS   64
#define SEPARATORS " \t\r\n"


#ifndef PATH_MAX
#define PATH_MAX 4096
#endif
/* for environ command */
extern char **environ;

/* trim leading spaces */
static char *skip_spaces(char *s) {
    while (*s == ' ' || *s == '\t') s++;
    return s;
}

/* print current dir prompt */
static void print_prompt(void) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("simpleshell:%s ==> ", cwd);
    } else {
        printf("simpleshell ==> ");
    }
    fflush(stdout);
}

/* set shell environment variable to full path of executable */
static void set_shell_env(char *argv0) {
    char fullpath[PATH_MAX];

    /* realpath resolves ./bin/simpleshell into an absolute path */
    if (realpath(argv0, fullpath) != NULL) {
        setenv("shell", fullpath, 1);
    } else {
    /* fallback: set whatever we got (better than nothing) */
        setenv("shell", argv0, 1);
    }
}

/* dir command: list directory contents */
static void cmd_dir(const char *path) {
    DIR *d = opendir(path);
    if (!d) {
        perror("dir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        printf("%s\n", entry->d_name);
    }
    closedir(d);
}

/* help command: print manual/help file */
static void cmd_help(void) {
    FILE *fp = fopen("manual/readme.txt", "r");
    if (!fp) {
        perror("help (could not open manual/readme.txt)");
        return;
    }

    char line[MAX_BUFFER];
    while (fgets(line, sizeof(line), fp)) {
        fputs(line, stdout);
    }
    fclose(fp);
}


/* pause command: wait for Enter */
static void cmd_pause(void) {
int c;
    printf("Press Enter to continue...");
    fflush(stdout);

    /* consume until newline or EOF */
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* run one command line (returns 1 if should quit, else 0) */
static int execute_line(char *buf) {
    char *args[MAX_ARGS];
    char *arg;
    int argc = 0;

    /* tokenize into args[] */
    arg = strtok(buf, SEPARATORS);
    while (arg != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = arg;
        arg = strtok(NULL, SEPARATORS);
    }
    args[argc] = NULL;

    if (argc == 0) {
        return 0; // empty line
    }

    /* quit */
    if (strcmp(args[0], "quit") == 0) {
        return 1;
    }

    /* clr */
    if (strcmp(args[0], "clr") == 0) {
        /* ANSI clear: home + clear screen */
        printf("\033[H\033[J");
        return 0;
    }


    /* cd */
    if (strcmp(args[0], "cd") == 0) {
        const char *target = NULL;

        if (argc >= 2) target = args[1];
        else target = getenv("HOME");   // cd with no args => HOME

        if (!target) {
            fprintf(stderr, "cd: HOME not set\n");
            return 0;
        }

        if (chdir(target) != 0) {
            perror("cd");
        }
        return 0;
    }


    /* dir */
    if (strcmp(args[0], "dir") == 0) {
        const char *path = (argc >= 2) ? args[1] : ".";
        cmd_dir(path);
        return 0;
    }


    /* environ */
    if (strcmp(args[0], "environ") == 0) {
        for (char **e = environ; *e != NULL; e++) {
            printf("%s\n", *e);
        }
        return 0;
    }


    /* echo */
    if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; i < argc; i++) {
            printf("%s", args[i]);
            if (i < argc - 1) printf(" ");
        }
        printf("\n");
        return 0;
    }

    /* help */
    if (strcmp(args[0], "help") == 0) {
        cmd_help();
        return 0;
    }


    /* pause */
    if (strcmp(args[0], "pause") == 0) {
        cmd_pause();
        return 0;
    }

    /* Stage 1: we only implement internal commands.
    If unknown command, show a friendly message. */
    fprintf(stderr, "Unknown command: %s\n", args[0]);
        return 0;
    }


int main(int argc, char **argv) {
    char buf[MAX_BUFFER];

    /* Stage 1 requirement: set shell environment variable */
    set_shell_env(argv[0]);


    /* batch mode: if a filename is provided */
    if (argc == 2) {
        FILE *fp = fopen(argv[1], "r");
        if (!fp) {
            perror("batch file");
            return 1;
        }

        while (fgets(buf, sizeof(buf), fp)) {
            /* allow lines with leading spaces */
            char *line = skip_spaces(buf);

            /* execute; if quit, stop */
            if (execute_line(line)) break;
        }


        fclose(fp);
        return 0;
    }



    /* interactive mode */
    while (1) {
        print_prompt();

        if (!fgets(buf, sizeof(buf), stdin)) {
            /* EOF (Ctrl+D) */
            printf("\n");
            break;
        }

        char *line = skip_spaces(buf);
        if (execute_line(line)) break;
    }

    return 0;
}



