#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>

/* Static and local Variables */
static char * name = "Brian I";
static unsigned long period_msec = 500; // timer period in msec
static int count = 0;

static struct timer_list name_timer;

/* Define modules parameters */
module_param(name, charp, S_IRUGO);
module_param(period_msec, ulong, S_IRUGO);

void name_timer_callback(unsigned long data);

/* ------------------------------------------------------------- */
/* Methods - kernel module */
static int __init name_timer_init(void)
{
  printk(KERN_ALERT "Installing name_timer module with name {%s} and timer period of {%ld} msec", 
         name, period_msec);

  // Setup timer
  setup_timer(&name_timer, name_timer_callback, 0);

  // Make initial timer call
  mod_timer(&name_timer, jiffies + msecs_to_jiffies(period_msec));

  return 0;
}

static void __exit name_timer_exit(void)
{
  del_timer(&name_timer);
  printk("Uninstalled name_timer module\n");
}

/* ------------------------------------------------------------- */
/* Methods - Timer */
void name_timer_callback(unsigned long data)
{
  count++; // Increment count for timer call
  printk(KERN_ALERT "name_timer() called for user {%s} with count {%d}", name, count);

  // Need to trigger call for next timer execution
  mod_timer(&name_timer, jiffies + msecs_to_jiffies(period_msec));

  return;
}

/* ------------------------------------------------------------- */
/* Module Initialization */
module_init(name_timer_init);
module_exit(name_timer_exit);
MODULE_DESCRIPTION("Kernel timer that periodically prints to the kernel log the provided name and trigger count.");
MODULE_AUTHOR("Brian I.");
MODULE_LICENSE("GPL");
