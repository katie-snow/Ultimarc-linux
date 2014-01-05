/*
 * ultimarc.c
 *
 *  Created on: Jan 5, 2014
 *      Author: rob
 */

#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert Abram, Katie Snow");
MODULE_DESCRIPTION("Ultimarc USB tools");


static int __init mymodule_init(void)
{
 printk ("My module worked!\n");
        return 0;
}

static void __exit mymodule_exit(void)
{
 printk ("Unloading my module.\n");
        return;
}

module_init(mymodule_init);
module_exit(mymodule_exit);





