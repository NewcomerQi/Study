#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/string.h>	// 字符串处理函数头文件
#include <linux/init.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,11,0)
#include <linux/sched.h>
#else
#include <linux/sched/task.h>	// init_task
#include <linux/sched/signal.h>	// for_each_process
#endif

MODULE_LICENSE("GPL");


static int ps_begin(void)
{
	int KernelThreadCount = 0;
	int UserThreadCount = 0;
	struct task_struct *p;
	p = &init_task;
	// 遍历所有进程
	for_each_process(p)
	{
		if( p->mm == NULL )
		{
			// printk("\nKernelThread Info!\n");
			printk("comm=%s, pid=%d, state=%ld\n", p->comm, p->pid, p->state);
			++KernelThreadCount;
		}
		else
		{
			// printk("\nUserThread Info!\n");
			printk("comm=%s, pid=%d, state=%ld\n", p->comm, p->pid, p->state);
			if(strcmp(p->comm, "a.out") == 0)	// 比较进程名
			{
				struct task_struct *prev_task = NULL, *next_task = NULL;
				printk("comm=%s, pid=%d, state=%ld\n", p->comm, p->pid, p->state);
			    prev_task = list_entry(p->tasks.prev, struct task_struct, tasks);	// 在进程链表中，获取指定进程的前一个进程
				if(prev_task != NULL)
					printk("HTTPTest prev task: %s[%d] state = %ld\n", prev_task->comm,prev_task->pid, prev_task->state);
			    next_task = list_entry(p->tasks.next, struct task_struct, tasks);	// 在进程链表中，获取指定进程的下一个进程
				if(next_task != NULL)
					printk("HTTPTest next task: %s[%d] state = %ld\n", next_task->comm,next_task->pid, next_task->state);
			}
			++UserThreadCount;
		}
	}
	printk("\nThe number of kernel Thread is:%d.\n", KernelThreadCount);
	printk("\nThe number of User Thread is:%d.\n", UserThreadCount);
	return 0;
}

static void ps_end(void)
{
	printk("\nExit!\n");
}
module_init( ps_begin );
module_exit( ps_end );


