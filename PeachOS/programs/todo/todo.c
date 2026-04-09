#include "todo.h"
#include "disk.h"
#include "stdio.h"
#include "stdlib.h"
#include "peachos.h"
#include "string.h"

static void todo_print_help()
{
    print("Commands:\n");
    print("  add <task>\n");
    print("  list\n");
    print("  remove <id>\n");
    print("  save <filename> <key>\n");
    print("  load <filename> <key>\n");
    print("  help\n");
    print("  exit\n");
}

static int parse_int(const char* text, int* out)
{
    int len = strnlen(text, 32);
    if (len <= 0)
    {
        return -1;
    }

    int value = 0;
    for (int i = 0; i < len; i++)
    {
        if (!isdigit(text[i]))
        {
            return -1;
        }

        value = (value * 10) + tonumericdigit(text[i]);
    }

    *out = value;
    return 0;
}

static int token_count(char** tokens, int max)
{
    int count = 0;
    for (int i = 0; i < max; i++)
    {
        if (!tokens[i])
        {
            break;
        }

        count++;
    }

    return count;
}

static void split_tokens(char* line, char** tokens, int max)
{
    for (int i = 0; i < max; i++)
    {
        tokens[i] = 0;
    }

    char* tok = strtok(line, " ");
    int i = 0;
    while(tok && i < max)
    {
        tokens[i++] = tok;
        tok = strtok(0, " ");
    }
}

static int starts_with(const char* text, const char* prefix)
{
    int len = strlen(prefix);
    return strncmp(text, prefix, len) == 0;
}

int todo_run()
{
    while(1)
    {
        print("todo> ");
        char line[256];
        peachos_terminal_readline(line, sizeof(line), true);
        print("\n");

        if (strnlen(line, sizeof(line)) == 0)
        {
            continue;
        }

        if (starts_with(line, "add "))
        {
            const char* task_text = line + 4;
            int id = sys_add_task(task_text);
            if (id < 0)
            {
                print("Failed to add task\n");
            }
            else
            {
                print("Added task with id ");
                print(itoa(id));
                print("\n");
            }
            continue;
        }

        char command_buf[256];
        strncpy(command_buf, line, sizeof(command_buf));
        char* args[4];
        split_tokens(command_buf, args, 4);
        int argc = token_count(args, 4);

        if (argc == 0)
        {
            continue;
        }

        if (strncmp(args[0], "help", 4) == 0)
        {
            todo_print_help();
        }
        else if (strncmp(args[0], "list", 4) == 0)
        {
            if (sys_list_tasks() < 0)
            {
                print("Failed to list tasks\n");
            }
        }
        else if (strncmp(args[0], "remove", 6) == 0)
        {
            if (argc < 2)
            {
                print("Usage: remove <id>\n");
                continue;
            }

            int id = 0;
            if (parse_int(args[1], &id) < 0)
            {
                print("Invalid id\n");
                continue;
            }

            if (sys_remove_task(id) < 0)
            {
                print("Failed to remove task\n");
            }
            else
            {
                print("Task removed\n");
            }
        }
        else if (strncmp(args[0], "save", 4) == 0)
        {
            if (argc < 3)
            {
                print("Usage: save <filename> <key>\n");
                continue;
            }

            if (todo_cmd_save(args[1], args[2]) < 0)
            {
                print("Save failed\n");
            }
            else
            {
                print("Tasks saved\n");
            }
        }
        else if (strncmp(args[0], "load", 4) == 0)
        {
            if (argc < 3)
            {
                print("Usage: load <filename> <key>\n");
                continue;
            }

            if (todo_cmd_load(args[1], args[2]) < 0)
            {
                print("Load failed (wrong key or filename)\n");
            }
            else
            {
                print("Tasks loaded\n");
            }
        }
        else if (strncmp(args[0], "exit", 4) == 0)
        {
            print("Exiting todo app\n");
            peachos_exit();
            return 0;
        }
        else
        {
            print("Unknown command. Type 'help'.\n");
        }
    }

    return 0;
}