#include <stdio.h>
#include <stdlib.h>

struct a {
  unsigned short num1;
  unsigned short num2;
};

struct b {
  unsigned char c1, c2, c3, c4;
};

int main() {
  struct a* test = (struct a*)malloc(sizeof(struct a));
  test->num1 = 0xFF00;
  test->num2 = 0xabcd;

  struct b other = *(struct b*)test;
  printf("%.2x, %.2x, %.2x, %.2x\n", other.c1, other.c2, other.c3, other.c4);

  return 0;
}
