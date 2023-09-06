
#include <linux/atomic.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h>

#include <asm/errno.h>
#include <linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Allef P. Araujo");
MODULE_DESCRIPTION("My Led Driver");
MODULE_VERSION("0.1");

#define SUCCESS 0
#define DEVICE_NAME "led"   // Dev name as it appears in /dev/devices
#define BUF_LEN  4          // Max length of the message from the device

// Prototypes
static int led_open(struct inode *, struct file *);
static int led_release(struct inode *, struct file *);
static ssize_t led_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t led_write(struct file *, const char __user *, size_t, loff_t *);

static unsigned int gpioLed = 17;   // pin 11 (GPIO17)
static int major;                   // major number assigned to our device driver

enum {
	CDEV_NOT_USED = 0,
	CDEV_EXCLUSIVE_OPEN = 1,
};

// Is device open? Used to prevent multiple access to device
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static char msg[BUF_LEN + 1]; // The msg the device will give when asked

static struct class *cls;

static struct file_operations led_fops = {
	.read = led_read,
	.write = led_write,
	.open = led_open,
	.release = led_release,
};

static int set_permissions(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

// The LMK initialization function
static int __init led_driver_init(void) {

    major = register_chrdev(0, DEVICE_NAME, &led_fops);
    if (major < 0) {
        printk("LED_DRIVER: Failed to registet Led device major: %d\n", major);
        return -ENODEV;
    }

    printk("LED_DRIVER: I've got a major %d\n", major);
    cls = class_create(THIS_MODULE, DEVICE_NAME);
    cls->dev_uevent = set_permissions;
	device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    printk("LED_DRIVER: Led created on /dev/%s\n", DEVICE_NAME);

    if (!gpio_is_valid(gpioLed)) {
        printk(KERN_INFO "LED_DRIVER: invalid LED GPIO\n");
        return -ENODEV;
    }

    gpio_request(gpioLed, "led");               // request LED GPIO
    gpio_direction_output(gpioLed, 0);          // set in output mode and on

    return SUCCESS;
}

static void __exit led_driver_exit(void) {
    gpio_set_value(gpioLed, 0);                 // turn the LED off
    gpio_unexport(gpioLed);                     // unexport the LED GPIO
    gpio_free(gpioLed);                         // free the LED GPIO

    device_destroy(cls, MKDEV(major, 0));
	class_destroy(cls);

	// Unregister the device
	unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "LED_DRIVER: Goodbay from the LKM!\n");
}

// Called when a process tries to open the device file
static int led_open(struct inode *inode, struct file *file)
{
    printk("LED_DRIVER: New process opened the LED driver!\n");
	return SUCCESS;
}

// Called when a process closes the device fie.
static int led_release(struct inode *inode, struct file *file)
{
    printk("LED_DRIVER: Process closed the LED driver!\n");
	return SUCCESS;
}

// Called when a process, which already opened the dev file, attempts to
// read from it.
static ssize_t led_read(struct file *filp, // see include/linux.fs.h
				char __user *buffer, // buffer to fill with data
				size_t length, // length of the buffer
				loff_t *offset)
{
    printk("LED_DRIVER: Reading from Led driver!\n");
	// Number of bytes actually written to the buffer
	int bytes_read = 0;
	const char *msg_ptr = msg;

	if (!*(msg_ptr + *offset)) { // we are at the end of message
		*offset = 0; // reset the offset
		return 0;
	}

	msg_ptr += *offset;

	// Actually put the data into the buffer
	while (length && *msg_ptr) {
		// The buffer is in the user data segment, not the kernel
		// segment so "*" assignment won't work. We have to use 
	    // put_user which copies data from the kernel data segment to
		// the user data segment.
		put_user(*(msg_ptr++), buffer++);
		length--;
		bytes_read++;
	}

	*offset += bytes_read;

	// Most read functions return the number of bytes put into the buffer.
	return bytes_read;
}

// Called when a process writes to dev file
static ssize_t led_write(struct file *filp, const char __user *buff,
				size_t len, loff_t *off)
{
    get_user(msg[0], buff);

    int val = msg[0] - '0';

    if (val == 1 || val == 0) {
        printk("LED_DRIVER: Set gpio to %d\n", val);
        gpio_set_value(gpioLed, val);
    }

	return len;
}

module_init(led_driver_init);
module_exit(led_driver_exit);