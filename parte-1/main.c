#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>

void print_thread(void *args)
{
    char *name = (char *)args;
    long id_thcurrent = thrd_current();
    long id_syscall = syscall(SYS_gettid);

    printf("Eu sou a thread %s -\n \tID da Thread (thrd_current): %ld \n \tID da Thread (syscall): %ld\n",
           name,
           id_thcurrent,
           id_syscall);
}

int main()
{
    int n = 0;
    char name[16];

    printf("Informe a quantidade de Threads: ");
    scanf("%d", &n);
    printf("\n");

    thrd_t *thread;
    thread = malloc(sizeof (thrd_t) * n);

    for (int index = 0; index < n; ++index) {
        sprintf(name, "thread_%d", index);
        thrd_create(&thread[index], (thrd_start_t) print_thread, name);
    }

    for (int index = 0; index < n; ++index) {
        thrd_join(thread[index], NULL);
    }

    return 0;
}