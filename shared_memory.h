/*
  * Project 2 - IOS
  * Author: Tomáš Lajda <xlajda00>
  * Date: 2024-04-24
  * Description: This file contains functions declaretion for creating and releasing shared memory and semaphores.
  * 
*/

#include <semaphore.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

/**
 * Creates a semaphore with the specified initial value.
 *
 * @param value The initial value of the semaphore.
 * @return A pointer to the created semaphore.
 */
sem_t *create_semaphore(int value);

/**
 * Releases the semaphore and frees the associated resources.
 *
 * @param sem A pointer to the semaphore to be released.
 */
void release_semaphore(sem_t *sem);

/**
 * Creates a shared integer with the specified initial value.
 *
 * @param value The initial value of the shared integer.
 * @return A pointer to the created shared integer.
 */
int *create_shared_int(int value);

/**
 * Releases the shared integer and frees the associated resources.
 *
 * @param var A pointer to the shared integer to be released.
 */
void release_shared_int(int *var);

/**
 * Creates an array of semaphores with the specified size and initial value.
 *
 * @param size The size of the semaphore array.
 * @param value The initial value of each semaphore in the array.
 * @return A pointer to the created semaphore array.
 */
sem_t *create_semaphore_array(int size, int value);

/**
 * Releases the semaphore array and frees the associated resources.
 *
 * @param sem_array A pointer to the semaphore array to be released.
 * @param size The size of the semaphore array.
 */
void release_semaphore_array(sem_t *sem_array, int size);

/**
 * Creates an array of shared integers with the specified size and initial value.
 *
 * @param size The size of the shared integer array.
 * @param value The initial value of each shared integer in the array.
 * @return A pointer to the created shared integer array.
 */
int *create_shared_int_array(int size, int value);

/**
 * Releases the shared integer array and frees the associated resources.
 *
 * @param array A pointer to the shared integer array to be released.
 * @param size The size of the shared integer array.
 */
void release_shared_int_array(int *array, int size);

#endif // SHARED_MEMORY_H