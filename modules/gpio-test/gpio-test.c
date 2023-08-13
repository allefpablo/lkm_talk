#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>    // for the IRQ code

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Derek Molloy");
MODULE_DESCRIPTION("A Button/Led test driver for the RPi");
MODULE_VERSION("0.1");

static unsigned int gpioLed = 17;           // pin 11 (GPIO17)
static unsigned int gpioButton = 27;        // pin 13 (GPIO27)
static unsigned int irqNumber;              // share IRQ num within file
static unsigned int numberPressess = 0;     // store number of presses
static bool ledOn = 0;                      // used to invert state of LED

// prototype for the custom IRQ handler function
static irq_handler_t erpi_gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

// The LMK initialization function
static int __init erpi_gpio_init(void) {
    int result = 0;
    printk(KERN_INFO "GPIO_TEST: Initializing the GPIO_TEST LKM\n");
    if (!gpio_is_valid(gpioLed)) {
        printk(KERN_INFO "GPIO_TEST: invalid LED GPIO\n");
        return -ENODEV;
    }

    ledOn = true;
    gpio_request(gpioLed, "sysfs");             // request LED GPIO
    gpio_direction_output(gpioLed, ledOn);      // set in output mode and on
    gpio_export(gpioLed, false);                // appears in /sys/class/gpio
                                                // false prevents in/out change

    gpio_request(gpioButton, "sysfs");          // set up gpioButton
    gpio_direction_input(gpioButton);           // set up as input
    gpio_set_debounce(gpioButton, 200);         // debounce delay of 200ms
    gpio_export(gpioButton, false);             // appears in /sys/class/gpio

    printk(KERN_INFO "GPIO_TEST: button value is currently: %d\n", gpio_get_value(gpioButton));
    irqNumber = gpio_to_irq(gpioButton);        // map GPIO to IRQ number
    printk(KERN_INFO "GPIO_TEST: button mapped to IRQ: %d\n", irqNumber);

    // This next call requests an interrupt line
    result = request_irq(irqNumber,                                 // interrupt number requested
                            (irq_handler_t) erpi_gpio_irq_handler,  // handler function
                            IRQF_TRIGGER_RISING,                    // on rising edge (press, not release)
                            "erpi_gpio_handler",                    // used in /proc/interrupts
                            NULL);                                  // *dev_id for shared interrupt lines
    
    printk(KERN_INFO "GPIO_TEST: IRQ request result is: %d\n", result);
    return result;
}

static void __exit erpi_gpio_exit(void) {
    printk(KERN_INFO "GPIO_TEST: button value is currently: %d\n", gpio_get_value(gpioButton));
    printk(KERN_INFO "GPIO_TEST: pressed %d times\n", numberPressess);
    gpio_set_value(gpioLed, 0);                 // turn the LED off
    gpio_unexport(gpioLed);                     // unexport the LED GPIO
    free_irq(irqNumber, NULL);                  // free the IRQ number, no *dev_id
    gpio_unexport(gpioButton);                  // unexport the button GPIO
    gpio_free(gpioLed);                         // free the LED GPIO
    gpio_free(gpioButton);                      // free the button GPIO
    printk(KERN_INFO "GPIO_TEST: Goodbay from the LKM!\n");
}

static irq_handler_t erpi_gpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
    ledOn = !ledOn;                             // invert the LED state
    gpio_set_value(gpioLed, ledOn);             // set LED accordingly
    printk(KERN_INFO "GPIO_TEST: Interrupt! (button is %d)\n", gpio_get_value(gpioButton));
    numberPressess++;                           // global counter
    return (irq_handler_t) IRQ_HANDLED;         // anounce IRQ handled
}

module_init(erpi_gpio_init);
module_exit(erpi_gpio_exit);