#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "mychardev"
#define BUFFER_SIZE 1024

static int major;
static char buffer[BUFFER_SIZE];
static struct class *dev_class;
static struct cdev my_cdev;

/* Function prototypes */
static int dev_open(struct inode *inode, struct file *file);
static int dev_release(struct inode *inode, struct file *file);
static ssize_t dev_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset);
static ssize_t dev_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset);

/* File operations structure */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

/* Open function */
static int dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

/* Release function */
static int dev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

/* Read function */
static ssize_t dev_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset) {
    size_t len = strlen(buffer);
    if (*offset >= len) {
        return 0; // EOF
    }

    if (count > len - *offset) {
        count = len - *offset;
    }

    if (copy_to_user(user_buffer, buffer + *offset, count)) {
        return -EFAULT;
    }

    *offset += count;
    printk(KERN_INFO "Read %zu bytes from device\n", count);
    return count;
}

/* Write function */
static ssize_t dev_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset) {
    if (count > BUFFER_SIZE - 1) {
        count = BUFFER_SIZE - 1;
    }

    memset(buffer, 0, BUFFER_SIZE); // Clear the buffer
    if (copy_from_user(buffer, user_buffer, count)) {
        return -EFAULT;
    }

    buffer[count] = '\0'; // Null-terminate the string
    printk(KERN_INFO "Received %zu bytes from user\n", count);
    return count;
}

/* Module initialization */
static int __init char_driver_init(void) {
    /* Allocate a major number dynamically */
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register a major number\n");
        return major;
    }
    printk(KERN_INFO "Registered character device with major number %d\n", major);

    /* Create device class */
    dev_class = class_create("mycharclass");
    if (IS_ERR(dev_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create device class\n");
        return PTR_ERR(dev_class);
    }

    /* Create device file */
    device_create(dev_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    /* Initialize and add the cdev */
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    cdev_add(&my_cdev, MKDEV(major, 0), 1);

    printk(KERN_INFO "Character driver initialized\n");
    return 0;
}

/* Module cleanup */
static void __exit char_driver_exit(void) {
    cdev_del(&my_cdev);
    device_destroy(dev_class, MKDEV(major, 0));
    class_destroy(dev_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Character driver unloaded\n");
}

module_init(char_driver_init);
module_exit(char_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yashraj");
MODULE_DESCRIPTION("A simple Linux character driver");
MODULE_VERSION("1.0");
