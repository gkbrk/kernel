#pragma once

size_t strlen(const char* str) {
  size_t len = 0;
  while (str[len]) {
    len++;
  }
  return len;
}

int strcmp(const char* a, const char* b) {
  for ( size_t i = 0; true; i++ ) {
    unsigned char ac = (unsigned char) a[i];
    unsigned char bc = (unsigned char) b[i];
    if ( ac == '\0' && bc == '\0' )
      return 0;
    if ( ac < bc )
      return -1;
    if ( ac > bc )
      return 1;
  }
}

void* memset(void* bufptr, int value, size_t size) {
  unsigned char* buf = (unsigned char*) bufptr;
  for (size_t i = 0; i < size; i++)
    buf[i] = (unsigned char) value;
  return bufptr;
}

void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size) {
  unsigned char* dst = (unsigned char*) dstptr;
  const unsigned char* src = (const unsigned char*) srcptr;
  for (size_t i = 0; i < size; i++) {
    dst[i] = src[i];
  }
  return dstptr;
}

/* reverse:  reverse string s in place */
static void reverse( char s[] ) {
  int i, j ;
  char c ;

  for ( i = 0, j = strlen(s)-1 ; i < j ; i++, j-- )
  {
    c = s[i] ;
    s[i] = s[j] ;
    s[j] = c ;
  }
}

/* itoa:  convert n to characters in s */
void itoa( int n, char s[] ){
  int i, sign ;

  if ( (sign = n) < 0 )  /* record sign */
  {
    n = -n;          /* make n positive */
  }

  i = 0;
  do
  {       /* generate digits in reverse order */
    s[i++] = n % 10 + '0';   /* get next digit */
  } while ((n /= 10) > 0) ;     /* delete it */

  if (sign < 0 )
  {
    s[i++] = '-';
  }

  s[i] = '\0';

  reverse( s ) ;
}