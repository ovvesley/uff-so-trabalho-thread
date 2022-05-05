#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

typedef struct th_args
{
    int **matrix_a;
    int **matrix_b;
    int **matrix_c;
    int start_heigth;
    int end_heigth;
    int start_width;
    int end_width;
    char thread_name[16];
} threads_args;

void sum_quart(int **matrix_a, int **matrix_b, int **matrix_c, int index_start_width, int index_end_width, int index_start_heigth, int index_end_heigth);

void *sum_matrix_thread(void *args)
{
    threads_args *thargs = (threads_args *)args;
    long id_thcurrent = thrd_current();
    long id_syscall = syscall(SYS_gettid);

    sum_quart(
            thargs->matrix_a,
            thargs->matrix_b,
            thargs->matrix_c,
            thargs->start_width,
            thargs->end_width,
            thargs->start_heigth,
            thargs->end_heigth);
    return thargs;
}

int **make_matrix(int height, int width)
{
    int **matrix = (int **)malloc(height * sizeof(int *));
    for (int index_i = 0; index_i < height; index_i++)
    {
        matrix[index_i] = (int *)malloc(width * sizeof(int));
    }
    return matrix;
}

void hydrate_matrix(int **matrix, int heigth, int width)
{
    for (int index_i = 0; index_i < heigth; index_i++)
    {
        for (int index_j = 0; index_j < width; index_j++)
        {
            matrix[index_i][index_j] = (rand() % 100);
        }
    }
}

void print_matrix(int **matrix, int heigth, int width, char *title)
{
    char text[1000];

    printf("%sApenas a amostragem de (%d)x(%d)\n\n", title, heigth, width);
    for (int index_i = 0; index_i < heigth; index_i++)
    {
        for (int index_j = 0; index_j < width; index_j++)
        {
            printf("%d\t", matrix[index_i][index_j]);
        }
        printf("\n");
    }
}

void sum_quart(int **matrix_a, int **matrix_b, int **matrix_c, int index_start_width, int index_end_width, int index_start_heigth, int index_end_heigth)
{
    for (int index_i = index_start_width; index_i < index_end_width; index_i++)
    {
        for (int index_j = index_start_heigth; index_j < index_end_heigth; index_j++)
        {
            for (int i = 0; i < 10000; ++i) {
                matrix_c[index_i][index_j] = matrix_a[index_i][index_j] + matrix_b[index_i][index_j];
            }
        }
    }
}

float diff_time_calculate(
        long int startSec, long int endSec,
        long int startUSec, long int endUSec)
{
    return (endSec - startSec) + 1e-6 * (endUSec - startUSec);
}

void sum_matrix(int **matrix_a, int **matrix_b, int **matrix_c, int heigth, int width)
{
    sum_quart(matrix_a, matrix_b, matrix_c, 0, 500, 0, 500);
    sum_quart(matrix_a, matrix_b, matrix_c, 500, 1000, 0, 500);
    sum_quart(matrix_a, matrix_b, matrix_c, 0, 500, 500, 1000);
    sum_quart(matrix_a, matrix_b, matrix_c, 500, 1000, 500, 1000);
}

threads_args *make_threads_args(
        int **matrix_a,
        int **matrix_b,
        int **matrix_c,
        char *thread_name,
        int start_width,
        int end_width,
        int start_heigth,
        int end_heigth)
{
    threads_args *th_args = (threads_args *)malloc(sizeof(threads_args));
    th_args->start_width = start_width;
    th_args->end_width = end_width;

    th_args->start_heigth = start_heigth;
    th_args->end_heigth = end_heigth;
    th_args->matrix_a = matrix_a;
    th_args->matrix_b = matrix_b;
    th_args->matrix_c = matrix_c;
    strcpy(th_args->thread_name, thread_name);
    return th_args;
}

void sum_matrixes_without_thread(int **matrix_a, int **matrix_b, int **matrix_c, int heigh, int width)
{
    sum_matrix(matrix_a, matrix_b, matrix_c, heigh, width);
}

