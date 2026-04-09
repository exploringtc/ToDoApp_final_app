#ifndef TODO_DISK_H
#define TODO_DISK_H

int todo_cmd_save(const char* filename, const char* key);
int todo_cmd_load(const char* filename, const char* key);

#endif