#include "shared_memory.h"

// Function to create a semaphore
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

// Function to release a semaphore
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

// Function to create a shared integer variable
int *create_shared_int(int value) {
  int *var = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  if(var == MAP_FAILED) {
    fprintf(stderr, "Cannot create shared variable\n");
    exit(EXIT_FAILURE);
  }

  *var = value;
  return var;
}

// Function to release a shared integer variable
void release_shared_int(int *var) {
  if (munmap(var, sizeof(int)) == -1) {
    fprintf(stderr, "Cannot unmap shared variable\n");
    exit(EXIT_FAILURE);
  }
}

// Function to create an array of semaphores
sem_t *create_semaphore_array(int size, int value) {
  sem_t *sem_array = mmap(NULL, sizeof(sem_t) * size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  if(sem_array == MAP_FAILED) {
    fprintf(stderr, "Cannot create semaphore array\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < size; i++) {
    if(sem_init(&sem_array[i], 1, value) == -1) {
      fprintf(stderr, "Cannot initialize semaphore\n");
      exit(EXIT_FAILURE);
    }
  }

  return sem_array;
}

// Function to release an array of semaphores
void release_semaphore_array(sem_t *sem_array, int size) {
  for (int i = 0; i < size; i++) {
    if (sem_destroy(&sem_array[i]) == -1) {
      fprintf(stderr, "Cannot destroy semaphore\n");
      exit(EXIT_FAILURE);
    }
  }

  if (munmap(sem_array, sizeof(sem_t) * size) == -1) {
    fprintf(stderr, "Cannot unmap semaphore array\n");
    exit(EXIT_FAILURE);
  }
}

// Function to create an array of shared integers
int *create_shared_int_array(int size, int value) {
  int *array = mmap(NULL, sizeof(int) * size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  if(array == MAP_FAILED) {
    fprintf(stderr, "Cannot create shared integer array\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < size; i++) {
    array[i] = value;
  }

  return array;
}

// Function to release an array of shared integers
void release_shared_int_array(int *array, int size) {
  if (munmap(array, sizeof(int) * size) == -1) {
    fprintf(stderr, "Cannot unmap shared integer array\n");
    exit(EXIT_FAILURE);
  }
}