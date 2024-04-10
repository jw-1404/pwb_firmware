#include <stdio.h>
#include <zephyr/kernel.h>

int main(void) {
  int i = 0;
  while (1) {
    printf("%d: demo/hello_world app on %s\n", i++, CONFIG_BOARD);
    k_msleep(2000);
  }
  return 0;
}
