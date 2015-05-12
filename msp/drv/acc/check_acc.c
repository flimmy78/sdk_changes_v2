#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/reboot.h>

#include <mach/hardware.h>
#include <asm/io.h>

//#include "hi_common.h"
//#include "hi_module.h"

#define GPIO5_REG_BASE 0xF8004000
#define GPIO_DATA 0x3fc
#define GPIO_DIR  0x400
#define OFFDELAY  2
#define ACC_REG   3
#define TIMEOUT   1200

struct timer_list acc_timer;
static int acc_flag = 0;
static int acc_count = 0;

static void __exit acc_exit(void)
{
	printk(KERN_INFO "acc_mod exit\n");
	del_timer(&acc_timer);
}

static unsigned int set_gpio_value(unsigned int reg, int gpio, int val)
{
	unsigned int regval;
	
	regval = readl(IO_ADDRESS(reg));
	//printk("read 0xF80043fc: 0x%08x\n", regval);
	
	regval = (regval & (0x0 << gpio));
	//printk("gpio5_%d=%d\n", gpio, regval);
	
	regval = (regval | (val << gpio));
	//printk("gpio5_%d=%d\n", gpio, regval);
	writel(regval, IO_ADDRESS(reg));
}

static unsigned int get_gpio_value(unsigned int reg, int gpio)
{
	unsigned int regval;

	regval = readl(IO_ADDRESS(reg));
	//printk("read 0xF80043fc: 0x%08x\n", regval);

	regval = ((regval >> gpio) & 0x1);
	//printk("gpio5_%d=%d\n", gpio, regval);
	
	return regval;
}

void check_acc_value(void)
{
	int val = 0;
	
	val = get_gpio_value((GPIO5_REG_BASE + GPIO_DATA), ACC_REG);
	
	if (val == 0) {
		acc_flag = 0;
		acc_count = 0;
	} else if (val == 1) {
		acc_flag = 1;
		acc_count ++;
		if ((acc_count >= TIMEOUT)&&(1 == acc_flag)) {

			set_gpio_value((GPIO5_REG_BASE + GPIO_DATA), OFFDELAY, 0);
			kernel_restart(NULL);
		}
	}
	acc_timer.expires = jiffies + msecs_to_jiffies(1000);
	add_timer(&acc_timer);
}

static int __init acc_init(void)
{
	printk("acc_mod init\n");
	
	init_timer(&acc_timer);

	acc_timer.expires = jiffies + msecs_to_jiffies(5000);
	acc_timer.function = check_acc_value; 
	
	add_timer(&acc_timer);
	return 0;
}

module_init(acc_init);
module_exit(acc_exit);

MODULE_LICENSE("GPL");
