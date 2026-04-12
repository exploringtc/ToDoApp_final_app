#ifndef TODO_DISK_H
#define TODO_DISK_H

// Saves the current in-memory task list to disk using a caller-provided key.
int todo_cmd_save(const char* filename, const char* key);
// Loads tasks from disk, decrypts with key, and repopulates in-memory state.
int todo_cmd_load(const char* filename, const char* key);

#endif