#include "stdio_impl.h"
//include <sys/uio.h>
#include <cosiolib/system.h>

size_t __stdio_write(FILE *f, const unsigned char *buf, size_t len)
{
   print_str((char*)(f->wbase), f->wpos-f->wbase);
   print_str((void*)buf, len);
   return f->wpos-f->wbase + len;
}
