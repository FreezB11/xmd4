//usb_driver

#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>

// probe function this function is called when the device in plugged
static int my_probe(struct usb_interface * interface, const struct usb_device_id *id){
    printk(KERN_INFO "[*] device (%04X:%04X) plugged \n",id->idVendor, id-> idProduct);
    return 0;
}

//disconnect
static void my_disconnect(struct usb_interface *interface){
    printk(KERN_INFO "[*] device removed\n");
}

// usb device id
static struct usb_device_id my_table[] = {
    //258a:002a
    { USB_DEVICE(0x258a,0x002a)},
    {},
};
MODULE_DEVICE_TABLE(usb,my_table);

// usb driver
static struct usb_driver my_usb = {
    .name = "MY USB Driver",
    .id_table = my_table, // usb_device_id
    .probe = my_probe,
    .disconnect = my_disconnect,
};

static int __init usb_entry(void){
    int ret = -1;
    printk(KERN_INFO "DRIVER started\n");
    printk(KERN_INFO "registering drivera\n");
    ret = usb_register(&my_usb);
    printk(KERN_INFO "registration complete\n");
    return ret;
}

static void __exit usb_exit(void){
    printk(KERN_INFO "closing driver");
    usb_deregister(&my_usb);
    printk(KERN_INFO "deregistering complete");
}

module_init(usb_entry);
module_exit(usb_exit);

MODULE_LICENSE("GPL");


/*
    REFERENCE:= https://www.youtube.com/watch?v=5IDL070RtoQ&list=PLM8zRjaI08aQKKdUIqObqLTp4o5A67pOy&index=2
*/