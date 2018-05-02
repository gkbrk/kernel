#pragma once

typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} Registers;

typedef struct Task {
    Registers regs;
    struct Task *next;
} Task;

static Task *runningTask;
static Task tasks[20];

void task_empty() {
    while (true) {
        yield();
    }
}

void initTasking() {
    // Get EFLAGS and CR3
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(tasks[0].regs.cr3)::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(tasks[0].regs.eflags)::"%eax");

    createTask(&tasks[1], task_empty, tasks[0].regs.eflags, (uint32_t*)tasks[0].regs.cr3);
    tasks[0].next = &tasks[1];
    tasks[1].next = &tasks[0];

    runningTask = &tasks[0];
}

void createTask(Task *task, void (*main)(), uint32_t flags, uint32_t *pagedir) {
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.eflags = flags;
    task->regs.eip = (uint32_t) main;
    task->regs.cr3 = (uint32_t) pagedir;
    task->regs.esp = (uint32_t) kmalloc_forever(1024); // Not implemented here
    task->next = 0;
}

void spawnTask(void (*main)()) {
    Task *t;
    Task *l;

    for (int i = 0; i < sizeof(tasks) / sizeof(Task); i++) {
        if (tasks[i].next == &tasks[0]) {
            t = &tasks[i + 1];
            l = &tasks[i];
            break;
        }
    }

    createTask(t, main, tasks[0].regs.eflags, (uint32_t*)tasks[0].regs.cr3);
    terminal_writestring("Created task\n");

    l->next = t;
    t->next = &tasks[0];
}

void exitTask() {
    int id = 0;

    for (int i = 0; i < sizeof(tasks) / sizeof(Task); i++) {
        if (runningTask == &tasks[i]) {
            tasks[i - 1].next = tasks[i].next;
            return;
        }
    }
}

void yield() {
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}