#include "todo.h"
#include "peachos.h"

// Forwards add requests to the PeachOS userspace API.
int sys_add_task(const char* task)
{
    return peachos_todo_add(task);
}

// Requests kernel-side listing of current tasks.
int sys_list_tasks()
{
    return peachos_todo_list();
}

// Removes a task by numeric id through the PeachOS API.
int sys_remove_task(int id)
{
    return peachos_todo_remove(id);
}

// Persists tasks to a file, optionally encrypted with the provided key.
int sys_save_tasks(const char* filename, const char* key)
{
    return peachos_todo_save(filename, key);
}

// Loads task data from a file and decrypts/validates with the provided key.
int sys_load_tasks(const char* filename, const char* key)
{
    return peachos_todo_load(filename, key);
}