#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>	// 线程函数
#include <linux/delay.h>	// 延迟函数
#include <linux/slab.h>		// 内存分配函数
MODULE_LICENSE("GPL");

static struct task_struct *task = NULL;
static int my_pthread(void *data)
{
	int count_ = 0;
	int n = 0;
	if(data != NULL)
	{
		int *count = (int *)data;
		count_ = *count;
		kfree(count);	// 释放内存
	}

	while(n < 10)
	{
		printk("count = %d\n", count_++);
		n++;
        ssleep(1);	// 睡眠1秒
		if(kthread_should_stop())
		{
			break;
		}
	}
	printk("thread over!");
	return count_;
}
static int pthread_begin(void)
{
	int *count = NULL;
	char name[20] = "thread %s";
	count = kmalloc(sizeof(*count), GFP_KERNEL);
	if(count == NULL)
	{
		printk(KERN_ALERT "kmalloc failed\n");
		return -1;
	}
	*count = 10;

	printk("main process begin");
	// task = kthread_create(my_pthread, NULL, "thread test");	// 处于不可运行状态，需要调用wake_up_process()来唤醒
	task = kthread_run(my_pthread, (void *)count, name, "test");	// 会同时调用kthread_create()和wake_up_process()
	if(!IS_ERR(task))
	{
		printk("thread create success\n");
		// wake_up_process(task);
	}
	else
	{
		printk("thread create failed\n");
	}
	printk("main process over\n");
	return 0;
}

static void pthread_end(void)
{
	printk("main exit\n");
	if(task != NULL)
	{
		printk("task state = %ld\n", task->state);
		if(task->state == TASK_UNINTERRUPTIBLE)		// 线程还在运行
		{
			kthread_stop(task);
		}
	}
	else
	{
		printk("task is NULL\n");
	}
}

module_init(pthread_begin);
module_exit(pthread_end);
