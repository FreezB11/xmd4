#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yash");
MODULE_DESCRIPTION("A simple linux hello driver");
MODULE_VERSION("0.1");

int irq = 12; // <-- here the value is set initially but u can change it with insmod hello.ko irq=<value>
module_param(irq, int, 0);



static int __init hello_init(void)
{
    printk(KERN_ALERT "Hello form Module\n"); // <-- this shows the error in the dmesg log
    printk(KERN_INFO "the process is \"%s\" (pid %i)\n",current->comm, current->pid);
    printk(KERN_INFO "this is worked if u can see the number %d\n",irq);
    return 0;
}

static void __exit hello_exit(void){
    printk(KERN_INFO "Module Exited\n");
}

module_init(hello_init);
module_exit(hello_exit);