void sum_matrixes_with_thread(int **matrix_a, int **matrix_b, int **matrix_c, int heigh, int width)
{
    struct timeval start, end;
    float diff_time;
    gettimeofday(&start, NULL);

    thrd_t thread[4];
    void *threturn[4];

    threads_args *th_args_0, *th_args_1, *th_args_2, *th_args_3;

    th_args_0 = make_threads_args(matrix_a, matrix_b, matrix_c, "thread_00", 0, 500, 0, 500);
    th_args_1 = make_threads_args(matrix_a, matrix_b, matrix_c, "thread_01", 500, 1000, 0, 500);
    th_args_2 = make_threads_args(matrix_a, matrix_b, matrix_c, "thread_02", 0, 500, 500, 1000);
    th_args_3 = make_threads_args(matrix_a, matrix_b, matrix_c, "thread_03", 500, 1000, 500, 1000);

    thrd_create(&thread[0], (thrd_start_t)sum_matrix_thread, th_args_0);
    thrd_create(&thread[1], (thrd_start_t)sum_matrix_thread, th_args_1);
    thrd_create(&thread[2], (thrd_start_t)sum_matrix_thread, th_args_2);
    thrd_create(&thread[3], (thrd_start_t)sum_matrix_thread, th_args_3);

    thrd_join(thread[0], NULL);
    thrd_join(thread[1], NULL);
    thrd_join(thread[2], NULL);
    thrd_join(thread[3], NULL);
}

void track_time(void (*callback)(int **matrix_a, int **matrix_b, int **matrix_c, int heigth, int width),
                char *name_execution, int **matrix_a,
                int **matrix_b, int **matrix_c,
                int heigth, int width)
{
    struct timeval start, end;
    float diff_time;
    gettimeofday(&start, NULL);

    printf("\n%s\n", name_execution);
    callback(matrix_a, matrix_b, matrix_c, heigth, width);

    gettimeofday(&end, NULL);
    diff_time = diff_time_calculate(
            start.tv_sec,
            end.tv_sec,
            start.tv_usec,
            end.tv_usec);
    printf("Tempo de Execução %f segundos\n", diff_time);
}
int main()
{
    int n;

    int heigh = 1000, width = 1000;

    int **matrix_a, **matrix_b, **matrix_c;

    matrix_a = make_matrix(heigh, width);
    matrix_b = make_matrix(heigh, width);
    matrix_c = make_matrix(heigh, width);

    hydrate_matrix(matrix_a, heigh, width);
    hydrate_matrix(matrix_b, heigh, width);
    track_time(&sum_matrixes_without_thread, "SOMA DE MATRIZES SEM THREAD", matrix_a, matrix_b, matrix_c, heigh, width);

//    print_matrix(matrix_a, 10, 10, "\n\nMatriz A\n\n");
//    print_matrix(matrix_b, 10, 10, "\n\nMatriz B\n\n");
//    print_matrix(matrix_c, 10, 10, "\n\nMatriz SOMADA\n\n");

    matrix_a = NULL;
    matrix_b = NULL;
    matrix_c = NULL;

    matrix_a = make_matrix(heigh, width);
    matrix_b = make_matrix(heigh, width);
    matrix_c = make_matrix(heigh, width);

    hydrate_matrix(matrix_a, heigh, width);
    hydrate_matrix(matrix_b, heigh, width);
    track_time(&sum_matrixes_with_thread, "SOMA DE MATRIZES COM 4 THREADS", matrix_a, matrix_b, matrix_c, heigh, width);

//    print_matrix(matrix_a, 10, 10, "\n\nMatriz A\n\n");
//    print_matrix(matrix_b, 10, 10, "\n\nMatriz B\n\n");
//    print_matrix(matrix_c, 10, 10, "\n\nMatriz SOMADA\n\n");

    return 0;
}