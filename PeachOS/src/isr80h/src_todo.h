#ifndef ISR80H_SRC_TODO_H
#define ISR80H_SRC_TODO_H

struct interrupt_frame;

void* isr80h_command10_todo_add(struct interrupt_frame* frame);
void* isr80h_command11_todo_list(struct interrupt_frame* frame);
void* isr80h_command12_todo_remove(struct interrupt_frame* frame);

#endif