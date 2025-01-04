#ifndef SCF_H
#define SCF_H

#include <time.h>

// Maximum number of reminders
#define MAX_REMINDERS 10
#define MAX_TASK_LENGTH 100

// Reminder structure
typedef struct
{
  char task[MAX_TASK_LENGTH];
  time_t reminder_time;
} Reminder;

// Function declarations for all built-ins (as before)
int lsh_cd(char **args);
int lsh_learn(char **args);
int lsh_run_code(char **args);
int lsh_set_reminder(char **args);
int lsh_check_reminders(char **args);
int lsh_search(char **args);
int lsh_ssh(char **args);
int lsh_define(char **args);
int lsh_preview(char **args);
int lsh_compress(char **args);

#endif // SCF_H
