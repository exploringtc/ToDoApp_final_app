#include "disk.h"
#include "todo.h"

int todo_cmd_save(const char* filename, const char* key)
{
    return sys_save_tasks(filename, key);
}

int todo_cmd_load(const char* filename, const char* key)
{
    return sys_load_tasks(filename, key);
}