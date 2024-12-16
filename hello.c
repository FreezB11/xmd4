#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yash");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
    printk(KERN_ALERT "Hello form Module\n"); // <-- this shows the error in the dmesg log
    printk(KERN_INFO "the process is \"%s\" (pid %i)\n",current->comm, current->pid);
    return 0;
}

static void __exit hello_exit(void){
    printk(KERN_INFO "Module Exited\n");
}

module_init(hello_init);
module_exit(hello_exit);
