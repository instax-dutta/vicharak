#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/mutex.h>

#define DEVICE_NAME "vicharak"
#define SET_SIZE_OF_QUEUE _IOW('a', 'a', int *)
#define PUSH_DATA _IOW('a', 'b', struct data *)
#define POP_DATA _IOR('a', 'c', struct data *)

struct data {
    int length;
    char data[256]; // Maximum length 256 bytes
};

static int major;
static struct cdev vicharak_cdev;
static struct class *vicharak_class;

static char **queue;
static int queue_size = 0, head = 0, tail = 0, count = 0;
static wait_queue_head_t wq;
static struct mutex lock;

static int vicharak_open(struct inode *inode, struct file *file) {
    return 0;
}

static int vicharak_release(struct inode *inode, struct file *file) {
    return 0;
}

static long vicharak_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    struct data user_data;
    int ret;

    mutex_lock(&lock);

    switch (cmd) {
        case SET_SIZE_OF_QUEUE:
            if (queue) {
                while (count--) kfree(queue[head++ % queue_size]);
                kfree(queue);
            }
            queue_size = *(int *)arg;
            queue = kmalloc_array(queue_size, sizeof(char *), GFP_KERNEL);
            head = tail = count = 0;
            break;

        case PUSH_DATA:
            if (count == queue_size) {
                mutex_unlock(&lock);
                return -EBUSY; // Queue is full
            }
            if (copy_from_user(&user_data, (struct data *)arg, sizeof(user_data))) {
                mutex_unlock(&lock);
                return -EFAULT;
            }
            queue[tail] = kmalloc(user_data.length, GFP_KERNEL);
            memcpy(queue[tail], user_data.data, user_data.length);
            tail = (tail + 1) % queue_size;
            count++;
            wake_up_interruptible(&wq);
            break;

        case POP_DATA:
            if (count == 0) {
                mutex_unlock(&lock);
                if (wait_event_interruptible(wq, count > 0)) return -ERESTARTSYS;
                mutex_lock(&lock);
            }
            user_data.length = strlen(queue[head]);
            strncpy(user_data.data, queue[head], user_data.length);
            kfree(queue[head]);
            head = (head + 1) % queue_size;
            count--;
            if (copy_to_user((struct data *)arg, &user_data, sizeof(user_data))) {
                mutex_unlock(&lock);
                return -EFAULT;
            }
            break;

        default:
            mutex_unlock(&lock);
            return -EINVAL;
    }

    mutex_unlock(&lock);
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = vicharak_open,
    .release = vicharak_release,
    .unlocked_ioctl = vicharak_ioctl,
};

static int __init vicharak_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) return major;

    vicharak_class = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(vicharak_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    init_waitqueue_head(&wq);
    mutex_init(&lock);

    printk(KERN_INFO "vicharak: Registered with major %d\n", major);
    return 0;
}

static void __exit vicharak_exit(void) {
    device_destroy(vicharak_class, MKDEV(major, 0));
    class_destroy(vicharak_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "vicharak: Unregistered\n");
}

module_init(vicharak_init);
module_exit(vicharak_exit);
MODULE_LICENSE("GPL");
