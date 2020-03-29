#pragma once

struct IDT_entry {
  unsigned short int offset_lowerbits;
  unsigned short int selector;
  unsigned char zero;
  unsigned char type_attr;
  unsigned short int offset_higherbits;
};

extern struct IDT_entry IDT[256];

#ifdef __cplusplus
extern "C"
#endif

    void
    idt_init();
