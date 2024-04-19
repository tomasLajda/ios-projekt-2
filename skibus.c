#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>

int convert_to_int(char *str) {
  char *endptr;
  int value = strtol(str, &endptr, 10);
  if (*endptr != '\0') {
    fprintf(stderr, "Invalid number: %s\n", str);
    exit(1);
  }

  return value;
}

void check_int_range(int value, int min, int max) {
  if (value < min || value > max) {
    fprintf(stderr, "Invalid value: %d\n", value);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  if(argc != 6) {
    printf("Usage: %s L Z K TL TB\n", argv[0]);
    return 1;
  }

  int L = convert_to_int(argv[1]);
  check_int_range(L, 0, 19999);

  int Z = convert_to_int(argv[2]);
  check_int_range(Z, 1, 10);

  int K = convert_to_int(argv[3]);
  check_int_range(K, 10, 100);

  int TL = convert_to_int(argv[4]);
  check_int_range(TL, 0, 10000);

  int TB = convert_to_int(argv[5]);
  check_int_range(TB, 0, 1000);

  
  
  return 0;
}