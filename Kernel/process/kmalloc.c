#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>		// 内存分配函数

MODULE_LICENSE("GPL");

static void test(void *data)
{
	int *count = (int *)data;
	printk("test count = %d\n", *count);
	kfree(count);
	printk("free over");
}
static int pthread_begin(void)
{
	int *count = NULL;
	count = kmalloc(sizeof(*count), GFP_KERNEL);
	if(count == NULL)
	{
		printk(KERN_ALERT "kmalloc failed\n");
		return -1;
	}
	*count = 10;
	test((void *)count);
	return 0;
}
static void myfree(void)
{
	printk("exit\n");
}

module_init(pthread_begin);
module_exit(myfree);
