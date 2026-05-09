#include "todo.h"
#include "peachos.h"
#include "string.h"

static void print_help(void)
{
    print("Commands:\n");
    print("  add <task>\n");
    print("  list\n");
    print("  remove <id>\n");
    print("  help\n");
    print("  exit\n");
}

static int parse_int(const char* text, int* out)
{
    int len = strnlen(text, 32);
    if (len <= 0)
        return -1;

    int value = 0;
    for (int i = 0; i < len; i++)
    {
        if (!isdigit(text[i]))
            return -1;
        value = (value * 10) + tonumericdigit(text[i]);
    }

    *out = value;
    return 0;
}

static int starts_with(const char* text, const char* prefix)
{
    int len = strlen(prefix);
    return strncmp(text, prefix, len) == 0;
}

int todo_run(void)
{
    print("Todo App\n");
    print("Type 'help' for commands.\n\n");

    while (1)
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
            if (strnlen(task_text, 64) == 0)
            {
                print("Usage: add <task>\n");
            }
            else if (peachos_todo_add(task_text) < 0)
            {
                print("ERROR: Failed to add task\n");
            }
            else
            {
                print("Task added\n");
            }
            continue;
        }

        if (strncmp(line, "help", 4) == 0)
        {
            print_help();
        }
        else if (strncmp(line, "list", 4) == 0)
        {
            if (peachos_todo_list() < 0)
            {
                print("ERROR: Failed to list tasks\n");
            }
        }
        else if (starts_with(line, "remove "))
        {
            int id = 0;
            if (parse_int(line + 7, &id) < 0)
            {
                print("ERROR: Invalid task ID\n");
            }
            else if (peachos_todo_remove(id) < 0)
            {
                print("ERROR: Failed to remove task\n");
            }
            else
            {
                print("Task removed\n");
            }
        }
        else if (strncmp(line, "exit", 4) == 0)
        {
            print("Bye\n");
            peachos_exit();
            return 0;
        }
        else
        {
            print("Unknown command\n");
        }
    }

    return 0;
}