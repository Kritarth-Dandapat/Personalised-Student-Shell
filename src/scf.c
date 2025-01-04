// scf.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include "scf.h" // Include the header file

#define MAX_REMINDERS 10
#define MAX_TASK_LENGTH 100

// Array to store reminders
Reminder reminders[MAX_REMINDERS];
int reminder_count = 0;

// Function to provide help for built-in commands
int lsh_learn(char **args)
{
  if (args[1] == NULL)
    printf("No command provided for help.\n");
  else if (strcmp(args[1], "ls") == 0)
    printf("ls: Lists the files in the current directory.\n");
  else if (strcmp(args[1], "cd") == 0)
    printf("cd: Changes the current working directory.\n");
  else
    printf("No tutorial available for this command.\n");

  return 1; // Continue executing
}

// Function to compile and run code files
int lsh_run_code(char **args)
{
  if (args[1] == NULL)
  {
    printf("lsh: expected argument to \"run\"\n");
    return 1;
  }

  char command[256];
  pid_t pid = fork(); // Create a new process

  if (pid == 0)
  {
    // Child process
    if (strstr(args[1], ".c"))
    {
      // Compile C file
      snprintf(command, sizeof(command), "gcc %s -o a.out", args[1]);
      system(command);
      // Execute compiled program
      snprintf(command, sizeof(command), "./a.out");
      system(command);
    }
    else if (strstr(args[1], ".py"))
    {
      // Execute Python script
      snprintf(command, sizeof(command), "python3 %s", args[1]);
      system(command);
    }
    else
    {
      printf("Unsupported file type.\n");
    }
    exit(0); // Exit child process
  }
  else
  {
    // Parent process waits for child to finish
    wait(NULL);
  }

  return 1; // Continue executing
}

// Function to set a reminder with a task and time
int lsh_set_reminder(char **args)
{
  if (args[1] == NULL || args[2] == NULL)
  {
    printf("lsh: expected task and time arguments for \"remind\"\n");
    return 1;
  }

  struct tm tm_time = {0};
  // Parse the provided time string
  strptime(args[2], "%Y-%m-%d %H:%M:%S", &tm_time);
  // Convert to time_t and store in reminders array
  reminders[reminder_count].reminder_time = mktime(&tm_time);
  // Copy the task description
  strcpy(reminders[reminder_count].task, args[1]);
  reminder_count++;

  printf("Reminder set: %s at %s\n", args[1], args[2]);
  return 1; // Continue executing
}

// Function to check and display due reminders
int lsh_check_reminders(char **args)
{
  time_t current_time = time(NULL); // Get current time
  int found = 0;

  // Iterate through all set reminders
  for (int i = 0; i < reminder_count; i++)
  {
    if (reminders[i].reminder_time <= current_time)
    {
      // Display the reminder if due
      printf("Reminder: %s - Time: %s", reminders[i].task, ctime(&reminders[i].reminder_time));
      found = 1;
    }
  }

  if (!found)
    printf("No reminders due.\n");

  return 1; // Continue executing
}

/**
 * @brief Searches for a given query in all files within a specified directory.
 * @param args List of arguments. args[0] is "search", args[1] is the query, and args[2] is the directory.
 * @return Always returns 1, to continue executing.
 */
int lsh_search(char **args)
{
  if (args[1] == NULL || args[2] == NULL)
  {
    printf("lsh: expected query and directory arguments for \"search\"\n");
    return 1;
  }

  struct dirent *entry;
  DIR *dp = opendir(args[2]); // Open the specified directory

  if (dp == NULL)
  {
    perror("opendir"); // Print error if directory can't be opened
    return 1;
  }

  // Iterate through directory entries
  while ((entry = readdir(dp)) != NULL)
  {
    if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
    {
      // If entry is a directory, construct new path and search recursively
      char new_path[256];
      snprintf(new_path, sizeof(new_path), "%s/%s", args[2], entry->d_name);
      lsh_search(args); // Recursively search subdirectories
    }
    else if (entry->d_type == DT_REG)
    {
      // If entry is a regular file, open and search for the query
      char file_path[256];
      snprintf(file_path, sizeof(file_path), "%s/%s", args[2], entry->d_name);
      FILE *file = fopen(file_path, "r");
      if (file)
      {
        char line[256];
        int line_num = 1;
        // Read file line by line
        while (fgets(line, sizeof(line), file))
        {
          if (strstr(line, args[1]))
          {
            // If query is found, print the matching line
            printf("Match found in %s, Line %d: %s", file_path, line_num, line);
          }
          line_num++;
        }
        fclose(file); // Close the file after reading
      }
    }
  }
  closedir(dp); // Close the directory stream

  return 1; // Continue executing
}

