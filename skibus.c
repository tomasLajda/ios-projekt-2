#include <stdio.h>
#include <stdarg.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/types.h>

int convert_to_int(char *str);
void check_int_range(int value, char* name, int min, int max);
sem_t *create_semaphore(int value);
void release_semaphore(sem_t *sem);
int *create_shared_variable(int value);
void release_shared_variable(int *var);
void set_shared_memory(char **argv);
void release_shared_memory();
void print_process(const char *format, ...);

FILE *file;
int *lineCount;
int *riders;
int *stopsCount;
sem_t *mutex;
sem_t *multiplex;
sem_t *bus;
sem_t *allAboard;
int busWaitTime;
int riderWaitTime;


int main(int argc, char *argv[]) {
  if(argc != 6) {
    printf("Usage: %s L Z K TL TB\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  if((file = fopen("proj2.out", "w")) == NULL) {
    fprintf(stderr, "Cannot open file\n");
    exit(EXIT_FAILURE);
  }

  set_shared_memory(argv);
  printf("%d\n", *riders);

  pid_t pid = fork();
  if (pid == 0) { // Child process
      sem_wait(mutex);
      if (*riders > 0) {
          sem_post(bus); // and pass the mutex
          printf("Bus arrived\n");
          sem_wait(allAboard); // and get the mutex back
      }
      sem_post(mutex);
      exit(EXIT_SUCCESS);
  } else if (pid < 0) {
      fprintf(stderr, "Fork failed\n");
      exit(EXIT_FAILURE);
  }

  while(wait(NULL) > 0);

  release_shared_memory();

  exit(EXIT_SUCCESS);
}

int convert_to_int(char *str) {
  char *endptr;
  int value = strtol(str, &endptr, 10);
  if (*endptr != '\0') {
    fprintf(stderr, "Invalid input value: %s\n", str);
    exit(EXIT_FAILURE);
  }

  return value;
}

void check_int_range(int value, char* name, int min, int max) {
  if (value < min || value > max) {
    fprintf(stderr, "%s value: %d out of range <%d; %d>\n", name, value, min, max);
    exit(EXIT_FAILURE);
  }
}

sem_t *create_semaphore(int value) {
  sem_t *sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  if(sem == MAP_FAILED) {
    fprintf(stderr, "Cannot create semaphore\n");
    exit(EXIT_FAILURE);
  }

  if(sem_init(sem, 1, value) == -1) {
    fprintf(stderr, "Cannot initialize semaphore\n");
    exit(EXIT_FAILURE);
  }

  return sem;
}

void release_semaphore(sem_t *sem) {
  if (sem_destroy(sem) == -1) {
    fprintf(stderr, "Cannot destroy semaphore\n");
    exit(EXIT_FAILURE);
  }

  if (munmap(sem, sizeof(sem_t)) == -1) {
    fprintf(stderr, "Cannot unmap semaphore\n");
    exit(EXIT_FAILURE);
  }
}

int *create_shared_variable(int value) {
  int *var = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  if(var == MAP_FAILED) {
    fprintf(stderr, "Cannot create shared variable\n");
    exit(EXIT_FAILURE);
  }

  *var = value;
  return var;
}

void release_shared_variable(int *var) {
  if (munmap(var, sizeof(int)) == -1) {
    fprintf(stderr, "Cannot unmap shared variable\n");
    exit(EXIT_FAILURE);
  }
}

void set_shared_memory(char **argv) {
  int L = convert_to_int(argv[1]);
  check_int_range(L, "L", 0, 19999);
  riders = create_shared_variable(0);

  int Z = convert_to_int(argv[2]);
  check_int_range(Z, "Z",1, 10);
  stopCount = create_shared_variable(Z);

  int K = convert_to_int(argv[3]);
  check_int_range(K, "K",10, 100);
  multiplex = create_semaphore(K);

  int TL = convert_to_int(argv[4]);
  check_int_range(TL, "TL", 0, 10000);
  riderWaitTime = TL;

  int TB = convert_to_int(argv[5]);
  check_int_range(TB, "TB", 0, 1000);
  busWaitTime = TB;

  lineCount = create_shared_variable(1);
  mutex = create_semaphore(1);
  bus = create_semaphore(1);
  allAboard = create_semaphore(0);
}

void release_shared_memory() {
  release_shared_variable(riders);
  release_shared_variable(stopsCount);
  release_shared_variable(lineCount);

  release_semaphore(mutex);
  release_semaphore(multiplex);
  release_semaphore(bus);
  release_semaphore(allAboard);
}

void print_process(const char *format, ...) {
  va_list args;
  va_start(args, format);
  fprintf(file, "%d: ", (*lineCount)++);
  vfprintf(file, format, args);
  fflush(file);
  va_end(args);
}
