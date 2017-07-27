#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

void* run(void* args)
{
    printf("I am thread, and return 100\n");
    pthread_exit((void*)100);
    //return (void*)100;
}

int main(int argc, char* args[])
{
    void* thread_return;

    pthread_t tid;
    if(pthread_create(&tid, NULL, run, NULL))
    {
        printf("create thread error\n");
        return 0;
    }

    //sleep(1);
    pthread_join(tid, &thread_return);
    printf("I am main, got thread return value:%d\n", (int*)thread_return);

    return 0;
}
