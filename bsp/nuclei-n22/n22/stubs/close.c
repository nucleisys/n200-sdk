/* See LICENSE of license details. */

#include <errno.h>
#include "n22/stubs/stub.h"

int _close(int fd)
{
  return _stub(EBADF);
}
