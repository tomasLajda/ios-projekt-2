/*
  * Project 2 - IOS
  * Author: Tomáš Lajda <xlajda00>
  * Date: 2024-04-24
  * Description: Program simulating the operation of a ski bus.
  * The program creates a bus process and L rider processes.
  * The bus process is responsible for driving the bus between stops and boarding riders.
  * The rider processes are responsible for arriving at the bus stop and boarding the bus.
  * The program uses shared memory and semaphores for synchronization.
*/

// headers
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>

// custom headers
#include "shared_memory.h"

// function declarations
int convert_to_int(char *str);
void check_int_range(int value, char* name, int min, int max);
void set_shared_memory(int L, int Z, int K);
void release_shared_memory(int Z);
void print_process(const char *format, ...);
void process_sleep(int max);
int generate_random_stop(int Z);
void fork_failed();
void bus(int Z,int K, int TB);
void rider(int idL, int TL, int Z, int K);

// global variables
FILE *file; 
int *lineCount; 
int *ridersOnStop;
int *ridersLeft; 
int *busCapacity;
sem_t *mutex;
sem_t *busStops; 
sem_t *busLeave; 
sem_t *outputMutex;


/**
 *
 * The skibus program simulates a skibus system where buses transport riders to a ski resort.
 * The program takes command line arguments for the number of buses, riders, and various time limits.
 * It creates child processes to simulate the buses and riders, and uses shared memory to coordinate their actions.
 * The program outputs the simulation results to a file named "proj2.out".
 *
 * @param argc The number of command line arguments.
 * @param argv An array of strings containing the command line arguments.
 *             The arguments should be in the following order: L Z K TL TB.
 *             L - The number of riders.
 *             Z - The number of buses.
 *             K - The maximum number of riders that can fit on a bus.
 *             TL - The maximum time a rider spends skiing.
 *             TB - The maximum time a bus spends driving.
 *
 * @return 0 on success, non-zero value on failure.
 */
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

  if((file = fopen("proj2.out", "w")) == NULL) {
    fprintf(stderr, "Cannot open file\n");
    exit(EXIT_FAILURE);
  }

  set_shared_memory(L, Z, K);

  // create bus
  pid_t pid = fork();
  if (pid == 0) {
    bus(Z, K, TB);
    fclose(file);
    exit(EXIT_SUCCESS);
  } else if (pid < 0) {
    fork_failed();
  }

  // create riders
  for(int idL = 1; idL <= L; idL++) {
    pid = fork();

    if (pid == 0) {
      rider(idL, TL, Z, K);
      fclose(file);
      exit(EXIT_SUCCESS);
    } else if (pid < 0) {
      fork_failed();
    }
  }

  while(wait(NULL) > 0);

  release_shared_memory(Z);
  fclose(file);

  return 0;
}

/**
 * Converts a string to an integer.
 *
 * @param str The string to be converted.
 * @return The converted integer value.
 * @throws If the input value is invalid, an error message is printed and the program exits.
 */
int convert_to_int(char *str) {
  char *endptr;
  int value = strtol(str, &endptr, 10);

  if (*endptr != '\0') {
    fprintf(stderr, "Invalid input value: %s\n", str);
    exit(EXIT_FAILURE);
  }

  return value;
}

/**
 * Checks if an integer value is within a specified range.
 *
 * @param value The integer value to be checked.
 * @param name The name of the value being checked.
 * @param min The minimum allowed value.
 * @param max The maximum allowed value.
 *
 * @return None.
 *
 * @trows If the input value is out of range, an error message is printed and the program exits.
 */
void check_int_range(int value, char* name, int min, int max) {
  if (value < min || value > max) {
    fprintf(stderr, "%s value: %d out of range <%d; %d>\n", name, value, min, max);
    exit(EXIT_FAILURE);
  }
}

/**
 * Sets up the shared memory and semaphores for the skibus program.
 *
 * @param L The number of riders waiting at the bus stop.
 * @param Z The number of bus stops.
 * @param K The capacity of the bus.
 */
void set_shared_memory(int L, int Z, int K) {
  // create shared int variables
  lineCount = create_shared_int(1);
  ridersLeft = create_shared_int(L);
  busCapacity = create_shared_int(K);
  ridersOnStop = create_shared_int_array(Z, 0);

  // create semaphores
  mutex = create_semaphore(1);
  busLeave = create_semaphore(0);
  outputMutex = create_semaphore(1);
  // creates an array of semaphores for each bus stop and final stop
  busStops = create_semaphore_array(Z + 1, 0);
}

