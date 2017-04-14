#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
//needed for IO things. Attention that this is different from kernel mode 

#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_WRITE _IOW(SCULL_IOC_MAGIC, 2, char*)
#define SCULL_READ _IOR(SCULL_IOC_MAGIC, 3, char*)
#define SCULL_WRITE_READ _IOWR(SCULL_IOC_MAGIC, 4, char*)
#define DEV_MSG_SIZE 100

int lcd;

void test()
{
    int k, i, sum; 
    char s[3];
    memset(s, '2', sizeof(s)); 
    printf("maximum size of dev_msg is set to: %d\n", DEV_MSG_SIZE);
    printf("test begin!\n");

    printf("test file write:\n");
    k = write(lcd, s, sizeof(s)); 
    printf("written = %d\n", k);
    printf("\n");

    printf("test hello:\n");
    k = ioctl(lcd, SCULL_HELLO); 
    printf("result = %d\n", k);
    printf("\n");

    printf("test IOW:\n");
    char a[] = "Hi, this is from Miao Qi";
    printf("write string: %s\n", a);
    k = ioctl(lcd, SCULL_WRITE, a); 
    printf("result = %d\n", k);
    printf("\n");

    printf("test IOR:\n");
    char b[DEV_MSG_SIZE]; 
    printf("read dev_msg from ioctl\n");
    k = ioctl(lcd, SCULL_READ, b); 
    printf("result = %s\n", b);
    printf("\n");

    printf("test IOWR:\n");
    char c[DEV_MSG_SIZE];
    memset(c, 'a', 5);
    printf("before chaning, user_msg is set to:%s\n", c);
    k = ioctl(lcd, SCULL_WRITE_READ, c); 
    printf("after chaning, user_msg is: %s\n", c);  
}

int main(int argc, char **argv)
{
    lcd = open("/dev/ioctl", O_RDWR);
    if (lcd == -1) {
        perror("unable to open lcd"); 
        exit(EXIT_FAILURE);
    } 
    test();
    close(lcd);
    return 0;
 }
