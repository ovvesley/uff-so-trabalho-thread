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

    printf("Informe a quantidade de Threads: ");
    scanf("%d", &n);
    printf("\n");

    thrd_t *thread;
    char **names;
    thread = malloc(sizeof (thrd_t) * n);

    names = malloc(sizeof (char**) * n);
    for (int index = 0; index< n; index++){
        names[index] = malloc(sizeof (char*) * 16);
        sprintf(names[index], "thread_%d", index);
    }

    for (int index = 0; index < n; ++index) {
        thrd_create(&thread[index], (thrd_start_t) print_thread, names[index]);
    }

    for (int index = 0; index < n; ++index) {
        thrd_join(thread[index], NULL);
    }


    for (int index = n; index > 0; --index) {
        free(names[index]);
    }
    free(thread);



    return 0;
}