/**
 * Releases the shared memory and semaphores used in the program.
 * 
 * @param Z The number of bus stops.
 */
void release_shared_memory(int Z) {
  // release shared int variables
  release_shared_int(lineCount);
  release_shared_int(ridersLeft);
  release_shared_int(busCapacity);
  release_shared_int_array(ridersOnStop, Z);

  // release semaphores
  release_semaphore(mutex);
  release_semaphore(busLeave);
  release_semaphore(outputMutex);
  release_semaphore_array(busStops, Z + 1);
}

/**
 * Prints a formatted message to the output file, with a line count prefix.
 *
 * @param format The format string for the message.
 * @param ... Additional arguments to be formatted according to the format string.
 */
void print_process(const char *format, ...) {
  sem_wait(outputMutex);
  va_list args;
  va_start(args, format);
  fprintf(file, "%d: ", (*lineCount)++);
  vfprintf(file, format, args);
  fflush(file);
  va_end(args);
  sem_post(outputMutex);
}

/**
 * Generates a random sleep time within the specified range and sleeps for that duration.
 *
 * @param max The maximum sleep time in milliseconds.
 */
void process_sleep(int max) {
  srand(time(NULL) ^ getpid());
  int time = rand() % (max + 1);
  usleep(time);
}

/**
 * Generates a random stop number between 1 and Z (inclusive).
 *
 * @param Z The maximum stop number.
 * @return The randomly generated stop number.
 */
int generate_random_stop(int Z) {
  srand(time(NULL) ^ getpid());
  return rand() % Z + 1;
}

/**
 * This function is called when the fork operation fails.
 * It prints an error message to the standard error stream,
 * releases shared memory, closes a file, and sends a SIGKILL signal to all processes in the process group.
 */
void fork_failed() {
  fprintf(stderr, "Fork failed\n");
  release_shared_memory(1);
  fclose(file);
  kill(0, SIGKILL);
}

/**
 * Simulates the behavior of a bus traveling between stops.
 *
 * @param Z The total number of stops.
 * @param K The maximum capacity of the bus.
 * @param TB The time it takes for the bus to travel between stops.
 */
void bus(int Z, int K, int TB) {
  print_process("BUS: started\n");
  process_sleep(TB);
  for(int idZ = 1; idZ <= Z + 1; idZ++) {
    // final stop
    if(idZ == Z + 1) {
      print_process("BUS: arrived to final\n");

      if(*ridersLeft > 0 || *busCapacity < K) {
        if(*busCapacity != K) {
          sem_post(&busStops[idZ - 1]);
          sem_wait(busLeave);
        }

        print_process("BUS: leaving final\n");

        if(*ridersLeft > 0) {
          idZ = 0;
          process_sleep(TB);
        }
      }

      if (*ridersLeft == 0) {
        print_process("BUS: finish\n");
      }
    } else {
      sem_wait(mutex);
      print_process("BUS: arrived to %d\n", idZ);

      if (ridersOnStop[idZ - 1] > 0 && *busCapacity > 0) {
        sem_post(&busStops[idZ - 1]);
        sem_wait(busLeave);
      }

      print_process("BUS: leaving %d\n", idZ);
      sem_post(mutex);
      process_sleep(TB);
    }
  }
}

/**
 * 
 * Behavior of a rider in a ski bus system.
 * The rider arrives at a bus stop, waits for the bus to arrive, boards the bus,
 * travels to the ski area, and then leaves the bus.
 *
 * @param idL The ID of the rider.
 * @param TL The time the rider spends at the bus stop before boarding the bus.
 * @param Z The bus stop where the rider arrives.
 * @param K The maximum capacity of the bus.
 */
void rider(int idL, int TL, int Z, int K) {
  print_process("L %d: started\n", idL);
  process_sleep(TL);

  int idZ = generate_random_stop(Z);
  print_process("L %d: arrived to %d\n", idL, idZ);

  sem_wait(mutex);
  ridersOnStop[idZ - 1]++;
  sem_post(mutex);

  sem_wait(&busStops[idZ - 1]);

  print_process("L %d: boarding\n", idL);
  ridersOnStop[idZ - 1]--;
  (*ridersLeft)--;
  (*busCapacity)--;

  if (*busCapacity == 0 || ridersOnStop[idZ - 1] == 0) {
    sem_post(busLeave);
  } else {
    sem_post(&busStops[idZ - 1]);
  }

  sem_wait(&busStops[Z]);

  print_process("L %d: going to ski\n", idL);
  (*busCapacity)++;

  if(*busCapacity == K) {
    sem_post(busLeave);
  } else {
    sem_post(&busStops[Z]);
  }
}