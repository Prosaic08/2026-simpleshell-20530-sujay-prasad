SimpleShell (Stage 1) Manual

NAME
  simpleshell - a simple command interpreter (Stage 1)

USAGE
  Interactive mode:
  ./bin/simpleshell

Batch mode:
  ./bin/simpleshell <batchfile>

DESCRIPTION
  This shell supports the following internal commands:

COMMANDS
  cd <dir>
  Change the current working directory to <dir>.
  If <dir> is not given, changes to the HOME directory.

  clr
    Clear the terminal screen.

  dir <dir>
    List the contents of directory <dir>.
    If <dir> is not given, lists the current directory (.).

  environ
    Display all environment variables.
    Note: includes the variable 'shell' set to the full path of this executable.

  echo <text>
    Print <text> to the screen.

  help
    Display this manual file.

  pause
    Pause execution and wait for the user to press Enter.

  quit
    Exit the shell.

  NOTES
    - Commands are separated by spaces/tabs.
    - Empty lines are ignored.
    - In batch mode, commands are read line-by-line from the batch file.
