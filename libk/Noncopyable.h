#ifndef CARDINAL_LIBK_NONCOPYABLE_H_
#define CARDINAL_LIBK_NONCOPYABLE_H_

// Small macros that disable automatic copy and move contructors.

#define MAKE_NONCOPYABLE(c)                                                    \
private:                                                                       \
  c(const c &) = delete;                                                       \
  c &operator=(const c &) = delete

#define MAKE_NONMOVABLE(c)                                                     \
private:                                                                       \
  c(c &&) = delete;                                                            \
  c &operator=(c &&) = delete

#endif