#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/gpio.h>

#define DEVICE_NAME "dummy"
#define MAJOR_NUM 42
#define NUM_DEVICES 4

static struct class *dummy_class;

// GPIO 
static unsigned int gpioLED = 21;       ///< hard coding the LED gpio for this example to P9_23 (GPIO49)
static bool	    ledOn = 0;          ///< Is the LED on or off? Used to invert its state (off by default)


static int dummy_open(struct inode *inode, struct file *file)
{
	pr_info("%s\n", __func__);
	return 0;
}

static int dummy_release(struct inode *inode, struct file *file)
{
	pr_info("%s\n", __func__);
	return 0;
}

static ssize_t dummy_read(struct file *file,
			char *buffer, size_t length, loff_t * offset)
{
	pr_info("%s %u\n", __func__, length);
	return 0;
}

static ssize_t dummy_write(struct file *file,
			 const char *buffer, size_t length, loff_t * offset)
{
	pr_info("%s %u\n", __func__, length);
	return length;
}

struct file_operations dummy_fops = {
	.owner = THIS_MODULE,
	.open = dummy_open,
	.release = dummy_release,
	.read = dummy_read,
	.write = dummy_write,
};

int __init dummy_init(void)
{
	int ret;
	int i;

	printk("Dummy loaded\n");
	ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &dummy_fops);
	if (ret != 0)
		return ret;

	dummy_class = class_create(THIS_MODULE, DEVICE_NAME);
	for (i = 0; i < NUM_DEVICES; i++) {
		device_create(dummy_class, NULL,
			      MKDEV(MAJOR_NUM, i), NULL, "dummy%d", i);
	}

    // Is the GPIO a valid GPIO number (e.g., the BBB has 4x32 but not all available)
    if (!gpio_is_valid(gpioLED)){
        printk(KERN_INFO "GPIO_TEST: invalid LED GPIO\n");
        return -ENODEV;
    }
    // Going to set up the LED. It is a GPIO in output mode and will be on by default
    ledOn = true;
    gpio_request(gpioLED, "sysfs");          // gpioLED is hardcoded to 49, request it
    gpio_direction_output(gpioLED, ledOn);   // Set the gpio to be in output mode and on
    // gpio_set_value(gpioLED, ledOn);          // Not required as set by line above (here for reference)
    gpio_export(gpioLED, false);             // Causes gpio49 to appear in /sys/class/gpio
                                // the bool argument prevents the direction from being changed
                                // the bool argument prevents the direction from being changed

	return 0;
}

void __exit dummy_exit(void)
{
	int i;

	for (i = 0; i < NUM_DEVICES; i++) {
		device_destroy(dummy_class, MKDEV(MAJOR_NUM, i));
	}
	class_destroy(dummy_class);

	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
	printk("Dummy unloaded\n");
   gpio_set_value(gpioLED, 0);              // Turn the LED off, makes it clear the device was unloaded
   gpio_unexport(gpioLED);                  // Unexport the LED GPIO
   gpio_free(gpioLED);                      // Free the LED GPIO
   printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n");
}

module_init(dummy_init);
module_exit(dummy_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chris Simmonds");
MODULE_DESCRIPTION("A dummy driver");