/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1
pthread_mutex_t mutex;// обьявляем мютекс и семафор
sem_t sema;
int head = 0; // индексы начала и хвоста списка
int tail = 0;

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}
task;

// the work queue
task worktodo;

task queue[QUEUE_SIZE + 1];
// the worker bee
pthread_t bee [NUMBER_OF_THREADS];

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) {
    pthread_mutex_lock(&mutex);// блокируем , чтобы к очереди никто не мог обратиться к очереди

    int next_tail = (tail + 1) % QUEUE_SIZE;//проверка на переполнение
    if (next_tail == head) {
        pthread_mutex_unlock(&mutex);
        return 1; // Queue overflow
    }

    queue[tail] = t;// добавляем наш таст
    tail = next_tail;// изменяем хвост

    pthread_mutex_unlock(&mutex);
    return 0;
}

// remove a task from the queue
task dequeue() {
    pthread_mutex_lock(&mutex);// блокируем , чтобы к очереди никто не мог обратиться к очереди

    task t = {NULL, NULL};
    if (head != tail) {
        t = queue[head];
        head = (head + 1) % QUEUE_SIZE;// для реализации кольцевой очереди
    }else{
        perror("The queue is empty");
    }


    pthread_mutex_unlock(&mutex);
    return t;
}

// the worker thread in the thread pool
void *worker(void *param)
{

    while (TRUE) {
        sem_wait(&sema);//блокируем семафор

        worktodo = dequeue();// берем задачу из пула

        if (worktodo.data != NULL && worktodo.function != NULL)//если дата и функция не нулевые - выполняем
            execute(worktodo.function, worktodo.data);

    }

    pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    worktodo.function = somefunction;//считываем функцию и дату
    worktodo.data = p;
    int success = enqueue(worktodo);
    if (!success)
        sem_post(&sema);//добавляем задачу в очередь. Если удачно , то увеличиваем семафор

    return success;
}

// initialize the thread pool
void pool_init(void)
{
    sem_init(&sema, 0, 0);// инициализация самафора и мютекса
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
        pthread_create(&bee[i],NULL,worker,NULL);// создаем каждый поток со статусом worker
}

// shutdown the thread pool
void pool_shutdown(void)
{
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_cancel(bee[i]);//запрос на заверщние
        pthread_join(bee[i],NULL);//ожидание заверщения потока
    }
    sem_destroy(&sema);//уничтожаем мютекс и семафору
    pthread_mutex_destroy(&mutex);
}
