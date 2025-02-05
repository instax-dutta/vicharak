#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE_NAME "/dev/vicharak"
#define PUSH_DATA _IOW('a', 'b', struct data *)

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
    d.length = 3;
    strcpy(d.data, "xyz");

    if (ioctl(fd, PUSH_DATA, &d) < 0) {
        perror("Failed to push data");
    }

    close(fd);
    return 0;
}
