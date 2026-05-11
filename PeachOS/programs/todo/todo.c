#include "todo.h"
#include "peachos.h"
#include "string.h"
#define MSG_WELCOME       "Todo App\n"
#define MSG_HINT          "Type 'help' for commands.\n\n"
#define MSG_PROMPT        "todo> "
#define MSG_BYE           "Bye\n"
#define MSG_UNKNOWN       "Unknown command\n"
#define MSG_USAGE_ADD     "Usage: add <task>\n"
#define MSG_BAD_ID        "ERROR: Invalid task ID\n"
#define MSG_ADD_OK        "Task added\n"
#define MSG_ADD_FAIL      "ERROR: Failed to add task\n"
#define MSG_LIST_FAIL     "ERROR: Failed to list tasks\n"
#define MSG_REMOVE_OK     "Task removed\n"
#define MSG_REMOVE_FAIL   "ERROR: Failed to remove task\n"

static void print_help(void)
{
    print("Commands:\n");
    print("  add <task>\n");
    print("  list\n");
    print("  remove <id>\n");
    print("  help\n");
    print("  exit\n");
}

static int starts_with(const char* text, const char* prefix)
{
    return strncmp(text, prefix, strlen(prefix)) == 0;
}

static int equals(const char* text, const char* word)
{
    int n = strlen(word);
    return strncmp(text, word, n) == 0 && (text[n] == 0 || text[n] == '\n');
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

int todo_run(void)
{
    print(MSG_WELCOME);
    print(MSG_HINT);

    while (1)
    {
        print(MSG_PROMPT);
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
                print(MSG_USAGE_ADD);
            else if (peachos_todo_add(task_text) < 0)
                print(MSG_ADD_FAIL);
            else
                print(MSG_ADD_OK);
            continue;
        }

        if (equals(line, "list"))
        {
            if (peachos_todo_list() < 0)
                print(MSG_LIST_FAIL);
            continue;
        }

        if (starts_with(line, "remove "))
        {
            int id = 0;
            if (parse_int(line + 7, &id) < 0)
                print(MSG_BAD_ID);
            else if (peachos_todo_remove(id) < 0)
                print(MSG_REMOVE_FAIL);
            else
                print(MSG_REMOVE_OK);
            continue;
        }

        if (equals(line, "help"))
        {
            print_help();
            continue;
        }

        if (equals(line, "exit"))
        {
            print(MSG_BYE);
            peachos_exit();
            return 0;
        }

        print(MSG_UNKNOWN);
    }

    return 0;
}
