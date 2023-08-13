#include <linux/init.h>     // kernel macros e.g. __init and __exit
#include <linux/module.h>   // We are writing a module
#include <linux/kernel.h>   // It is kernel work, so we need kernel data types and functions

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Allef P. Araujo");
MODULE_DESCRIPTION("Basic Kernel Module");
MODULE_VERSION("1.0");

static char *name = "kernel";

module_param(name, charp, S_IRUGO); // S_IRUGO can be read/not changed, charp means char pointer
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");

// Initialiaztion function
static int __init hello_init(void)
{
    printk(KERN_INFO "Init: Hello %s from my custom module!\n", name);
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Exit: Goodbay %s from my custom module!\n", name);
}

module_init(hello_init);
module_exit(hello_exit);