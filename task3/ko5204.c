#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


#include <linux/io.h>
#include <linux/vmalloc.h>


static int __init pagetable_walker_init(void) {
	return 0;
}

static void __exit pagetable_walker_exit(void) {
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your-Name");
MODULE_DESCRIPTION("Simple Kernel Page Table Walker");
MODULE_VERSION("0.1");

module_init(pagetable_walker_init);
module_exit(pagetable_walker_exit);