// Store the SSH

#define MAX_SSH_LINE_LENGTH 1024

// Structure to hold SSH connection details
typedef struct
{
  char custom_name[256];
  char hostname[256];
  char password[256]; // password is hidden, not stored in clear text
} SSHConnection;

// Function to load saved SSH connections from ssh.txt
int load_ssh_connections(SSHConnection *connections)
{
  FILE *file = fopen(".ssh/ssh.txt", "r");
  if (file == NULL)
  {
    perror("Could not open SSH file");
    return 0;
  }

  int count = 0;
  while (fscanf(file, "%s %s %s", connections[count].custom_name, connections[count].hostname, connections[count].password) != EOF)
  {
    count++;
  }
  fclose(file);
  return count;
}

// Function to save SSH connections to ssh.txt
void save_ssh_connections(SSHConnection *connections, int count)
{
  FILE *file = fopen(".ssh/ssh.txt", "w");
  if (file == NULL)
  {
    perror("Could not open SSH file");
    return;
  }

  for (int i = 0; i < count; i++)
  {
    fprintf(file, "%s %s %s\n", connections[i].custom_name, connections[i].hostname, connections[i].password);
  }
  fclose(file);
}

// Function to list saved SSH connections
void list_ssh_connections(SSHConnection *connections, int count)
{
  if (count == 0)
  {
    printf("No saved SSH connections found.\n");
    return;
  }

  printf("Saved SSH Connections:\n");
  for (int i = 0; i < count; i++)
  {
    printf("[%d] %s -> %s\n", i + 1, connections[i].custom_name, connections[i].hostname);
  }
}

// Function to delete a saved SSH connection
void delete_ssh_connection(SSHConnection *connections, int *count, int index)
{
  if (index < 0 || index >= *count)
  {
    printf("Invalid connection index.\n");
    return;
  }

  // Shift remaining connections to overwrite the deleted one
  for (int i = index; i < *count - 1; i++)
  {
    connections[i] = connections[i + 1];
  }

  (*count)--;                                // Reduce the connection count
  save_ssh_connections(connections, *count); // Save updated connections
  printf("SSH connection deleted successfully.\n");
}

// Main SSH function to handle both saving, connecting, and deleting connections
int lsh_ssh(char **args)
{
  static SSHConnection connections[10];
  static int count = 0;
  static int password_needed = 1;

  if (args[1] == NULL)
  {
    // If no arguments, list saved connections or proceed with regular SSH
    list_ssh_connections(connections, count);
    return 1;
  }

  if (strcmp(args[1], "-s") == 0)
  {
    // Save the connection with custom name and password
    if (args[2] == NULL || args[3] == NULL)
    {
      printf("Usage: ssh -s <custom_name> <hostname>\n");
      return 1;
    }

    // Get custom name and hostname
    char custom_name[256];
    char hostname[256];
    char password[256];

    strcpy(custom_name, args[2]);
    strcpy(hostname, args[3]);

    printf("Enter the password for %s (will not be shown): ", hostname);
    // Take password input without showing
    system("stty -echo"); // Turn off echo to hide password
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Remove the newline
    system("stty echo");                   // Turn echo back on

    // Save the new connection
    strcpy(connections[count].custom_name, custom_name);
    strcpy(connections[count].hostname, hostname);
    strcpy(connections[count].password, password);
    count++;

    save_ssh_connections(connections, count); // Save the connection to the file

    printf("SSH connection saved: %s -> %s\n", custom_name, hostname);
    return 1;
  }

  // If the user wants to delete an existing connection
  if (strcmp(args[1], "-d") == 0)
  {
    // List and delete connection
    list_ssh_connections(connections, count);
    printf("Enter the number of the connection to delete: ");
    int index;
    scanf("%d", &index);
    delete_ssh_connection(connections, &count, index - 1);
    return 1;
  }

  // Otherwise, treat as normal ssh and connect using the provided hostname
  printf("Connecting to %s...\n", args[1]);
  char ssh_command[MAX_SSH_LINE_LENGTH];
  snprintf(ssh_command, sizeof(ssh_command), "ssh %s", args[1]);
  system(ssh_command);

  return 1;
}

