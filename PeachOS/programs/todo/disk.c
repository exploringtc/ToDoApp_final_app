#include "disk.h"
#include "todo.h"

// Thin command-layer wrapper that delegates persistence to the syscall facade.
int todo_cmd_save(const char* filename, const char* key)
{
    return sys_save_tasks(filename, key);
}

// Thin command-layer wrapper that delegates restore logic to the syscall facade.
int todo_cmd_load(const char* filename, const char* key)
{
    return sys_load_tasks(filename, key);
}