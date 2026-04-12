#ifndef TODO_APP_H
#define TODO_APP_H

// Starts the interactive todo command loop.
int todo_run();

// Syscall-facing helpers used by the command layer.
int sys_add_task(const char* task);
int sys_list_tasks();
int sys_remove_task(int id);
int sys_save_tasks(const char* filename, const char* key);
int sys_load_tasks(const char* filename, const char* key);

// Command-specific persistence helpers.
int todo_cmd_save(const char* filename, const char* key);
int todo_cmd_load(const char* filename, const char* key);

// Symmetric XOR encryption/decryption helper for task payloads.
void xor_crypt_buffer(char* data, int len, const char* key);

#endif