#include "todo.h"
#include "peachos.h"

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;

    print("Todo List OS\n");
    print("Type 'help' for commands.\n");
    return todo_run();
}