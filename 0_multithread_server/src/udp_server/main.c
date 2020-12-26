#include "query_thread.h"
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

const static int32_t S_PORT = 55555;

#define THREAD_NUM  (5)

int32_t tlog[THREAD_NUM];
pthread_t tid[THREAD_NUM];
int32_t tmp[THREAD_NUM];

void single_hander_cb(int s)
{
    printf("\n");
    for (int32_t i = 0; i < THREAD_NUM; ++i)
    {
        printf("tid: %d, cnt: %d\n", i, tlog[i]);
    }
    exit(0);
}

int main(int argc, char const *argv[])
{
    struct sigaction single_hander;
    single_hander.sa_handler = single_hander_cb;
    sigemptyset(&single_hander.sa_mask);
    single_hander.sa_flags = 0;
    sigaction(SIGINT, &single_hander, NULL);

    for (int32_t i = 0; i < THREAD_NUM; ++i)
    {
        tmp[i] = i;
        pthread_create(&tid[i], NULL, query_thread_cb, &tmp[i]);
        usleep(2000);
    }
    printf("please input Ctrl+C\n");

    for (int i = 0; i < THREAD_NUM; ++i)
    {
        pthread_join(tid[i], NULL);
    }
    return 0;
}

