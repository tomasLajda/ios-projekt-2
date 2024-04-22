#include <semaphore.h> 
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

sem_t *create_semaphore(int value);
void release_semaphore(sem_t *sem);

int *create_shared_int(int value);
void release_shared_int(int *var);

sem_t *create_semaphore_array(int size, int value);
void release_semaphore_array(sem_t *sem_array, int size);

int *create_shared_int_array(int size, int value);
void release_shared_int_array(int *array, int size);

#endif // SHARED_MEMORY_H