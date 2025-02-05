#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_NAME "/dev/vicharak"
#define SET_SIZE_OF_QUEUE _IOW('a', 'a', int *)

int main(void) {
    int fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    int size = 10; // Set queue size to 10
    if (ioctl(fd, SET_SIZE_OF_QUEUE, &size) < 0) {
        perror("Failed to set queue size");
    }

    close(fd);
    return 0;
}