// Store functional definations
#define DEFINITIONS_FILE ".definitions.txt"

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"
#define BOLD "\x1b[1m"

void store_definition(const char *keyword, const char *definition)
{
  FILE *file = fopen(DEFINITIONS_FILE, "a");
  if (file == NULL)
  {
    perror("Could not open definitions file");
    return;
  }

  // Store the keyword with its definition
  fprintf(file, "%s = %s\n", keyword, definition);
  fclose(file);

  printf(GREEN "Definition for '%s' stored successfully!\n" RESET, keyword);
}

void retrieve_definition(const char *keyword)
{
  FILE *file = fopen(DEFINITIONS_FILE, "r");
  if (file == NULL)
  {
    perror("Could not open definitions file");
    return;
  }

  char line[512];
  int found = 0;
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strncmp(line, keyword, strlen(keyword)) == 0)
    {
      printf(CYAN "Definition for '%s': %s" RESET, keyword, line + strlen(keyword) + 3); // Skip the keyword and "= "
      found = 1;
      break;
    }
  }

  if (!found)
  {
    printf(RED "No definition found for '%s'.\n" RESET, keyword);
  }

  fclose(file);
}

void delete_definition(const char *keyword)
{
  FILE *file = fopen(DEFINITIONS_FILE, "r");
  if (file == NULL)
  {
    perror("Could not open definitions file");
    return;
  }

  FILE *temp_file = fopen("temp_defs.txt", "w");
  if (temp_file == NULL)
  {
    perror("Could not open temporary file");
    fclose(file);
    return;
  }

  char line[512];
  int found = 0;
  while (fgets(line, sizeof(line), file) != NULL)
  {
    if (strncmp(line, keyword, strlen(keyword)) == 0)
    {
      found = 1; // Skip the line
    }
    else
    {
      fprintf(temp_file, "%s", line); // Copy the line to the temp file
    }
  }

  if (found)
  {
    printf(GREEN "Definition for '%s' deleted successfully.\n" RESET, keyword);
    remove(DEFINITIONS_FILE);
    rename("temp_defs.txt", DEFINITIONS_FILE); // Replace the old file with the new one
  }
  else
  {
    printf(RED "No definition found for '%s'.\n" RESET, keyword);
    remove("temp_defs.txt");
  }

  fclose(file);
  fclose(temp_file);
}

void show_all_definitions()
{
  FILE *file = fopen(DEFINITIONS_FILE, "r");
  if (file == NULL)
  {
    perror("Could not open definitions file");
    return;
  }

  char line[512];
  printf(BOLD CYAN "All Definitions:\n" RESET);
  while (fgets(line, sizeof(line), file) != NULL)
  {
    printf(YELLOW "%s" RESET, line);
  }

  fclose(file);
}

int lsh_define(char **args)
{
  // If no arguments, print usage instructions
  if (args[1] == NULL)
  {
    printf(RED "Usage: define <keyword> <definition> to store a definition\n" RESET);
    printf("or use 'define <keyword>' to retrieve a definition\n");
    printf("or use 'define delete <keyword>' to delete a definition\n");
    printf("or use 'define all' to list all definitions\n");
    return 1;
  }

  // Show all definitions
  if (strcmp(args[1], "all") == 0)
  {
    show_all_definitions();
  }
  // Delete definition
  else if (strcmp(args[1], "delete") == 0 && args[2] != NULL)
  {
    delete_definition(args[2]);
  }
  // Store new definition
  else if (args[2] != NULL)
  {
    store_definition(args[1], args[2]);
  }
  // Retrieve definition
  else
  {
    retrieve_definition(args[1]);
  }

  return 1;
}

