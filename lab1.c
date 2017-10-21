#include "lab1.h"
#include "thpool.h"
#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)
int len = 0, row_no = 0, thread_number = 1;
int** M;
int d = -8;
int tmax(int a, int b, int c){
    return ((a>b && a>c) ? a : (b>c) ? b:c);
}
int *c, *r;
struct args *a;
struct args{
    int line, id, width;
};
void calc_up(void *t){
    struct args *arg = t;
    int i = arg->line;
    int st = i - (arg->id * arg->width);
    for (int j=st, cnt=1; j>0 && cnt<=arg->width; j--,cnt++) {
        if (j == 0 || i-j == 0) continue;
        M[j][i - j] = tmax(
            M[j-1][i-j-1] + S[(j-1) % 26][(i-j-1)%26],
            r[j] + d,
            c[i - j] + d
        );
        r[j] = max(r[j], M[j][i-j]);
        c[i-j] = max(c[i-j], M[j][i-j]);
    }
}
void calc_down(void *t){
    struct args *arg = t;
    int i = arg->line;
    int st = len - 1 - (arg->id * arg->width);
    for (int j = st, cnt = 1; j>=i - len + 1 && cnt<=arg->width; j--,cnt++){
        M[j][i-j] = tmax(
            M[j-1][i-j-1] + S[(j-1) % 26][(i-j-1) % 26],
            r[j] + d,
            c[i-j] + d
        );
        r[j] = max(r[j], M[j][i-j]);
        c[i-j] = max(c[i-j], M[j][i-j]);
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
    a = malloc(len * sizeof(struct args));
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
    
    //assignment of task
    for (int i=1; i<len; i++) {
        int number_per_task = (i+1+1) / thread_number;
        if (number_per_task == 0) number_per_task++;
        int total = min(thread_number, i);
        
        for (int k=0; k<total; k++) {
            a[k].line = i; a[k].id = k; a[k].width = number_per_task;
            thpool_add_work(thpool, (void*)calc_up, &a[k]);
        }
        thpool_wait(thpool);
    }

    for (int i=1; i<len; i++) {
        int number_per_thread = (len - i + 1) / thread_number;
        if (number_per_thread == 0) number_per_thread++;
        int total = min(thread_number, i);
        for (int k=0; k<total; k++) {
            a[k].line = len + i - 1; a[k].id = k; a[k].width = number_per_thread;
            thpool_add_work(thpool, (void*)calc_down, &a[k]);
        }
        thpool_wait(thpool);
    }
    
    //for (int i=0; i<len; i++) printf("%4d%c", M[row_no][i], " \n"[i==len-1]);

    // for (int i=0; i<len; i++){
    //     for (int j=0; j<len; j++) {
    //         printf("%4d%c", M[i][j], " \n"[j==len-1]);
    //     }
    // }

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
    free(a);
    exit(0);
}
