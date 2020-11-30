#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/gpio.h>

#define DEVICE_NAME "modx"
#define MAJOR_NUM 42  // as this is an answer to everything

static struct class *modx_class;

// GPIO 
static unsigned int gpioLED = 21;       ///<hard coding the LED gpio for this example 
static bool	    ledOn = 0;          ///< used to invert state


static int modx_open(struct inode *inode, struct file *file)
{
	pr_info("%s\n", __func__);
	return 0;
}

static int modx_release(struct inode *inode, struct file *file)
{
	pr_info("%s\n", __func__);
	return 0;
}

static ssize_t modx_read(struct file *file,
			char *buffer, size_t length, loff_t * offset)
{
	pr_info("%s %u\n", __func__, length);
	return 0;
}

static ssize_t modx_write(struct file *file,
			 const char *buffer, size_t length, loff_t * offset)
{
	pr_info("%s %u\n", __func__, length);
	return length;
}

struct file_operations modx_fops = {
	.owner = THIS_MODULE,
	.open = modx_open,
	.release = modx_release,
	.read = modx_read,
	.write = modx_write,
};

int __init modx_init(void)
{
	int ret;

	printk("Module X loaded\n");
	ret = register_chrdev(MAJOR_NUM, DEVICE_NAME, &modx_fops);
	if (ret != 0)
		return ret;

	modx_class = class_create(THIS_MODULE, DEVICE_NAME);

	device_create(modx_class, NULL, MKDEV(MAJOR_NUM, 0), NULL, "modx%d", 0);

    // Is the GPIO a valid GPIO number
    if (!gpio_is_valid(gpioLED)){
        printk(KERN_INFO "GPIO_TEST: invalid LED GPIO\n");
        return -ENODEV;   
    }

    ledOn = true;
    gpio_request(gpioLED, "sysfs");          
    gpio_direction_output(gpioLED, ledOn);   
    gpio_export(gpioLED, false);      

	return 0;
}

void __exit modx_exit(void)
{
	device_destroy(modx_class, MKDEV(MAJOR_NUM, 0));
	class_destroy(modx_class);

	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    gpio_set_value(gpioLED, 0);              // Turn the LED off
    gpio_unexport(gpioLED);                  // Unexport the LED GPIO
    gpio_free(gpioLED);                      // Free the LED GPIO
    printk(KERN_INFO "Module X going down\n");
}

module_init(modx_init);
module_exit(modx_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karol Przybylski");
MODULE_DESCRIPTION("A simple GPIO driver");