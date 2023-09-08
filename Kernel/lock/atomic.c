#include <linux/module.h>
#include <linux/init.h>
#include <asm/atomic.h>
// 版本声明
MODULE_LICENSE("Dual BSD/GPL");

static char *target = "world";
static int age=20;
module_param(target, charp, S_IRUGO);
module_param(age, int, S_IRUGO);
// 初始化模块
static int __init helloworld_init(void)
{
	atomic_t v;
	atomic_t u = ATOMIC_INIT(2);
	atomic_set(&v, 4);	// v = 4
	printk("v = %d\n", atomic_read(&v));
	printk("u = %d\n", atomic_read(&u));
	atomic_add(2, &v);	// v = v + 2 = 6
	atomic_inc(&v);		// v = v + 1 = 7
	printk("v = %d\n", atomic_read(&v));
	printk("check: %d\n", atomic_dec_and_test(&u));

	return 0;
}

// 清理模块
static void __exit helloworld_exit(void)
{
}

module_init(helloworld_init);	// 模块初始化
module_exit(helloworld_exit);	// 模块退出

// 作者，软件描述，版本等声明信息
MODULE_AUTHOR("HYQ");		// 作者声明
MODULE_DESCRIPTION("atomic demo");		// 描述声明
MODULE_VERSION("0.0.1");	// 版本
MODULE_ALIAS("Chapter 17, Example 1");		// 模块别名

