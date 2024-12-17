#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

int mem_size = 1024;
static char buff_mem[1024];

// the proc
static struct proc_dir_entry *my_proc;

static int my_open(struct inode *, struct file *);
static ssize_t my_read(struct file *file, char __user *, size_t count, loff_t *offset);
static ssize_t my_write(struct file *file, const char __user *,size_t count, loff_t *offset);

// proc operations 
struct proc_ops my_ops = {
    .proc_open = my_open,
    .proc_read = my_read,
    .proc_write = my_write,
};

static int my_open(struct inode *, struct file *){
    printk(KERN_INFO "file opened\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *usr_buff, size_t count, loff_t *offset){
    printk(KERN_INFO "file is being read\n");
    //taking the size of the buffer memory
    size_t len = strlen(buff_mem);
    if(*offset >= len){
        return 0; // EOF(end of file)
        printk("Nothing to read\n");
    }

    if(count > len - *offset){
        count = len - *offset;
    }

    if(copy_to_user(usr_buff, buff_mem + *offset,count)){
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
    memset(buff_mem, 0, mem_size);
    if(copy_from_user(buff_mem, usr_buff, count)){
        return -EFAULT;
    }
    buff_mem[count] = '\0';
    printk(KERN_INFO "recieved %zu bytes from the user\n",count);
    return count;
}

static int __init mod_init(void){
    printk(KERN_INFO "Entring into the mod\n");
    my_proc = proc_create("myproc",0,NULL,&my_ops);
    if(!my_proc){
        printk(KERN_ALERT "failed to create the proc\n");
        return -1;
    }
    printk(KERN_INFO "proc made\n");

    return 0;
}

static void __exit mod_exit(void){
    printk(KERN_INFO "Closing the mod\n");
    proc_remove(my_proc);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HS4Y");
MODULE_DESCRIPTION("learning readable kernel module");
MODULE_VERSION("0.9");