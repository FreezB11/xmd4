#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yash");
MODULE_VERSION("0.1");

int hello_init(void)
{
    printk(KERN_INFO "HELLO WORLD\n");
    return 0;
}

void hello_exit(void){
    printk(KERN_INFO "good bye world\n");
}

module_init(hello_init);
module_exit(hello_exit);
