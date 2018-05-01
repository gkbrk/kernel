#pragma once

typedef struct {
    uint32_t eax, ebx, ecx, edx, esi, edi, esp, ebp, eip, eflags, cr3;
} Registers;

typedef struct Task {
    Registers regs;
    struct Task *next;
} Task;

static Task *runningTask;
static Task tasks[10];

static void otherMain() {
    char prev_sec;
    while (true) {
        char sec = get_RTC_second();
        sec = (sec & 0x0F) + ((sec / 16) * 10);

        if (sec != prev_sec) {
            char num[5];
            itoa(sec, num);

            size_t old_col = terminal_column;
            size_t old_row = terminal_row;

            terminal_row = 0;
            terminal_column = VGA_WIDTH - 4;
            terminal_writestring(num);

            terminal_row = old_row;
            terminal_column = old_col;
        }

        prev_sec = sec;
        yield();
    }
}

void initTasking() {
    // Get EFLAGS and CR3
    asm volatile("movl %%cr3, %%eax; movl %%eax, %0;":"=m"(tasks[0].regs.cr3)::"%eax");
    asm volatile("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(tasks[0].regs.eflags)::"%eax");

    createTask(&tasks[1], otherMain, tasks[0].regs.eflags, (uint32_t*)tasks[0].regs.cr3);
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
    task->regs.esp = (uint32_t) kmalloc_forever(1) + 0x1000; // Not implemented here
    task->next = 0;
}

Task *getLastTask() {
    terminal_writestring("Looking for last task\n");

    for (int i = 0; i < 10; i++) {
        if (tasks[i].next == &tasks[0]) {
            terminal_writestring("Found it\n");
            char a[5];
            itoa(i, a);
            terminal_writestring(a);
            terminal_writestring("\n");
            return &tasks[i];
        }
    }
}

void spawnTask(void (*main)()) {
    Task *t;
    Task *l;

    for (int i = 0; i < 10; i++) {
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

void yield() {
    Task *last = runningTask;
    runningTask = runningTask->next;
    switchTask(&last->regs, &runningTask->regs);
}