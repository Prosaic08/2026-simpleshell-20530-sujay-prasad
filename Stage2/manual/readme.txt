SimpleShell (Stage 2) Manual

NAME
simpleshell - a simple command interpreter (Stage 2)

USAGE
Interactive mode:
./bin/simpleshell

Batch mode:
./bin/simpleshell

DESCRIPTION
simpleshell is a simple Linux command interpreter written in C.
It supports internal commands, external command execution, batch mode,
input/output redirection, and background execution.

INTERNAL COMMANDS

cd
Change the current working directory to .
If is not given, the shell displays the current directory.
If the directory does not exist, an error is shown.
The PWD environment variable is updated after a successful change.

clr
Clear the terminal screen.

dir
List the contents of directory .
If is not given, the current directory is listed.

environ
Display all environment variables.

echo
Display on the screen.

help
Display this manual file.

pause
Pause shell operation until Enter is pressed.

quit
Exit the shell.

EXTERNAL COMMANDS
Any command that is not one of the internal commands is treated as an
external program. The shell creates a child process using fork() and
runs the command using execvp().

Example:
./bin/simpleshell batchfile

I/O REDIRECTION
simpleshell supports input and output redirection.

Output redirection:
command > file
Send output to file. If file exists, it is overwritten.

command >> file  
Send output to file. If file exists, output is appended.

Input redirection:
command < file
Use file as input instead of the keyboard.

Redirection can be used with external commands and with the following
internal commands:
dir
environ
echo
help

Examples:
ls > files.txt
date >> log.txt
wc < files.txt
sort < names.txt > sorted.txt
echo hello > message.txt
environ > env.txt

BACKGROUND EXECUTION
If a command ends with &, it runs in the background.
The shell immediately returns to the prompt without waiting for the
command to finish.

Example:
sleep 5 &

Background execution is supported for external commands.
