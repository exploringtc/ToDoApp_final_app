/*
 * PeachOS 32-Bit Kernel project
 */

#ifndef ISR80H_TODO_H
#define ISR80H_TODO_H

struct interrupt_frame;

void* isr80h_command10_todo_add(struct interrupt_frame* frame);
void* isr80h_command11_todo_list(struct interrupt_frame* frame);
void* isr80h_command12_todo_remove(struct interrupt_frame* frame);
void* isr80h_command13_todo_save(struct interrupt_frame* frame);
void* isr80h_command14_todo_load(struct interrupt_frame* frame);

#endif