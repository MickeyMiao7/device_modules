#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define onemillion 1024 * 1024

int main(){
    int i;
    FILE *fp = fopen("/dev/four", "w");
    // FILE *fp = fopen("temp.txt", "w");
    // const char a = 'a';
    const char b = 'b';
    const char c = 'c';


    if(fp == NULL)
        printf("device open error");

    char *a = (char*) malloc(sizeof(char) * 1 * onemillion);
    memset(a, 'a', sizeof(char) * 1 * onemillion);
    fprintf(fp, "%s", a);
    //for(i = 0; i < 1 * onemillion; i++)
    //    fprintf(fp, "%c", a);
    //for(i = 0; i < 3 * onemillion; i++)
    //    fprintf(fp, "%c", b);
    //for(i = 0; i < 1 * onemillion; i++)
    //    fprintf(fp, "%c", c);
    fclose(fp);
    free(a);
    return 0;
}
