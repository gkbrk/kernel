#include "string.h"
#include "printf.h"

size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len]) {
    len++;
  }
  return len;
}

int strcmp(const char *a, const char *b) {
  for (size_t i = 0; true; i++) {
    unsigned char ac = (unsigned char)a[i];
    unsigned char bc = (unsigned char)b[i];
    if (ac == '\0' && bc == '\0')
      return 0;
    if (ac < bc)
      return -1;
    if (ac > bc)
      return 1;
  }
}

bool streq(const char *str1, const char *str2) {
  return strcmp(str1, str2) == 0;
}

char *strcpy(char *dest, const char *src) {
  char *save = dest;
  while ((*dest++ = *src++))
    ;
  return save;
}

char *strcat(char *dest, const char *src) {
  strcpy(dest + strlen(dest), src);
  return dest;
}

char *strsep(char **stringp, const char *delim) {
  char *s;
  const char *spanp;
  int c, sc;
  char *tok;
  if ((s = *stringp) == NULL)
    return (NULL);
  for (tok = s;;) {
    c = *s++;
    spanp = delim;
    do {
      if ((sc = *spanp++) == c) {
        if (c == 0)
          s = NULL;
        else
          s[-1] = 0;
        *stringp = s;
        return (tok);
      }
    } while (sc != 0);
  }
}

char *strdup(const char *str) {
  size_t siz;
  char *copy;

  siz = strlen(str) + 1;
  if ((copy = (char *)kmalloc(siz)) == NULL)
    return (NULL);
  (void)memcpy(copy, str, siz);
  return (copy);
}

void sprintf(char *buf, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsprintf(buf, NULL, fmt, ap);
  va_end(ap);
}

void *memset(void *bufptr, int value, size_t size) {
  unsigned char *buf = (unsigned char *)bufptr;
  for (size_t i = 0; i < size; i++)
    buf[i] = (unsigned char)value;
  return bufptr;
}

void *memcpy(void *dstptr, const void *srcptr, size_t size) {
  unsigned char *dst = (unsigned char *)dstptr;
  const unsigned char *src = (const unsigned char *)srcptr;
  for (size_t i = 0; i < size; i++) {
    dst[i] = src[i];
  }
  return dstptr;
}

/* reverse:  reverse string s in place */
static void reverse(char s[]) {
  int i, j;
  char c;

  for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

/* itoa:  convert n to characters in s */
void itoa(int n, char s[]) {
  int i, sign;

  if ((sign = n) < 0) /* record sign */
  {
    n = -n; /* make n positive */
  }

  i = 0;
  do {                     /* generate digits in reverse order */
    s[i++] = n % 10 + '0'; /* get next digit */
  } while ((n /= 10) > 0); /* delete it */

  if (sign < 0) {
    s[i++] = '-';
  }

  s[i] = '\0';

  reverse(s);
}

void u64toa(uint64_t n, char s[]) {
  uint64_t i;

  i = 0;
  do {                     /* generate digits in reverse order */
    s[i++] = n % 10 + '0'; /* get next digit */
  } while ((n /= 10) > 0); /* delete it */

  s[i] = '\0';

  reverse(s);
}

bool isdigit(char c) { return c >= '0' && c <= '9'; }

size_t atoi(const char *c) {
  size_t value = 0;
  int sign = 1;
  if (*c == '+' || *c == '-') {
    if (*c == '-')
      sign = -1;
    c++;
  }
  while (isdigit(*c)) {
    value *= 10;
    value += (int)(*c - '0');
    c++;
  }
  return (value * sign);
}
