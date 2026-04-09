#include "todo.h"
#include "peachos.h"

int sys_add_task(const char* task)
{
    return peachos_todo_add(task);
}

int sys_list_tasks()
{
    return peachos_todo_list();
}

int sys_remove_task(int id)
{
    return peachos_todo_remove(id);
}

int sys_save_tasks(const char* filename, const char* key)
{
    return peachos_todo_save(filename, key);
}

int sys_load_tasks(const char* filename, const char* key)
{
    return peachos_todo_load(filename, key);
}