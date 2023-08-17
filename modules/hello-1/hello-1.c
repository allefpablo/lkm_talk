#include <linux/init.h>     // kernel macros e.g. __init and __exit
#include <linux/module.h>   // We are writing a module
#include <linux/kernel.h>   // It is kernel work, so we need kernel data types and functions

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Allef P. Araujo");
MODULE_DESCRIPTION("My First Kernel Module");
MODULE_VERSION("1.0");

// Initialiaztion function
static int __init hello_1_init(void)
{
    printk(KERN_INFO "Init: Hello Kernel World!\n");
    return 0;
}

static void __exit hello_1_exit(void)
{
    printk(KERN_INFO "Exit: Goodbay Kernel!\n");
}

module_init(hello_1_init);
module_exit(hello_1_exit);