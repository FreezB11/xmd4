#include <linux/init.h>
#include <linux/module.h>
// for file operation definition
#include <linux/fs.h>
// required to create character device
#include <linux/cdev.h>
// gives the ability toi move data from userspce to kernel space and vice versa
#include <linux/uaccess.h>

int mem_size = 1024;
// pseudo device memory
static char dev_buff[1024];

/*
  device number(major and minor)  
  our driver exist in the kernel and it controls with open and close 
  operation
  when you use an open system call on the device file, how does the kernel connects the open 
  sytem call to the intended driver's open method??
  -> to establish the connection the kernel uses "device number"
*/
/*
    ##CREATE A DEVICE NUMBER##
    we ask the kernel to dynamically allocate the device number(s)

    #creation#
    alloc_chrdev_region();  //1.create device number
        int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char*name);
    cdev_init();            //2.make a char device registration with vfs
        void cdev_init(struct cdev *cdev, const struct file_operations *fops);
    cdev_add();       
        int cdev_add(struct cdev *p, dev_t dev, unsigned count);      
        here the count = no of consecutive minor numbers corresponding to this device

    class_create();         //3.create device files
    device_create();

    #deletion#
    unregister_chrdev_region();

    cdev_del();

    class_destroy();
    device_destroy(); 
*/

// device number creation
dev_t device_number;
// class variable
static struct class *mydev_class;
// cdev variable
struct cdev my_cdev;// ={
//     .owner = THIS_MODULE,
// };
// we shall now provide the file_ops definition
static int my_open(struct inode *,struct file *);
// static int my_open(struct inode *, struct file *){
//     printk(KERN_INFO "file opened\n");
//     return 0;
// }
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char __user *, size_t count, loff_t *offset);
static ssize_t my_write(struct file *file, const char __user *, size_t count, loff_t *offset);



// file operations of the driver
static struct file_operations my_ops ={
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

// here are the code for the definitions 
// static int my_open(struct inode *, struct file *){
//     printk(KERN_INFO "file opened\n");
//     return 0;
// }

static int my_open(struct inode *, struct file *){
    printk(KERN_INFO "file opened\n");
    return 0;
}

/* Release function */
static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *usr_buff, size_t count, loff_t *offset){
    printk(KERN_INFO "file is being read\n");
    //taking the size of the buffer memory
    size_t len = strlen(dev_buff);
    if(*offset >= len){
        return 0; // EOF(end of file)
        printk("Nothing to read\n");
    }

    if(count > len - *offset){
        count = len - *offset;
    }

    if(copy_to_user(usr_buff, dev_buff + *offset,count)){
        return -EFAULT;
    }

    *offset += count;
    printk(KERN_INFO "Read %zu bytes from the device\n",count);
    return count;
}

static ssize_t my_write(struct file *file, const char __user *usr_buff,size_t count, loff_t *offset){
    if(count > mem_size - 1){
        count = mem_size - 1;
    }

    // we shall clear the memory
    memset(dev_buff, 0, mem_size);
    if(copy_from_user(dev_buff, usr_buff, count)){
        return -EFAULT;
    }
    dev_buff[count] = '\0';
    printk(KERN_INFO "recieved %zu bytes from the user\n",count);
    return count;
}

static int __init mod_init(void){
    printk(KERN_INFO "Starting module\n");
    // dynamically allocate a device number
    if(alloc_chrdev_region(&device_number,0,1,"mydev")){
        printk(KERN_ALERT "oops!! something went wrong");
    }

    //create the class
    mydev_class = class_create("mydevclass");
    /*
        should write a error handler
    */
    // create the device file
    if(IS_ERR(device_create(mydev_class, NULL, device_number, NULL, "mydev"))){
        printk(KERN_ALERT "Failed to create device\n");
        class_destroy(mydev_class);
        unregister_chrdev_region(device_number,1);
        return PTR_ERR(device_create(mydev_class, NULL, device_number, NULL, "mydev"));
    }
    //initializing cdev
    cdev_init(&my_cdev,&my_ops);
    //adding the cdev to the kernel vfs
    my_cdev.owner = THIS_MODULE;
    cdev_add(&my_cdev, device_number,1);
    printk(KERN_INFO "Driver initialized\n");
    printk(KERN_INFO "%d major number",MAJOR(device_number));
    return 0;
}

static void __exit mod_exit(void){
    printk(KERN_INFO "Closing module\n");
    //if(unregister_chrdev_region())
    cdev_del(&my_cdev);
    device_destroy(mydev_class, device_number);
    class_destroy(mydev_class);
    unregister_chrdev_region(device_number,1);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");

/*

 Run this after you hav einjected the module
 mknod /dev/<your_device_name> c <major_number> 0
 chmod 666 /dev/<name>
*/