#include "idt.h"
#include <kernel/drivers/io.h>
#include <libk/debug.h>

struct IDT_entry IDT[256];

extern "C" void interrupt_enable() { asm volatile("sti"); }

extern "C" void interrupt_disable() { asm volatile("cli"); }

extern "C" void hlt() { asm volatile("hlt"); }

void init_timer(uint32_t freq) {
  dbg() << "Setting timer freq to " << freq;
  uint32_t divisor = 1193180 / freq;
  outb(0x43, 0x36);
  uint8_t l = (uint8_t)(divisor & 0xFF);
  uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);
  outb(0x40, l);
  outb(0x40, h);
}

void set_exception_handler(size_t interrupt_index, void (*handler)()) {
  IDT[interrupt_index].offset_lowerbits = (unsigned long)handler & 0xffff;
  IDT[interrupt_index].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[interrupt_index].zero = 0;
  IDT[interrupt_index].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[interrupt_index].offset_higherbits =
      ((unsigned long)handler & 0xffff0000) >> 16;
}

extern "C" void idt_init() {
  extern int load_idt(void *);
  extern int irq0();
  extern int irq1();
  extern int irq2();
  extern int irq3();
  extern int irq4();
  extern int irq5();
  extern int irq6();
  extern int irq7();
  extern int irq8();
  extern int irq9();
  extern int irq10();
  extern int irq11();
  extern int irq12();
  extern int irq13();
  extern int irq14();
  extern int irq15();

  unsigned long irq0_address;
  unsigned long irq1_address;
  unsigned long irq2_address;
  unsigned long irq3_address;
  unsigned long irq4_address;
  unsigned long irq5_address;
  unsigned long irq6_address;
  unsigned long irq7_address;
  unsigned long irq8_address;
  unsigned long irq9_address;
  unsigned long irq10_address;
  unsigned long irq11_address;
  unsigned long irq12_address;
  unsigned long irq13_address;
  unsigned long irq14_address;
  unsigned long irq15_address;
  unsigned long idt_address;
  unsigned long idt_ptr[2];

  /* remapping the PIC */
  outb(0x20, 0x11);
  outb(0xA0, 0x11);
  outb(0x21, 0x20);
  outb(0xA1, 40);
  outb(0x21, 0x04);
  outb(0xA1, 0x02);
  outb(0x21, 0x01);
  outb(0xA1, 0x01);
  outb(0x21, 0x0);
  outb(0xA1, 0x0);

  set_exception_handler(0, []() {
    basic_serial_printf("Divide by zero!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(1, []() {
    basic_serial_printf("Debug!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(2, []() {
    basic_serial_printf("Non Maskable Interrupt!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(3, []() {
    basic_serial_printf("Breakpoint!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(4, []() {
    basic_serial_printf("Overflow!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(5, []() {
    basic_serial_printf("Bound Range Exceeded!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(6, []() {
    basic_serial_printf("Invalid Opcode!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(7, []() {
    basic_serial_printf("Device Not Available!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(8, []() {
    basic_serial_printf("Double fault!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(9, []() {
    basic_serial_printf("Coprocessor Segment Overrun!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(10, []() {
    basic_serial_printf("Invalid TSS!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(11, []() {
    basic_serial_printf("Segment Not Present!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(12, []() {
    basic_serial_printf("Stack-Segment Fault!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(13, []() {
    basic_serial_printf("General Protection Fault!\n");
    basic_serial_printf("Did you access a null pointer?\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(14, []() {
    basic_serial_printf("Page Fault!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(15, []() {
    basic_serial_printf("Reserved!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(16, []() {
    basic_serial_printf("x87 Floating-Point Exception!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(17, []() {
    basic_serial_printf("Alignment Check!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(18, []() {
    basic_serial_printf("Machine Check!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(19, []() {
    basic_serial_printf("SIMD Floating-Point Exception!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(20, []() {
    basic_serial_printf("Virtualization Exception!\n");
    ASSERT_NOT_REACHED;
  });
  set_exception_handler(21, []() {
    basic_serial_printf("Control Protection Exception!\n");
    ASSERT_NOT_REACHED;
  });

  irq0_address = (unsigned long)irq0;
  IDT[32].offset_lowerbits = irq0_address & 0xffff;
  IDT[32].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[32].zero = 0;
  IDT[32].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[32].offset_higherbits = (irq0_address & 0xffff0000) >> 16;

  irq1_address = (unsigned long)irq1;
  IDT[33].offset_lowerbits = irq1_address & 0xffff;
  IDT[33].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[33].zero = 0;
  IDT[33].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[33].offset_higherbits = (irq1_address & 0xffff0000) >> 16;

  irq2_address = (unsigned long)irq2;
  IDT[34].offset_lowerbits = irq2_address & 0xffff;
  IDT[34].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[34].zero = 0;
  IDT[34].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[34].offset_higherbits = (irq2_address & 0xffff0000) >> 16;

  irq3_address = (unsigned long)irq3;
  IDT[35].offset_lowerbits = irq3_address & 0xffff;
  IDT[35].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[35].zero = 0;
  IDT[35].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[35].offset_higherbits = (irq3_address & 0xffff0000) >> 16;

  irq4_address = (unsigned long)irq4;
  IDT[36].offset_lowerbits = irq4_address & 0xffff;
  IDT[36].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[36].zero = 0;
  IDT[36].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[36].offset_higherbits = (irq4_address & 0xffff0000) >> 16;

  irq5_address = (unsigned long)irq5;
  IDT[37].offset_lowerbits = irq5_address & 0xffff;
  IDT[37].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[37].zero = 0;
  IDT[37].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[37].offset_higherbits = (irq5_address & 0xffff0000) >> 16;

  irq6_address = (unsigned long)irq6;
  IDT[38].offset_lowerbits = irq6_address & 0xffff;
  IDT[38].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[38].zero = 0;
  IDT[38].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[38].offset_higherbits = (irq6_address & 0xffff0000) >> 16;

  irq7_address = (unsigned long)irq7;
  IDT[39].offset_lowerbits = irq7_address & 0xffff;
  IDT[39].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[39].zero = 0;
  IDT[39].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[39].offset_higherbits = (irq7_address & 0xffff0000) >> 16;

  irq8_address = (unsigned long)irq8;
  IDT[40].offset_lowerbits = irq8_address & 0xffff;
  IDT[40].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[40].zero = 0;
  IDT[40].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[40].offset_higherbits = (irq8_address & 0xffff0000) >> 16;

  irq9_address = (unsigned long)irq9;
  IDT[41].offset_lowerbits = irq9_address & 0xffff;
  IDT[41].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[41].zero = 0;
  IDT[41].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[41].offset_higherbits = (irq9_address & 0xffff0000) >> 16;

  irq10_address = (unsigned long)irq10;
  IDT[42].offset_lowerbits = irq10_address & 0xffff;
  IDT[42].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[42].zero = 0;
  IDT[42].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[42].offset_higherbits = (irq10_address & 0xffff0000) >> 16;

  irq11_address = (unsigned long)irq11;
  IDT[43].offset_lowerbits = irq11_address & 0xffff;
  IDT[43].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[43].zero = 0;
  IDT[43].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[43].offset_higherbits = (irq11_address & 0xffff0000) >> 16;

  irq12_address = (unsigned long)irq12;
  IDT[44].offset_lowerbits = irq12_address & 0xffff;
  IDT[44].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[44].zero = 0;
  IDT[44].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[44].offset_higherbits = (irq12_address & 0xffff0000) >> 16;

  irq13_address = (unsigned long)irq13;
  IDT[45].offset_lowerbits = irq13_address & 0xffff;
  IDT[45].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[45].zero = 0;
  IDT[45].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[45].offset_higherbits = (irq13_address & 0xffff0000) >> 16;

  irq14_address = (unsigned long)irq14;
  IDT[46].offset_lowerbits = irq14_address & 0xffff;
  IDT[46].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[46].zero = 0;
  IDT[46].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[46].offset_higherbits = (irq14_address & 0xffff0000) >> 16;

  irq15_address = (unsigned long)irq15;
  IDT[47].offset_lowerbits = irq15_address & 0xffff;
  IDT[47].selector = 0x08; /* KERNEL_CODE_SEGMENT_OFFSET */
  IDT[47].zero = 0;
  IDT[47].type_attr = 0x8e; /* INTERRUPT_GATE */
  IDT[47].offset_higherbits = (irq15_address & 0xffff0000) >> 16;

  /* fill the IDT descriptor */
  idt_address = (unsigned long)IDT;
  idt_ptr[0] =
      (sizeof(struct IDT_entry) * 256) + ((idt_address & 0xffff) << 16);
  idt_ptr[1] = idt_address >> 16;

  load_idt(idt_ptr);
}
