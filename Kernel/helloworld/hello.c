#include <linux/module.h>
#include <linux/init.h>

// 版本声明
MODULE_LICENSE("Dual BSD/GPL");

static char *target = "world";
static int age=20;
module_param(target, charp, S_IRUGO);
module_param(age, int, S_IRUGO);
// 初始化模块
static int __init helloworld_init(void)
{
	printk(KERN_ALERT "Hello %s module init\n", target);	// 打印信息
	printk(KERN_ALERT "I am %d years old\n", age);
	return 0;
}

// 清理模块
static void __exit helloworld_exit(void)
{
	printk(KERN_ALERT "Hello world module exit\n");	// 打印信息
}

module_init(helloworld_init);	// 模块初始化
module_exit(helloworld_exit);	// 模块退出

// 作者，软件描述，版本等声明信息
MODULE_AUTHOR("HYQ");		// 作者声明
MODULE_DESCRIPTION("Hello world demo");		// 描述声明
MODULE_VERSION("0.0.1");	// 版本
MODULE_ALIAS("Chapter 17, Example 1");		// 模块别名

