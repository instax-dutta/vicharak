#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEVICE_NAME "/dev/vicharak"
#define POP_DATA _IOR('a', 'c', struct data *)

struct data {
    int length;
    char data[256];
};

int main(void) {
    int fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    struct data d;
    if (ioctl(fd, POP_DATA, &d) < 0) {
        perror("Failed to pop data");
    } else {
        printf("Received: %s\n", d.data);
    }

    close(fd);
    return 0;
}
