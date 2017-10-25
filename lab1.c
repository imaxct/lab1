#include "lab1.h"
#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)
int len = 0, row_no = 0, thread_number = 1, d = -8;
int **M, *c, *r;
pthread_t *threads;
bool** vis;
int tmax(int a, int b, int c){
    return ((a>b && a>c) ? a : (b>c) ? b:c);
}

void *calc(void *t){
    int id = (intptr_t)t;
    for (int i=id; i<len; i += thread_number){
        //printf("%d-%d\n", id, i);
        for (int j=1; j<len; j++){
            while (!vis[j][i-1]);
            M[j][i] = tmax(
                M[j-1][i-1] + S[(j-1)%26][(i-1)%26],
                r[j] + d,
                c[i] + d
            );
            r[j] = max(M[j][i], r[j]);
            c[i] = max(M[j][i], c[i]);
            vis[j][i] = true;
        }
    }
    return NULL;
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
        fprintf(stderr, "invalid row number\n");
        exit(-1);
    }
    
    /**
     * init arrays
     */
    
    M = malloc(len * sizeof(int*));
    c = malloc(len * sizeof(int));
    r = malloc(len * sizeof(int));
    vis = malloc(len * sizeof(bool*));
    threads = malloc(len * sizeof(pthread_t));
    for (int i=0; i<len; i++) {
        M[i] = malloc(len * sizeof(int));
        M[i][0] = i * d;
        M[0][i] = i * d;
        vis[i] = malloc(len * sizeof(bool));
    }
    for (int i=0; i<len; i++) {
        c[i] = r[i] = -1000000000;
        vis[i][0] = vis[0][i] = true;
    }
    
    //assignment of task
    for (int i=1; i<=thread_number; i++) {
        pthread_create(&threads[i-1], NULL, calc, (void*)(intptr_t)i);
    }
    for (int i=1; i<=thread_number; i++) {
        pthread_join(threads[i-1], NULL);
    }
    //for (int i=0; i<len; i++) printf("%4d%c", M[row_no][i], " \n"[i==len-1]);

    /**
     * delete arrays
     */
    for (int i=0; i<len; i++) {
        free(M[i]);
        free(vis[i]);
    }
    free(M);
    free(c);
    free(r);
    free(vis);
    free(threads);
    exit(0);
}
