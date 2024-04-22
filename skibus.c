#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "shared_memory.h"

int convert_to_int(char *str);
void check_int_range(int value, char* name, int min, int max);
void set_shared_memory(int L, int Z, int K);
void release_shared_memory(int Z);
void print_process(const char *format, ...);
void process_sleep(int max);
int generate_random_stop(int Z);

FILE *file; 
int *lineCount; 
int *ridersOnStop;
int *ridersLeft; 
int *busCapacity;
sem_t *mutex;
sem_t *busStops; 
sem_t *allAboard; 
sem_t *output;

int main(int argc, char *argv[]) {
  if(argc != 6) {
    printf("Usage: %s L Z K TL TB\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  int L = convert_to_int(argv[1]);
  check_int_range(L, "L", 0, 19999);

  int Z = convert_to_int(argv[2]);
  check_int_range(Z, "Z",1, 10);

  int K = convert_to_int(argv[3]);
  check_int_range(K, "K",10, 100);

  int TL = convert_to_int(argv[4]);
  check_int_range(TL, "TL", 0, 10000);

  int TB = convert_to_int(argv[5]);
  check_int_range(TB, "TB", 0, 1000);

  set_shared_memory(L, Z, K);

  if((file = fopen("proj2.out", "w")) == NULL) {
    fprintf(stderr, "Cannot open file\n");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();
  if (pid == 0) {
    print_process("BUS: started\n");
    process_sleep(TB);
    for(int i = 1; i <= Z+1; i++) {
      if(i == Z+1) {
        print_process("BUS: arrived to final\n");
        sem_post(&busStops[i - 1]);
        sem_wait(allAboard);
        if(*ridersLeft > 0) {
          i = 0;
          print_process("BUS: leaving final\n");
          process_sleep(TB);
        }

        if (*ridersLeft == 0) {
          print_process("BUS: leaving final\n");
          print_process("BUS: finish\n");
        }
        continue;
      }

      sem_wait(mutex);
      print_process("BUS: arrived to %d\n", i);

      if (ridersOnStop[i - 1] > 0 && *busCapacity > 0) {
        sem_post(&busStops[i - 1]);
        sem_wait(allAboard);
      }

      print_process("BUS: leaving %d\n", i);
      sem_post(mutex);
      process_sleep(TB);
    }

    fclose(file);
    exit(EXIT_SUCCESS);
  } else if (pid < 0) {
    fprintf(stderr, "Fork failed\n");
    release_shared_memory(Z);
    fclose(file);
    exit(EXIT_FAILURE);
  }

  for(int i = 1; i <= L; i++) {
    pid = fork();

    if (pid == 0) {
      print_process("L %d: started\n", i);
      process_sleep(TL);

      int riderStop = generate_random_stop(Z);
      print_process("L %d: arrived to %d\n", i, riderStop);

      sem_wait(mutex);
      ridersOnStop[riderStop - 1]++;
      sem_post(mutex);

      sem_wait(&busStops[riderStop - 1]);

      print_process("L %d: boarding\n", i);

      ridersOnStop[riderStop - 1]--;
      (*ridersLeft)--;
      (*busCapacity)--;

      if (*busCapacity == 0 || ridersOnStop[riderStop - 1] == 0) {
        sem_post(allAboard);
      } else {
        sem_post(&busStops[riderStop - 1]);
      }

      sem_wait(&busStops[Z]);
      print_process("L %d: going to ski\n", i);
      (*busCapacity)++;
      if(*busCapacity == K) {
        sem_post(allAboard);
      } else {
        sem_post(&busStops[Z]);
      }

      fclose(file);
      exit(EXIT_SUCCESS);
    } else if (pid < 0) {
      fprintf(stderr, "Fork failed\n");
      release_shared_memory(Z);
      fclose(file);
      exit(EXIT_FAILURE);
    }
  }

  while(wait(NULL) > 0);

  release_shared_memory(Z);
  fclose(file);

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

void set_shared_memory(int L,int Z, int K) {
  lineCount = create_shared_int(1);
  ridersLeft = create_shared_int(L);
  busCapacity = create_shared_int(K);
  ridersOnStop = create_shared_int_array(Z, 0);

  mutex = create_semaphore(1);
  allAboard = create_semaphore(0);
  output = create_semaphore(1);
  busStops = create_semaphore_array(Z + 1, 0);
}

void release_shared_memory(int Z) {
  release_shared_int(lineCount);
  release_shared_int(ridersLeft);
  release_shared_int(busCapacity);
  release_shared_int_array(ridersOnStop, Z);

  release_semaphore(mutex);
  release_semaphore(allAboard);
  release_semaphore(output);
  release_semaphore_array(busStops, Z + 1);
}

void print_process(const char *format, ...) {
  sem_wait(output);
  va_list args;
  va_start(args, format);
  fprintf(file, "%d: ", (*lineCount)++);
  vfprintf(file, format, args);
  fflush(file);
  va_end(args);
  sem_post(output);
}

void process_sleep(int max) {
  srand(time(NULL) ^ getpid());
  int time = rand() % (max + 1);
  usleep(time);
}

int generate_random_stop(int Z) {
  srand(time(NULL) ^ getpid());
  return rand() % Z + 1;
}