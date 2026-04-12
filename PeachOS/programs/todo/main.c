#include "todo.h"
#include "peachos.h"

// User-space program entrypoint: print banner then start command loop.
int main(int argc, char** argv)
{
    // Arguments are currently unused by this shell-style program.
    (void) argc;
    (void) argv;

    print("Todo List OS\n");
    print("Type 'help' for commands.\n");
    return todo_run();
}