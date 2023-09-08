#include <linux/module.h>
#include <linux/init.h>
#include <asm/bitops.h>
// 版本声明
MODULE_LICENSE("Dual BSD/GPL");

static char *target = "world";
static int age=20;
module_param(target, charp, S_IRUGO);
module_param(age, int, S_IRUGO);
// 初始化模块
static int __init helloworld_init(void)
{
	unsigned long word = 0;

	set_bit(0, &word);		// 第0位被设置（原子地）
	set_bit(1, &word);		// 第1位被设置（原子地）
	printk("%lu\n", word);	// 打印3
	printk("bit: %lu\n", find_first_zero_bit(&word, 5));	// 搜索5个位，返回第一个没被设置的位号
	printk("bit: %lu\n", find_first_bit(&word, 5));			// 搜索5个位，返回第一个被设置的位号

	clear_bit(1, &word);	// 清空第1位
	printk("%lu\n", word);	// 打印3
	change_bit(0, &word);	// 翻转第0位的值，这里它被清空
	printk("%lu\n", word);	// 打印3

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

