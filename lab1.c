#include "lab1.h"
#include "thpool.h"
#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)
int len = 0, row_no = 0, thread_number = 1, number_of_task = 1;
int** M;
int d = -8;
int tmax(int a, int b, int c){
    return ((a>b && a>c) ? a : (b>c) ? b:c);
}
int *c, *r;
struct args{
    int line, id;
};
void calc(void *t){
    struct args *arg = t;
    int st = arg->id * number_of_task + 1, ed = number_of_task * (arg->id + 1);
    int i = arg->line;
    for (int j=st; j<i; j++) {
        printf("%d (%d,%d)\n",arg->line, i-j, j);
        M[i - j][j] = tmax(
            M[i-j-1][j-1] + S[(i-j-1)%26][(j-1) % 26],
            r[i - j] + d,
            c[j] + d
        );
        r[i-j] = max(r[i-j], M[i-j][j]);
        c[j] = max(c[j], M[i-j][j]);
    }
}
int main(int argc, char* argv[]){
    if (argc != 4) {
        fprintf(stderr, "wrong number of arguments, usage:\n\t %s <dimension> <row_number> <thread_number>\n", argv[0]);
        exit(-1);
    }
    
    len = atoi(argv[1]) + 1;
    row_no = atoi(argv[2]);
    thread_number = atoi(argv[3]);
    
    if (thread_number <= 0 || thread_number > 16) {
        fprintf(stderr, "wrong number of threads (1,16]\n");
        exit(-1);
    }

    if (row_no < 0 || row_no >= len) {
        fprintf(stderr, "");
        exit(-1);
    }
    
    /**
     * init arrays
     */
    
    M = malloc(len * sizeof(int*));
    c = malloc(len * sizeof(int));
    r = malloc(len * sizeof(int));
    for (int i=0; i<len; i++) c[i] = r[i] = -1000000000;
    for (int i=0; i<len; i++) {
        M[i] = malloc(len * sizeof(int));
        M[i][0] = i * d;
        M[0][i] = i * d;
    }

    /**
     * init thread pool
     */
    threadpool thpool = thpool_init(thread_number);

    for (int i=1; i<len; i++) {
        //assignment of task
        number_of_task = i / thread_number;
        printf("number%d: %d\n", i, number_of_task);
        if (number_of_task == 0) number_of_task++;
        for (int k=1; k<=number_of_task; k++) {
            struct args a; a.line = i; a.id = k;
            thpool_add_work(thpool, (void*)calc, &a);
        }
        thpool_wait(thpool);
    }
    // for (int i=1; i<len; i++) {
    //     for (int j=i; j<len; j++) {
    //         M[len - j + i - 1][j] = tmax(
    //             M[len - j + i - 2][j - 1] + S[(len - j + i - 2) % 26][(j-1)%26],
    //             r[len - j + i - 1] + d,
    //             c[j] + d
    //         );
    //         r[len - j + i - 1] = max(r[len - j + i - 1], M[len - j + i - 1][j]);
    //         c[j] = max(c[j], M[len - j + i - 1][j]);
    //     }
    // }
    
    
    //for (int i=0; i<len; i++) printf("%4d%c", M[row_no][i], " \n"[i==len-1]);

    for (int i=0; i<len; i++){
        for (int j=0; j<len; j++) {
            printf("%4d%c", M[i][j], " \n"[j==len-1]);
        }
    }

    /**
     * destroy thread pool
     */
    thpool_destroy(thpool);
    /**
     * delete arrays
     */
    for (int i=0; i<len; i++) free(M[i]);
    free(M);
    free(c);
    free(r);
    exit(0);
}
