#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define onemillion 1024 * 1024

int main(){
    int i;
    FILE *fp = fopen("file.txt", "w");


    if(fp == NULL)
        printf("device open error");

//     char *a = (char*) malloc(sizeof(char) * 4 * onemillion + 1);
//     memset(a, 'a', sizeof(char) * 4 * onemillion + 1);
//     fprintf(fp, "%s", a);
// 
    char *a = (char*) malloc(sizeof(char) * 1 * onemillion);
    memset(a, 'a', sizeof(char) * 1 * onemillion);
    fprintf(fp, "%s", a);

    char *b = (char*) malloc(sizeof(char) * 3 * onemillion);
    memset(b, 'b', sizeof(char) * 3 * onemillion);
    fprintf(fp, "%s", b);

    char *c = (char*) malloc(sizeof(char) * 1 * onemillion);
    memset(c, 'c', sizeof(char) * 1 * onemillion);
    fprintf(fp, "%s", c);

    fclose(fp);
    free(a);
    free(b);
    free(c);
    return 0;
}
