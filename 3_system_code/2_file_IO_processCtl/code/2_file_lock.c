#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


int main(){
    int fd = open("log.txt", O_RDWR | O_CREAT, 0666);
    if (fd == -1)
    {
        perror("Open failed: ");
        return -1;
    }

    /// 设置文件锁
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;  // 锁定整个文件
	
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl failed");
        close(fd);
        return 1;
    }

    printf("File locked for writing\n");

    close(fd);
    return 0;
}