#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char *argv[]) {

    int fd = open("test.txt", O_RDWR | O_CREAT , 0666);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    char buf[1024] = {0};
    ssize_t bytes_read = read(fd, buf, sizeof(buf)-1);
    printf("bytes_read = %d\n", bytes_read);
    if (bytes_read == -1)
    {
        perror("read failed.");
        return -1;
    }
    buf[bytes_read] = '\0';
    printf("buf: %s\n", buf);
    
    char* buff = "hello world";
    ssize_t bytes_write = write(fd, buff, strlen(buff));
    if(bytes_write == -1)
    {
        perror("write failed:");
        return -1;
    }

    off_t offset = lseek(fd, 0, SEEK_SET);
    if (offset == -1)
    {
        perror("lseek failed: ");
        return -1;
    }
    char* buffs = "Start:\t";
    bytes_write = write(fd, buffs, strlen(buffs));
    if(bytes_write == -1)
    {
        perror("write failed:");
        return -1;
    }


    printf("writting done.\n");


    close(fd);
    return 0;
}