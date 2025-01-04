#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "scf.h" // Include header
#include "utils.h"

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_history(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
// Add these new commands to the list of built-ins
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "history",
    "remind",
    "search",
    "run",
    "learn",
    "ssh",
    "define",
    "preview",
    "compress",
    "env"};

int (*builtin_func[])(char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
    &lsh_history,
    &lsh_set_reminder, // For "remind"
    &lsh_search,       // For "search"
    &lsh_run_code,     // For "run"
    &lsh_learn,        // For "learn"
    &lsh_ssh,
    &lsh_define,
    &lsh_preview,
    &lsh_compress,
    &lsh_env};

int lsh_num_builtins()
{
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Builtin command: display history of all commands used.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_history(char **args)
{
  FILE *fp;
  char *line = NULL;
  size_t len = 0;

  // If an argument is provided
  if (args[1] != NULL)
  {
    // Check if the argument is "-c" (case-insensitive)
    if (strcasecmp(args[1], "-c") == 0)
    {
      // Attempt to remove the history file
      if (remove("history.txt") != 0)
      {
        perror("Error clearing history");
      }
      else
      {
        printf("History cleared successfully.\n");
      }
      return 1;
    }
    else
    {
      fprintf(stderr, "Invalid option: %s\n", args[1]);
      fprintf(stderr, "Usage: history [-c]\n");
      return 1;
    }
  }

  // Default behavior: display history
  printf("History of commands used:\n");
  fp = fopen("history.txt", "r");
  if (fp == NULL)
  {
    fprintf(stderr, "No history found.\n");
    return 1;
  }

  while (getline(&line, &len, fp) != -1)
  {
    printf("%s", line);
  }

  fclose(fp);
  if (line)
    free(line);
  return 1;
}

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  }
  else
  {
    // Change the current working directory to the specified path.
    // If the directory does not exist, an error message is printed.
    if (chdir(args[1]) != 0) // system call to change directory
    {
      perror("lsh");
    }
  }
  return 1;
}

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"

