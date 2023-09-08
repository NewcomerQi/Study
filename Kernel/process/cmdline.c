#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/sched/signal.h>	// for_each_process
#include <linux/string.h>
#define PARM_LENTH 256

static int (*get_cmdline_fn)(struct task_struct *task, char *buffer, int buflen);

typedef int (*pfn_get_cmdline)(struct task_struct *task, char *buffer,int buflen);
/* 将获取的buffer中的 ‘\0’替换成空格 */
static void deal_raw_cmdline(char *buffer, unsigned int length)
{
        int i = 0;
        for (i = 0; i < length; i ++) {
                if (buffer[i] == '\0') {
                        buffer[i] = ' ';
                }
        }
}

static int __init query_process_param_init(void)
{
	int ret = 0;
	struct task_struct *tsk = NULL;
	char buffer[PARM_LENTH] = {0};
	/* 采用直接赋值的方法获取函数地址 */
	// get_cmdline_fn = (int (*)(struct task_struct *, char *, int))0x0000000000000000;
	get_cmdline_fn = (pfn_get_cmdline)kallsyms_lookup_name("get_cmdline");
	if(get_cmdline_fn)
	{
		rcu_read_lock();
		for_each_process(tsk) {
			printk("pid -> %d comm -> %s\n", tsk->pid, tsk->comm);
			if (tsk->mm == NULL) {
				continue;
			}

			memset(buffer, 0, sizeof(buffer));
			ret = get_cmdline_fn(tsk, buffer, sizeof(buffer));
			if (ret < 0) {
				continue;
			}
			deal_raw_cmdline(buffer, sizeof(buffer));
			printk("cmdline: %s\n", buffer);
		}
		rcu_read_unlock();
	}

	return 0;
}

static void __exit query_process_param_exit(void)
{
	printk("Query process param exit!\n");
}

module_init(query_process_param_init);
module_exit(query_process_param_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("curtis");

