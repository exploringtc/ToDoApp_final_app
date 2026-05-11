#include "src_todo.h"
#include "task/task.h"
#include "string/string.h"
#include "memory/memory.h"
#include "status.h"
#include "kernel.h"

#define TODO_MAX_TASKS 64
#define TODO_DESC_MAX 64

struct todo_task
{
    int id;
    int active;
    char description[TODO_DESC_MAX];
};



static struct todo_task todo_tasks[TODO_MAX_TASKS];
static int todo_next_id = 1;

static void todo_print_int(int value)
{
    char out[16];
    int pos = 15;
    int negative = 0;
    out[15] = 0;

    if (value == 0)
    {
        print("0");
        return;
    }

    if (value < 0)
    {
        negative = 1;
        value = -value;
    }

    while(value > 0 && pos > 0)
    {
        int digit = value % 10;
        out[--pos] = '0' + digit;
        value /= 10;
    }

    if (negative && pos > 0)
    {
        out[--pos] = '-';
    }

    print(&out[pos]);
}

static int todo_find_free_slot()
{
    for (int i = 0; i < TODO_MAX_TASKS; i++)
    {
        if (!todo_tasks[i].active)
        {
            return i;
        }
    }

    return -1;
}

static int todo_find_task_by_id(int id)
{
    for (int i = 0; i < TODO_MAX_TASKS; i++)
    {
        if (todo_tasks[i].active && todo_tasks[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

static int todo_task_count()
{
    int count = 0;
    for (int i = 0; i < TODO_MAX_TASKS; i++)
    {
        if (todo_tasks[i].active)
        {
            count++;
        }
    }

    return count;
}



void* isr80h_command10_todo_add(struct interrupt_frame* frame)
{
    void* task_ptr = task_get_stack_item(task_current(), 0);
    char task_desc[TODO_DESC_MAX];
    int res = copy_string_from_task(task_current(), task_ptr, task_desc, sizeof(task_desc));
    if (res < 0)
    {
        return ERROR(-EINVARG);
    }

    if (strnlen(task_desc, sizeof(task_desc)) <= 0)
    {
        return ERROR(-EINVARG);
    }

    int slot = todo_find_free_slot();
    if (slot < 0)
    {
        return ERROR(-ENOMEM);
    }

    todo_tasks[slot].id = todo_next_id++;
    todo_tasks[slot].active = 1;
    strncpy(todo_tasks[slot].description, task_desc, sizeof(todo_tasks[slot].description));

    return (void*) todo_tasks[slot].id;
}

void* isr80h_command11_todo_list(struct interrupt_frame* frame)
{
    (void) frame;

    int count = todo_task_count();
    if (count == 0)
    {
        print("No tasks available\n");
        return 0;
    }

    for (int i = 0; i < TODO_MAX_TASKS; i++)
    {
        if (!todo_tasks[i].active)
        {
            continue;
        }

        print("[");
        todo_print_int(todo_tasks[i].id);
        print("] ");
        print(todo_tasks[i].description);
        print("\n");
    }

    return 0;
}

void* isr80h_command12_todo_remove(struct interrupt_frame* frame)
{
    (void) frame;

    int id = (int) task_get_stack_item(task_current(), 0);
    int slot = todo_find_task_by_id(id);
    if (slot < 0)
    {
        return ERROR(-EINVARG);
    }

    memset(&todo_tasks[slot], 0, sizeof(struct todo_task));
    return 0;
}


