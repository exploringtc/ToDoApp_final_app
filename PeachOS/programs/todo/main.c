/*
 * todo.elf entry point.
 * This file is intentionally tiny.
 * All command logic lives in todo.c (todo_run).
 */

#include "todo.h"
#include "peachos.h"

int main(void)
{
    return todo_run();
}
