/*
  * Project 2 - IOS
  * Author: Tomáš Lajda <xlajda00>
  * Date: 2024-04-24
  * Description: This file contains functions for creating and releasing shared memory and semaphores.
  * 
*/

#include "shared_memory.h"

/**
 * @brief Creates a semaphore with the specified initial value.
 * 
 * @param value The initial value of the semaphore.
 * @return A pointer to the created semaphore.
 */
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

/**
 * @brief Releases the specified semaphore.
 * 
 * @param sem A pointer to the semaphore to be released.
 */
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

/**
 * @brief Creates a shared integer with the specified initial value.
 * 
 * @param value The initial value of the shared integer.
 * @return A pointer to the created shared integer.
 */
int *create_shared_int(int value) {
  int *var = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

  if(var == MAP_FAILED) {
    fprintf(stderr, "Cannot create shared variable\n");
    exit(EXIT_FAILURE);
  }

  *var = value;
  return var;
}

/**
 * @brief Releases the specified shared integer.
 * 
 * @param var A pointer to the shared integer to be released.
 */
void release_shared_int(int *var) {
  if (munmap(var, sizeof(int)) == -1) {
    fprintf(stderr, "Cannot unmap shared variable\n");
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief Creates an array of semaphores with the specified size and initial value.
 * 
 * @param size The size of the semaphore array.
 * @param value The initial value of each semaphore in the array.
 * @return A pointer to the created semaphore array.
 */
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

/**
 * @brief Releases the specified semaphore array.
 * 
 * @param sem_array A pointer to the semaphore array to be released.
 * @param size The size of the semaphore array.
 */
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

/**
 * @brief Creates an array of shared integers with the specified size and initial value.
 * 
 * @param size The size of the shared integer array.
 * @param value The initial value of each shared integer in the array.
 * @return A pointer to the created shared integer array.
 */
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

/**
 * @brief Releases the specified shared integer array.
 * 
 * @param array A pointer to the shared integer array to be released.
 * @param size The size of the shared integer array.
 */
void release_shared_int_array(int *array, int size) {
  if (munmap(array, sizeof(int) * size) == -1) {
    fprintf(stderr, "Cannot unmap shared integer array\n");
    exit(EXIT_FAILURE);
  }
}