/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... file system*/
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <asm/gpio.h>
#include <asm/arch/pxa-regs.h>
#include <linux/interrupt.h>

MODULE_LICENSE("Dual BSD/GPL");

// gpio input pins
#define PIN0		29
#define PIN1		101
#define PIN2		117
//#define PIN3		30	// can also try 17, 31

// notes
static char note1 = 'C';
static char note2 = 'D';
static char note3 = 'E';
static char note4 = 'F';
//static char note5 = 'A';

// currently playing note
static char current_note = 'X';

// general
#define HIGH	1
#define LOW	0

/* Buffer to store data */
static char *mygpio_buffer;
/* length of the current message */
static int mygpio_len;

// interrupt vars
static unsigned int pin0_irq, pin1_irq, pin2_irq; //pin3_irq;
static irq_handler_t pin0_wrapper(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t pin1_wrapper(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t pin2_wrapper(unsigned int irq, void *dev_id, struct pt_regs *regs);
//static irq_handler_t btn3_wrapper(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t pin_handler(void);

// INTERRUPT FUNCTIONS
// actual handler (used by all pins)
static irq_handler_t pin_handler(void){
	// if only PIN0 is high	
	if (!gpio_get_value(PIN2) && !gpio_get_value(PIN1) && gpio_get_value(PIN0)){
		current_note = note1;
	}
	// if only PIN1 is high	
	if (!gpio_get_value(PIN2) && gpio_get_value(PIN1) && !gpio_get_value(PIN0)){
		current_note = note2;
	}
	// if only PIN0 and PIN1 are high	
	if (!gpio_get_value(PIN2) && gpio_get_value(PIN1) && gpio_get_value(PIN0)){
		current_note = note3;
	}
	// if only PIN2 is high	
	if (gpio_get_value(PIN2) && !gpio_get_value(PIN1) && !gpio_get_value(PIN0)){
		current_note = note4;
	}
	// if only PIN2 and PIN0 are high	
	//if (gpio_get_value(PIN2) && !gpio_get_value(PIN1) && gpio_get_value(PIN0)){
	//	current_note = note5;
	//}

	return (irq_handler_t) IRQ_HANDLED;
}

// wrapper for irq interrupt to PIN0
static irq_handler_t pin0_wrapper(unsigned int irq, void *dev_id, struct pt_regs *regs){
	return pin_handler();
}

// wrapper for irq interrupt to PIN1
static irq_handler_t pin1_wrapper(unsigned int irq, void *dev_id, struct pt_regs *regs){
	return pin_handler();
}

// wrapper for irq interrupt to PIN2
static irq_handler_t pin2_wrapper(unsigned int irq, void *dev_id, struct pt_regs *regs){
	return pin_handler();
}

// wrapper for irq interrupt to PIN3
//static irq_handler_t pin3_wrapper(unsigned int irq, void *dev_id, struct pt_regs *regs){
//	return pin_handler();
//}

/* Declaration of gpio functions */
static int mygpio_open(struct inode *inode, struct file *filp);
static int mygpio_release(struct inode *inode, struct file *filp);
static ssize_t mygpio_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static ssize_t mygpio_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static void mygpio_exit(void);
static int mygpio_init(void);

/* Structure that declares the usual file */
/* access functions */
struct file_operations mygpio_fops = {
	open: mygpio_open,
	release: mygpio_release,
	write: mygpio_write,
	read: mygpio_read,
};

/* Declaration of the init and exit functions */
module_init(mygpio_init);
module_exit(mygpio_exit);

// read/write vars
static unsigned capacity = 128;
static unsigned bite = 128;
module_param(capacity, uint, S_IRUGO);
module_param(bite, uint, S_IRUGO);

/* Global variables of the driver */
/* Major number */
static int mygpio_major = 63;

static int mygpio_init(void)
{
	int result;
	
	// set GPIO pins as inputs
	pxa_gpio_mode(PIN0 | GPIO_IN);
	pxa_gpio_mode(PIN1 | GPIO_IN);
	pxa_gpio_mode(PIN2 | GPIO_IN);
	//pxa_gpio_mode(PIN3 | GPIO_IN);

	/* Registering memory */
	result = register_chrdev(mygpio_major, "mygpio", &mygpio_fops);
	if (result < 0)
	{
		printk(KERN_ALERT
			"mygpio: cannot obtain major number %d\n", mygpio_major);
		return result;
	}

	/* Allocating gpio for the buffer */
	mygpio_buffer = kmalloc(capacity, GFP_KERNEL); 
	if (!mygpio_buffer)
	{ 
		printk(KERN_ALERT "Insufficient kernel memory\n"); 
		result = -ENOMEM;
		goto fail; 
	} 
	memset(mygpio_buffer, 0, capacity);
	mygpio_len = 0;

	//printk(KERN_ALERT "Inserting gpio module\n");


	// interrupt stuff
	pin0_irq = gpio_to_irq(PIN0);
	pin1_irq = gpio_to_irq(PIN1);
	pin2_irq = gpio_to_irq(PIN2);
	//pin3_irq = gpio_to_irq(PIN3);

	result = request_irq(pin0_irq, (irq_handler_t) pin0_wrapper,
			    IRQF_TRIGGER_RISING, "pin0_wrapper", NULL);
	if (result < 0)
	{
		printk(KERN_ALERT
			"mygpio: cannot obtain irq for PIN0\n");
		goto fail;
	}

	result = request_irq(pin1_irq, (irq_handler_t) pin1_wrapper,
			    IRQF_TRIGGER_RISING, "pin1_wrapper", NULL);
	if (result < 0)
	{
		printk(KERN_ALERT
			"mygpio: cannot obtain irq for PIN1\n");
		goto fail;
	}

	result = request_irq(pin2_irq, (irq_handler_t) pin2_wrapper,
			    IRQF_TRIGGER_RISING, "pin2_wrapper", NULL);
	if (result < 0)
	{
		printk(KERN_ALERT
			"mygpio: cannot obtain irq for PIN2\n");
		goto fail;
	}

	/*
	result = request_irq(pin3_irq, (irq_handler_t) pin3_wrapper,
			    IRQF_TRIGGER_RISING, "pin3_wrapper", NULL);
	if (result < 0)
	{
		printk(KERN_ALERT
			"mygpio: cannot obtain irq for PIN3\n");
		goto fail;
	}
	*/


	return 0;

fail: 
	mygpio_exit(); 
	return result;
}

static void mygpio_exit(void)
{
	/* Freeing the major number */
	unregister_chrdev(mygpio_major, "mygpio");

	/* Freeing buffer memory */
	if (mygpio_buffer)
	{
		kfree(mygpio_buffer);
	}

	// free interrupt lines
	free_irq(pin0_irq, NULL);
	free_irq(pin1_irq, NULL);
	free_irq(pin2_irq, NULL);
	//free_irq(pin3_irq, NULL);

	//printk(KERN_ALERT "Removing gpio module\n");
}

// file operation functions
static int mygpio_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int mygpio_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{

	char message[7], temp[2];
	int len;

	// initialize message
	strcpy(message, "");

	// add current_note
	sprintf(temp, "%c\0", current_note);
	strcat(message, temp);

	// clean up
	len = (int)strlen(message);
	if (*f_pos >= len)
		return 0;
	if (copy_to_user(buf, message, 7)) 
		return -EFAULT;
	*f_pos += len;

	// return
	return len;
}

static int mygpio_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

static int mygpio_release(struct inode *inode, struct file *filp)
{
	return 0;
}