int lsh_help(char **args)
{
  // If no argument is passed, print the general help message
  if (args[1] == NULL)
  {
    printf(BOLD GREEN "Kritarth Dande's LSH - A Custom Shell\n" RESET);
    printf(BLUE "Welcome to the interactive shell! Here are some helpful details about each command:\n" RESET);
    printf("You can type program names and arguments, and hit enter to execute them.\n");
    printf("The following are built-in commands available to you:\n\n");

    // List of all built-ins with color
    for (int i = 0; i < sizeof(builtin_str) / sizeof(builtin_str[0]); i++)
    {
      printf("  " CYAN "%s" RESET "\n", builtin_str[i]);
    }

    printf("\nFor more information about external programs, use the 'man' command.\n");
    printf(YELLOW "Example: " RESET "man ls\n");
  }
  // If the user enters "help cd", provide specific help for the "cd" command
  else if (strcmp(args[1], "cd") == 0)
  {
    printf(BOLD CYAN "cd:\n" RESET);
    printf("    " BLUE "Changes the current working directory.\n" RESET);
    printf("    Usage: cd <path>\n");
    printf("    Example: " YELLOW "cd /home/user/projects\n" RESET);
    printf("    This command will change the current directory to the specified path.\n\n");
  }
  // If the user enters "help help", provide specific help for the "help" command
  else if (strcmp(args[1], "help") == 0)
  {
    printf(BOLD CYAN "help:\n" RESET);
    printf("    " BLUE "Displays information about shell built-in commands and other external commands.\n" RESET);
    printf("    Usage: help [command]\n");
    printf("    Example: " YELLOW "help ls\n" RESET);
    printf("    If no command is specified, a list of all built-in commands will be shown.\n\n");
  }
  // If the user enters "help exit", provide specific help for the "exit" command
  else if (strcmp(args[1], "exit") == 0)
  {
    printf(BOLD CYAN "exit:\n" RESET);
    printf("    " BLUE "Exits the shell program.\n" RESET);
    printf("    Usage: exit\n");
    printf("    Example: " YELLOW "exit\n" RESET);
    printf("    This command will terminate the shell session.\n\n");
  }
  // If the user enters "help history", provide specific help for the "history" command
  else if (strcmp(args[1], "history") == 0)
  {
    printf(BOLD CYAN "history:\n" RESET);
    printf("    " BLUE "Displays the history of previously executed commands.\n" RESET);
    printf("    Usage: history\n");
    printf("    Example: " YELLOW "history\n" RESET);
    printf("    This command will show the list of commands you have previously entered.\n\n");
  }
  // If the user enters "help remind", provide specific help for the "remind" command
  else if (strcmp(args[1], "remind") == 0)
  {
    printf(BOLD CYAN "remind:\n" RESET);
    printf("    " BLUE "Sets a reminder for a specific task or event.\n" RESET);
    printf("    Usage: remind <task> <YYYY-MM-DD HH:MM:SS>\n");
    printf("    Example: " YELLOW "remind 'Meeting with Bob' 2025-01-10 14:30:00\n" RESET);
    printf("    This command will set a reminder for the given task at the specified time.\n\n");
  }
  // If the user enters "help search", provide specific help for the "search" command
  else if (strcmp(args[1], "search") == 0)
  {
    printf(BOLD CYAN "search:\n" RESET);
    printf("    " BLUE "Searches for a given query in all files within a specified directory.\n" RESET);
    printf("    Usage: search <query> <dir>\n");
    printf("    Example: " YELLOW "search 'function' /home/user/code\n" RESET);
    printf("    This command will recursively search through the directory and list lines in files\n");
    printf("    that match the given query string.\n\n");
  }
  // If the user enters "help run", provide specific help for the "run" command
  else if (strcmp(args[1], "run") == 0)
  {
    printf(BOLD CYAN "run:\n" RESET);
    printf("    " BLUE "Executes a code file (.c or .py) in the shell.\n" RESET);
    printf("    Usage: run <filename>\n");
    printf("    Example: " YELLOW "run script.py\n" RESET);
    printf("    This command compiles and runs a C program or executes a Python script.\n\n");
  }
  // If the user enters "help learn", provide specific help for the "learn" command
  else if (strcmp(args[1], "learn") == 0)
  {
    printf(BOLD CYAN "learn:\n" RESET);
    printf("    " BLUE "Provides a tutorial or description for built-in commands like 'ls', 'cd', etc.\n" RESET);
    printf("    Usage: help learn <command>\n");
    printf("    Example: " YELLOW "help learn ls\n" RESET);
    printf("    This will provide a tutorial for the specified command.\n\n");
  }
  else if (strcmp(args[1], "ssh") == 0)
  {
    printf(BOLD CYAN "ssh:\n" RESET);
    printf("    " BLUE "The ssh command allows you to securely connect to remote machines over the network.\n" RESET);
    printf("    Usage: ssh <hostname> [options]\n");
    printf("    Example: " YELLOW "ssh user@hostname\n" RESET);
    printf("    This command will initiate an SSH connection to the specified remote host.\n");
    printf("    You can also provide a custom name for SSH connections and store passwords with '-s'.\n\n");

    printf(BOLD CYAN "Options:\n" RESET);
    printf("    -s    " BLUE "Store the SSH connection with a custom name and password.\n" RESET);
    printf("          Usage: ssh <hostname> -s\n");
    printf("          Example: " YELLOW "ssh user@hostname -s\n" RESET);
    printf("          This option saves the SSH connection details for later use.\n\n");

    printf("    " BOLD CYAN "Managing Saved Connections:\n" RESET);
    printf("    " BLUE "When using ssh without any options, you'll be presented with a list of saved connections.\n" RESET);
    printf("    You can choose to connect to any saved connection or create a new one.\n");
    printf("    Use the connection name to quickly connect to a saved host without needing to type the full hostname.\n\n\n");
  }
  else if (strcmp(args[1], "define") == 0)
  {
    printf(BOLD CYAN "define:\n" RESET);
    printf("    " BLUE "Stores and retrieves custom definitions for programming terms or concepts.\n" RESET);
    printf("    Usage:\n");
    printf("        " YELLOW "define <term> <definition>\n" RESET);
    printf("            " GREEN "Stores a definition for the specified term.\n" RESET);
    printf("        " YELLOW "define all\n" RESET);
    printf("            " GREEN "Displays all stored definitions.\n" RESET);
    printf("        " YELLOW "define <term>\n" RESET);
    printf("            " GREEN "Retrieves the definition for the specified term.\n" RESET);
    printf("    Example:\n");
    printf("        " YELLOW "define variable A container for storing data in a program.\n" RESET);
    printf("        " YELLOW "define all\n" RESET);
    printf("        " YELLOW "define variable\n" RESET);
    printf("    " BLUE "Definitions are stored in a hidden file and can be accessed at any time.\n\n" RESET);
  }
  else if (strcmp(args[1], "preview") == 0)
  {
    printf(BOLD CYAN "preview:\n" RESET);
    printf("    " BLUE "Displays the first few lines of a file for a quick preview.\n" RESET);
    printf("    Usage: preview <file> [-n <lines>]\n");
    printf("    Example: " YELLOW "preview example.txt -n 5\n" RESET);
    printf("    This will show the first 5 lines of 'example.txt'.\n\n");
  }
  else if (strcmp(args[1], "env") == 0)
  {
    printf(BOLD CYAN "env:\n" RESET);
    printf("    " BLUE "Manages and displays environment variables.\n" RESET);
    printf("    Usage: env [options]\n");
    printf("    Options:\n");
    printf("      " YELLOW "search <VAR_NAME>" RESET " : Search for a specific environment variable\n");
    printf("      " YELLOW "set <VAR_NAME> <VALUE>" RESET " : Set a new environment variable\n");
    printf("      " YELLOW "unset <VAR_NAME>" RESET " : Remove an environment variable\n");
    printf("      " YELLOW "list" RESET " : List all environment variables\n");
    printf("    Example: " YELLOW "env search PATH\n" RESET);
    printf("    This will search for the environment variable 'PATH'.\n");
    printf("    Example: " YELLOW "env set MY_VAR my_value\n" RESET);
    printf("    This will set a new environment variable 'MY_VAR' with the value 'my_value'.\n");
    printf("    Example: " YELLOW "env unset MY_VAR\n" RESET);
    printf("    This will remove the environment variable 'MY_VAR'.\n");
    printf("\n");
  }
  // If the user enters "help <other command>", print a default message for unknown commands
  else
  {
    printf("No specific help available for '%s'. Use 'help' for a general list of commands.\n", args[1]);
  }

  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  /**
   * fork() creates a new process by duplicating the calling process.
   * The new process, known as the child, is an exact copy of the parent process
   * except for the returned value. In the child process, fork() returns 0,
   * while in the parent process, it returns the child's process ID.
   * This allows both processes to execute concurrently.
   */
  pid = fork();
  if (pid == 0)
  {
    // Child process
    /**
     * Executes a program, replacing the current process image.
     */
    if (execvp(args[0], args) == -1)
    {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0)
  {
    // Error forking
    perror("lsh");
  }
  else
  {
    // Parent process
    do
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL)
  {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++)
  {
    if (strcmp(args[0], builtin_str[i]) == 0)
    {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer)
  {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1)
  {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n')
    {
      buffer[position] = '\0';
      return buffer;
    }
    else
    {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize)
    {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer)
      {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token;

  if (!tokens)
  {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL)
  {
    tokens[position] = token;
    position++;

    if (position >= bufsize)
    {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens)
      {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   @brief Loop getting input and executing it.
 */

#define MAX_CWD_LENGTH 1024

void lsh_loop(void)
{
  char *line;
  char **args;
  int status;

  do
  {
    // Get the current user's username using getlogin
    char *username = getlogin();
    if (username == NULL)
    {
      username = "unknown"; // Fallback if getlogin fails
    }

    // Get current working directory
    char cwd[MAX_CWD_LENGTH];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
      perror("getcwd error");
      strncpy(cwd, "unknown", sizeof(cwd)); // Fallback if getcwd fails
    }

    // Print custom prompt: username@pss:/current/directory$
    printf("\033[1;32m%s@pss:\033[0m\033[1;34m%s\033[0m $ ", username, cwd);

    line = lsh_read_line();

    if (line[0] != '\0') // Only write non-empty lines
    {
      FILE *fp = fopen("history.txt", "a");
      if (fp != NULL)
      {
        // Get the current working directory
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) == NULL)
        {
          perror("getcwd error");
          strncpy(cwd, "unknown", sizeof(cwd)); // Fallback if getcwd fails
        }

        // Get the current timestamp
        time_t now = time(NULL);
        char timestamp[20]; // Format: YYYY-MM-DD HH:MM:SS
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

        // Write the enhanced details to the file
        fprintf(fp, "[%s] [%s] %s\n", timestamp, cwd, line);
        fclose(fp);
      }
    }
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"

// Function to print the welcome screen
void print_welcome_screen()
{
  // Border and title
  printf("\n");
  printf(CYAN "***************************************\n" RESET);
  printf(CYAN "** " BOLD BLUE "Welcome to the Custom Shell!" RESET CYAN "      **\n" RESET);
  printf(CYAN "***************************************\n\n");

  // Description of the shell
  printf(GREEN "** " RESET "This is your interactive shell, designed for seamless command-line exploration\n");
  printf(GREEN "** " RESET "and task management. It supports multiple built-in commands and scripting.\n");
  printf("\n");

  // Instructions
  printf(BOLD "Instructions:\n" RESET);
  printf(" - " YELLOW "'help' " RESET "for a list of commands.\n");
  printf(" - " YELLOW "'exit' " RESET "to leave the shell.\n");
  printf(" - " YELLOW "'cd <dir>' " RESET "to change the directory.\n");
  printf(" - " YELLOW "'ls' " RESET "to list files in the current directory.\n");
  printf(" - " YELLOW "'remind <task> <time>' " RESET "to set a reminder.\n");
  printf(" - " YELLOW "'search <query> <dir>' " RESET "to search for a query in a directory.\n");
  printf(" - " YELLOW "'run <file>' " RESET "to execute a code file (.c or .py).\n");
  printf(" - " YELLOW "'ssh <hostname> [options]' " RESET "to securely connect to a remote machine, with optional storage for connections.\n");
  printf(" - " YELLOW "'define <term> [definition]' " RESET "to store or retrieve a definition for a programming term. Use 'define all' to list all definitions.\n");
  printf(" - " YELLOW "'preview <file> [-n <lines>]'" RESET " to quickly view the first few lines of a file.\n");

  printf("\n");

  // How to get started
  printf(CYAN "Get started by typing a command! Type " RESET "'help'" CYAN " for assistance.\n" RESET);

  // End border
  printf(CYAN "***************************************\n" RESET);
  printf("\n");
}

int main(int argc, char **argv)
{
  // Print the enhanced welcome message with instructions
  print_welcome_screen();

  // Run the command loop (the main logic of the shell)
  lsh_loop();

  // Perform any shutdown/cleanup, if necessary (though not required in this example)
  return EXIT_SUCCESS;
}
