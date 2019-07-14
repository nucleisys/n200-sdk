/* See LICENSE of license details. */

#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "soc/drivers/soc.h"

ssize_t _write(int fd, const void* ptr, size_t len)
{
  const uint8_t * current = (const char *)ptr;

  if (isatty(fd)) {
    for (size_t jj = 0; jj < len; jj++) {
      while (UART0_REG(UART_REG_TXFIFO) & 0x80000000) ;
      UART0_REG(UART_REG_TXFIFO) = current[jj];

      if (current[jj] == '\n') {
        while (UART0_REG(UART_REG_TXFIFO) & 0x80000000) ;
        UART0_REG(UART_REG_TXFIFO) = '\r';
      }
    }
    return len;
  }

  return -1;
}
