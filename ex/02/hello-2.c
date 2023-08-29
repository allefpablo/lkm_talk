#include <linux/init.h>     // kernel macros e.g. __init and __exit
#include <linux/module.h>   // We are writing a module
#include <linux/kernel.h>   // It is kernel work, so we need kernel data types and functions

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Allef P. Araujo"); // TODO: Change to your name
MODULE_DESCRIPTION("My Second Kernel Module");
MODULE_VERSION("1.0");

static char *name = "kernel";

// TODO: Declare parameter with module_param
// TODO: Document parameter with MODULE_PARM_DESC

// Initialiaztion function
static int __init hello_2_init(void)
{
    printk(KERN_INFO "Init: Hello %s! I'm the Hacker Man!\n", name);
    return 0;
}

static void __exit hello_2_exit(void)
{
    printk(KERN_INFO "Exit: Goodbay %s!\n", name);
}

module_init(hello_2_init);
module_exit(hello_2_exit);