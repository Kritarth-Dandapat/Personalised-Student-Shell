#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"

// Color definitions for better visibility
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

// Function for searching environment variable
int lsh_search_env(char **args)
{
  if (args[2] == NULL)
  {
    printf(RED "Usage: search <VAR_NAME>\n" RESET);
    return 1;
  }

  char *value = getenv(args[2]);
  if (value == NULL)
  {
    printf(RED "Environment variable '%s' not found.\n" RESET, args[2]);
  }
  else
  {
    printf(GREEN "Found %s=%s\n" RESET, args[2], value);
  }

  return 1;
}

// Function for setting environment variable
int lsh_setenv(char **args)
{
  if (args[2] == NULL || args[3] == NULL)
  {
    printf(RED "Usage: set <VAR_NAME> <VALUE>\n" RESET);
    return 1;
  }

  if (setenv(args[2], args[3], 1) == 0)
  {
    printf(GREEN "Environment variable '%s' set to '%s'\n" RESET, args[2], args[3]);
  }
  else
  {
    perror(RED "Error setting environment variable" RESET);
  }

  return 1;
}

// Function for unsetting (deleting) an environment variable
int lsh_unsetenv(char **args)
{
  if (args[2] == NULL)
  {
    printf(RED "Usage: unset <VAR_NAME>\n" RESET);
    return 1;
  }

  if (unsetenv(args[2]) == 0)
  {
    printf(GREEN "Environment variable '%s' removed.\n" RESET, args[2]);
  }
  else
  {
    perror(RED "Error removing environment variable" RESET);
  }

  return 1;
}

// Function to list all environment variables with color
int lsh_custom_list_env(char **args)
{
  extern char **environ;

  printf(CYAN "Custom listing of environment variables:\n" RESET);
  printf("----------------------------------------\n");

  for (int i = 0; environ[i] != NULL; i++)
  {
    // Get the environment variable and its value
    char *env_var = environ[i];
    char *var_name = strtok(env_var, "=");
    char *var_value = strtok(NULL, "=");

    // Print the variable name in blue and its value in green
    printf(BLUE "%s" RESET " = " GREEN "%s\n" RESET, var_name, var_value);
  }

  printf("----------------------------------------\n");
  return 1;
}

// Main function to handle the 'env' command
int lsh_env(char **args)
{
  if (args[1] == NULL)
  {
    // If no argument is provided, run the system's default env command.
    if (fork() == 0)
    {
      // Child process
      execvp("env", args);
      perror("execvp");
      exit(EXIT_FAILURE);
    }
    else
    {
      // Parent process waits for the child to finish
      wait(NULL);
    }
  }
  else
  {
    // Custom behavior for environment management
    if (strcmp(args[1], "search") == 0)
    {
      return lsh_search_env(args);
    }
    else if (strcmp(args[1], "set") == 0)
    {
      return lsh_setenv(args);
    }
    else if (strcmp(args[1], "unset") == 0)
    {
      return lsh_unsetenv(args);
    }
    else
    {
      // Default: list the environment variables with custom color formatting
      return lsh_custom_list_env(args);
    }
  }
  return 1;
}
