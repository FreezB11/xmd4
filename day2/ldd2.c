#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

static struct proc_dir_entry *my_proc;
struct proc_ops my_ops = {

};

static int __init mod_init(void){
    printk(KERN_INFO "Entring into the mod");
    my_proc = proc_create("myproc",0,NULL,&my_ops);
    printk(KERN_INFO "proc made");

    return 0;
}

static void __exit mod_exit(void){
    printk(KERN_INFO "Closing the mod");
    proc_remove(my_proc);
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HS4Y");
MODULE_DESCRIPTION("learning readable kernel module");
MODULE_VERSION("0.9");