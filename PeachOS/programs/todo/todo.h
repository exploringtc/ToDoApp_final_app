#ifndef TODO_APP_H
#define TODO_APP_H

/*
 * todo_run
 *  - Starts the user-facing REPL.
 *  - Reads commands from the terminal.
 *  - For add/list/remove, calls kernel via int 0x80 syscalls.
 *  - Returns when the user types "exit".
 */
int todo_run(void);

#endif