#define DEFAULT_PREVIEW_LINES 10

int lsh_preview(char **args)
{
  if (args[1] == NULL)
  {
    printf("Usage: preview <file> [-n <lines>]\n");
    return 1;
  }

  char *file_name = args[1];
  int lines_to_show = DEFAULT_PREVIEW_LINES;

  // Parse optional `-n` flag
  for (int i = 2; args[i] != NULL; i++)
  {
    if (strcmp(args[i], "-n") == 0 && args[i + 1] != NULL)
    {
      lines_to_show = atoi(args[i + 1]);
      if (lines_to_show <= 0)
      {
        printf("Invalid number of lines: %s\n", args[i + 1]);
        return 1;
      }
      i++; // Skip the number
    }
  }

  // Open the file
  FILE *file = fopen(file_name, "r");
  if (file == NULL)
  {
    perror("Error opening file");
    return 1;
  }

  // Read and display the file line by line
  char buffer[1024];
  int line_count = 0;
  printf(BOLD BLUE "Previewing first %d lines of %s:\n" RESET, lines_to_show, file_name);
  while (fgets(buffer, sizeof(buffer), file) != NULL)
  {
    printf("%s", buffer);
    if (++line_count >= lines_to_show)
    {
      break;
    }
  }

  // If the file has fewer lines than requested
  if (line_count < lines_to_show)
  {
    printf(BOLD YELLOW "\n[End of file reached, %d lines displayed.]\n" RESET, line_count);
  }

  fclose(file);
  return 1;
}

#define ENCRYPT_CMD "openssl enc -aes-256-cbc -salt -in "
#define DECRYPT_CMD "openssl enc -d -aes-256-cbc -salt -in "

int lsh_encrypt(char **args)
{
  if (args[1] == NULL || args[2] == NULL)
  {
    printf("Usage: encrypt <input_file> <output_file>\n");
    return 1;
  }

  char command[512];
  printf("Encrypting %s to %s...\n", args[1], args[2]);

  // Construct the encryption command
  snprintf(command, sizeof(command), "%s%s -out %s", ENCRYPT_CMD, args[1], args[2]);

  // Execute the command
  if (system(command) == 0)
  {
    printf("File encrypted successfully: %s\n", args[2]);
  }
  else
  {
    printf("Encryption failed.\n");
  }

  return 1;
}

/**
 * @brief Decrypt a file using AES-256-CBC decryption.
 * @param args Command arguments, where args[1] is the input file, args[2] is the output file.
 * @return 1 to continue the shell.
 */
int lsh_decrypt(char **args)
{
  if (args[1] == NULL || args[2] == NULL)
  {
    printf("Usage: decrypt <input_file> <output_file>\n");
    return 1;
  }

  char command[512];
  printf("Decrypting %s to %s...\n", args[1], args[2]);

  // Construct the decryption command
  snprintf(command, sizeof(command), "%s%s -out %s", DECRYPT_CMD, args[1], args[2]);

  // Execute the command
  if (system(command) == 0)
  {
    printf("File decrypted successfully: %s\n", args[2]);
  }
  else
  {
    printf("Decryption failed.\n");
  }

  return 1;
}

#define COMPRESS_CMD "tar -czf " // Command to compress files using tar
int lsh_compress(char **args)
{
  if (args[1] == NULL)
  {
    printf("Usage: compress <output_archive.tar.gz> <file1> <file2> ... <fileN>\n");
    return 1;
  }

  char command[1024];
  int i;

  // Construct the command to compress files into a .tar.gz archive
  snprintf(command, sizeof(command), "%s%s ", COMPRESS_CMD, args[1]);

  // Add all input files to the command
  for (i = 2; args[i] != NULL; i++)
  {
    snprintf(command + strlen(command), sizeof(command) - strlen(command), "%s ", args[i]);
  }

  // Execute the command
  if (system(command) == 0)
  {
    printf("Files compressed successfully into %s\n", args[1]);
  }
  else
  {
    printf("Compression failed.\n");
  }

  return 1;
}
