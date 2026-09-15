#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/stat.h>

extern char end;
extern char __StackLimit;

void *_sbrk(ptrdiff_t increment)
{
  static uintptr_t heap_end;
  const uintptr_t heap_start = (uintptr_t)&end;
  const uintptr_t heap_limit = (uintptr_t)&__StackLimit;
  uintptr_t previous_end;

  if (heap_end == 0U)
  {
    heap_end = heap_start;
  }
  previous_end = heap_end;

  if (increment >= 0)
  {
    if (heap_end > heap_limit || (uintptr_t)increment > heap_limit - heap_end)
    {
      errno = ENOMEM;
      return (void *)-1;
    }
    heap_end += (uintptr_t)increment;
  }
  else
  {
    /* Avoid negating PTRDIFF_MIN and reject shrinking below the heap origin. */
    const uintptr_t decrement = (uintptr_t)(-(increment + 1)) + 1U;
    if (decrement > heap_end - heap_start)
    {
      errno = ENOMEM;
      return (void *)-1;
    }
    heap_end -= decrement;
  }
  return (void *)previous_end;
}

int _fstat(int file, struct stat *status)
{
  (void)file;
  if (status != 0)
  {
    status->st_mode = S_IFCHR;
  }
  return 0;
}

int _isatty(int file)
{
  (void)file;
  return 1;
}

int _close(int file)
{
  (void)file;
  return -1;
}

int _lseek(int file, int pointer, int direction)
{
  (void)file;
  (void)pointer;
  (void)direction;
  return 0;
}

int _read(int file, char *buffer, int length)
{
  (void)file;
  (void)buffer;
  (void)length;
  return 0;
}

int _write(int file, const char *buffer, int length)
{
  (void)file;
  (void)buffer;
  return length;
}

int _getpid(void)
{
  return 1;
}

int _kill(int process_id, int signal)
{
  (void)process_id;
  (void)signal;
  errno = EINVAL;
  return -1;
}

void _exit(int status)
{
  (void)status;
  while (1)
  {
  }
}
