/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include "threadpool.h"

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // create some work to do
    struct data work;
    work.a = 5;
    work.b = 10;

    struct data work2;
    work2.a = 7;
    work2.b = 2;

    struct data work3;
    work3.a = 1;
    work3.b = 6;

    struct data work4;
    work4.a = 4;
    work4.b = 4;

    struct data work5;
    work5.a = 14;
    work5.b = -5;

    // initialize the thread pool
    pool_init();

    // submit the work to the queue
    pool_submit(&add,&work);
    pool_submit(&add,&work2);
    pool_submit(&add,&work3);
    pool_submit(&add,&work4);
    pool_submit(&add,&work5);




    // may be helpful 
    //sleep(3);

    pool_shutdown();

    return 0;
}
