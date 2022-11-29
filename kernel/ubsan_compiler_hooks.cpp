#include <kernel/drivers/BasicSerial.h>
#include <libk/Noncopyable.h>
#include <libk/assert.h>
#include <stdint.h>

// UBSan data structures

class SourceLocation {
  MAKE_NONCOPYABLE(SourceLocation);
  MAKE_NONMOVABLE(SourceLocation);

public:
  void WriteToSerial() const {
    basic_serial_printf("SourceLocation { file: %s, line: %d, column: %d }",
                        m_filename, m_line, m_column);
  }

private:
  const char *m_filename;
  uint32_t m_line;
  uint32_t m_column;
};

class TypeDescriptor {
  MAKE_NONCOPYABLE(TypeDescriptor);
  MAKE_NONMOVABLE(TypeDescriptor);

public:
  void WriteToSerial() const {
    basic_serial_printf("TypeDescriptor { name: %s }", m_name);
  }

private:
  uint16_t m_kind;
  uint16_t m_info;
  char m_name[1];
};

class OverflowData {
  MAKE_NONCOPYABLE(OverflowData);
  MAKE_NONMOVABLE(OverflowData);

public:
  void WriteToSerial() const {
    basic_serial_printf("OverflowData { type: ");
    m_type.WriteToSerial();
    basic_serial_printf(", location: ");
    m_location.WriteToSerial();
    basic_serial_printf(" }");
  }

private:
  SourceLocation m_location;
  TypeDescriptor m_type;
};

class ShiftOutOfBoundsData {
  MAKE_NONCOPYABLE(ShiftOutOfBoundsData);
  MAKE_NONMOVABLE(ShiftOutOfBoundsData);

public:
  void WriteToSerial() const {
    basic_serial_printf("ShiftOutOfBoundsData { location: ");
    m_location.WriteToSerial();
    basic_serial_printf(" }, lhs_type: ");
    m_lhs_type->WriteToSerial();
    basic_serial_printf(" }, rhs_type: ");
    m_rhs_type->WriteToSerial();
    basic_serial_printf(" }");
  }

private:
  SourceLocation m_location;
  TypeDescriptor *m_lhs_type;
  TypeDescriptor *m_rhs_type;
};

class InvalidValueData {
  MAKE_NONCOPYABLE(InvalidValueData);
  MAKE_NONMOVABLE(InvalidValueData);

public:
  void WriteToSerial() const {
    basic_serial_printf("InvalidValueData { location: ");
    m_location.WriteToSerial();
    basic_serial_printf(" }, type: ");
    m_type->WriteToSerial();
    basic_serial_printf(" }");
  }

private:
  SourceLocation m_location;
  TypeDescriptor *m_type;
};

class UnreachableData {
  MAKE_NONCOPYABLE(UnreachableData);
  MAKE_NONMOVABLE(UnreachableData);

public:
  void WriteToSerial() const {
    basic_serial_printf("UnreachableData { location: ");
    m_location.WriteToSerial();
    basic_serial_printf(" }");
  }

private:
  SourceLocation m_location;
};

extern "C" void __ubsan_handle_add_overflow(OverflowData *overflow,
                                            unsigned long lhs,
                                            unsigned long rhs) {
  basic_serial_printf("UBSAN: Addition overflow at ");
  overflow->WriteToSerial();
  basic_serial_write_char('\n');
  ASSERT_NOT_REACHED;
}

extern "C" void __ubsan_handle_sub_overflow(OverflowData *overflow,
                                            unsigned long lhs,
                                            unsigned long rhs) {
  basic_serial_printf("UBSAN: Subtraction overflow: ");
  overflow->WriteToSerial();
  basic_serial_write_char('\n');
  ASSERT_NOT_REACHED;
}

extern "C" void __ubsan_handle_mul_overflow(OverflowData *overflow,
                                            unsigned long lhs,
                                            unsigned long rhs) {
  basic_serial_printf("UBSAN: Multiplication overflow: ");
  overflow->WriteToSerial();
  basic_serial_write_char('\n');
  ASSERT_NOT_REACHED;
}

extern "C" void __ubsan_handle_divrem_overflow(OverflowData *overflow,
                                               unsigned long lhs,
                                               unsigned long rhs) {
  basic_serial_printf("UBSAN: Division overflow: ");
  overflow->WriteToSerial();
  basic_serial_write_char('\n');
  ASSERT_NOT_REACHED;
}

extern "C" void __ubsan_handle_negate_overflow(OverflowData *overflow,
                                               unsigned long old_val) {
  basic_serial_printf("UBSAN: Negation overflow: ");
  overflow->WriteToSerial();
  basic_serial_write_char('\n');
  ASSERT_NOT_REACHED;
}

extern "C" void __ubsan_handle_shift_out_of_bounds(ShiftOutOfBoundsData *data,
                                                   unsigned long lhs,
                                                   unsigned long rhs) {
  basic_serial_printf("UBSAN: Shift out of bounds: ");
  data->WriteToSerial();
  basic_serial_write_char('\n');
  ASSERT_NOT_REACHED;
}

extern "C" void __ubsan_handle_load_invalid_value(InvalidValueData *data,
                                                  unsigned long val) {
  basic_serial_printf("UBSAN: Load invalid value: ");
  data->WriteToSerial();
  basic_serial_write_char('\n');
  ASSERT_NOT_REACHED;
}

extern "C" void __ubsan_handle_missing_return(UnreachableData *data) {
  basic_serial_printf("UBSAN: Missing return: ");
  data->WriteToSerial();
  basic_serial_write_char('\n');
  ASSERT_NOT_REACHED;
}