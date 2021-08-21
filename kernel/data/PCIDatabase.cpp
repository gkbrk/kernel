#include <kernel/data/PCIDatabase.h>

// TODO: Generate this from a text file or load from disk during runtime

Vendor vendors[] = {{.vendorId = 0x8086, .name = "Intel Corporation"},
                    {.vendorId = 0x1af4, .name = "Red Hat, Inc."},
                    {.vendorId = 0x1234, .name = "QEMU/BOCHS"}};

Device devices[] = {
    {.vendorId = 0x8086, .deviceId = 0x1237, .name = "Pentium 440FX [Natoma]"},
    {.vendorId = 0x1af4, .deviceId = 0x1000, .name = "VirtIO network device"},
    {.vendorId = 0x1234, .deviceId = 0x1111, .name = "Virtual VGA adapter"},
    {.vendorId = 0x8086,
     .deviceId = 0x7113,
     .name = "82371AB/EB/MB PIIX4 ACPI"},
    {.vendorId = 0x8086,
     .deviceId = 0x7010,
     .name = "82371SB PIIX3 IDE [Natoma/Triton II]"},
    {.vendorId = 0x8086,
     .deviceId = 0x7000,
     .name = "82371SB PIIX3 ISA [Natoma/Triton II]"}};