#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/cdev.h>

#include <asm/siginfo.h>
#include <linux/pid.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h>
#include <linux/pid_namespace.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

// 中断号
#define IRQ_NUM			1

// 定义驱动程序的 ID，在中断处理函数中用来判断是否需要处理	
#define IRQ_DRIVER_ID	1234

// 设备名称
#define MYDEV_NAME		"mydev"

// 驱动程序数据结构
struct myirq
{
    int devid;
};
 
// 工作者线程
typedef struct my_struct
{
	unsigned char m_key_code;
	struct work_struct my_work;
} my_struct_t;

struct myirq mydev  ={ IRQ_DRIVER_ID };

#define KBD_DATA_REG        0x60  
#define KBD_STATUS_REG      0x64
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80


struct workqueue_struct *my_wq = NULL;
my_struct_t i_my_struct;

// 设备类
static struct class *my_class;

// 用来保存设备
struct cdev my_cdev;

// 用来保存设备号
int mydev_major = 0;
int mydev_minor = 0;

// 用来保存向谁发送信号，应用程序通过 ioctl 把自己的进程 ID 设置进来。
static int g_pid = 0;


// 用来发送信号给应用程序
static void send_signal(int sig_no, unsigned char key_code)
{
	int ret;
	struct siginfo info;
	struct task_struct *my_task = NULL;
	if (0 == g_pid)
	{
		// 说明应用程序没有设置自己的 PID
	    printk("pid[%d] is not valid! \n", g_pid);
	    return;
	}

	printk("send signal %d to pid %d \n", sig_no, g_pid);

	// 构造信号结构体
	memset(&info, 0, sizeof(struct siginfo));
	info.si_signo = sig_no;
	info.si_errno = 100;
	info.si_code = key_code;

	// 获取自己的任务信息，使用的是 RCU 锁
	rcu_read_lock();
	my_task = pid_task(find_vpid(g_pid), PIDTYPE_PID);
	rcu_read_unlock();

	if (my_task == NULL)
	{
	    printk("get pid_task failed! \n");
	    return;
	}

	// 发送信号
	ret = send_sig_info(sig_no, &info, my_task);
	if (ret < 0) 
	{
	       printk("send signal failed! \n");
	}
}
// 中断下半部工作
static void work_handler(struct work_struct *work)
{
	my_struct_t *my_key = container_of(work, my_struct_t, my_work);
	printk("Ths is work_handler func\n");
	printk("key_code: %d\n", my_key->m_key_code);
	send_signal(SIGUSR1, my_key->m_key_code);
}

//中断处理函数
static irqreturn_t myirq_handler(int irq, void * dev)
{
    struct myirq mydev;
    unsigned char key_code;
    mydev = *(struct myirq*)dev;	
	
	// 检查设备 id，只有当相等的时候才需要处理
	if (IRQ_DRIVER_ID == mydev.devid)
	{
		// 读取键盘扫描码
		key_code = inb(KBD_DATA_REG);
		i_my_struct.m_key_code = key_code;
		queue_work(my_wq, &i_my_struct.my_work);
		printk("in myirq_handler, key_code: %d\n", key_code);
	}	

	return IRQ_HANDLED;
}

// 驱动模块初始化函数
static void myirq_init(void)
{
    printk("myirq_init is called. \n");

	// 注册中断处理函数
    if(request_irq(IRQ_NUM, myirq_handler, IRQF_SHARED, MYDEV_NAME, &mydev)!=0)
    {
        printk("register irq[%d] handler failed. \n", IRQ_NUM);
        return -1;
    }

    printk("register irq[%d] handler success. \n", IRQ_NUM);
}

// 当应用程序打开设备的时候被调用
static int mydev_open(struct inode *inode, struct file *file)
{
	
	printk("mydev_open is called. \n");
	return 0;	
}

static long mydev_ioctl(struct file* file, unsigned int cmd, unsigned long arg)
{
	void __user *pArg;
	printk("mydev_ioctl is called. cmd = %d \n", cmd);
	if (100 == cmd)
	{
		// 说明应用程序设置进程的 PID 
		pArg = (void *)arg;
		if (!access_ok(VERIFY_READ, pArg, sizeof(int)))
		{
		    printk("access failed! \n");
		    return -EACCES;
		}

		// 把用户空间的数据复制到内核空间
		if (copy_from_user(&g_pid, pArg, sizeof(int)))
		{
		    printk("copy_from_user failed! \n");
		    return -EFAULT;
		}
	}

	return 0;
}

static const struct file_operations mydev_ops={
	.owner = THIS_MODULE,
	.open  = mydev_open,
	.unlocked_ioctl = mydev_ioctl
};

static int __init mydev_driver_init(void)
{
	int devno;
	dev_t num_dev;

	printk("mydev_driver_init is called. \n");

	// 注册中断处理函数
    if(request_irq(IRQ_NUM, myirq_handler, IRQF_SHARED, MYDEV_NAME, &mydev)!=0)
    {
        printk("register irq[%d] handler failed. \n", IRQ_NUM);
        return -1;
    }

	// 动态申请设备号(严谨点的话，应该检查函数返回值)
	alloc_chrdev_region(&num_dev, mydev_minor, 1, MYDEV_NAME);

	// 获取主设备号
	mydev_major = MAJOR(num_dev);
	printk("mydev_major = %d. \n", mydev_major);

	// 创建设备类
	my_class = class_create(THIS_MODULE, MYDEV_NAME);

	// 创建设备节点
	devno = MKDEV(mydev_major, mydev_minor);
	
	// 初始化cdev结构
	cdev_init(&my_cdev, &mydev_ops);

	// 注册字符设备
	cdev_add(&my_cdev, devno, 1);

	// 创建设备节点
	device_create(my_class, NULL, devno, NULL, MYDEV_NAME);

	// 初始化工作者线程
	my_wq = create_workqueue("my work queue");
	INIT_WORK(&(i_my_struct.my_work), work_handler);

	return 0;
}

static void __exit mydev_driver_exit(void)
{	
	printk("mydev_driver_exit is called. \n");

	// 删除设备节点
	cdev_del(&my_cdev);
	device_destroy(my_class, MKDEV(mydev_major, mydev_minor));

	// 释放设备类
	class_destroy(my_class);

	// 注销设备号
	unregister_chrdev_region(MKDEV(mydev_major, mydev_minor), 1);

	// 注销中断处理函数
	free_irq(IRQ_NUM, &mydev);

	// 取消任何与work_struct相关的挂起工作
	cancel_work_sync(&i_my_struct.my_work);
	// 刷新工作者线程
	flush_workqueue(my_wq);
	destroy_workqueue(my_wq);
}

MODULE_LICENSE("GPL");
module_init(mydev_driver_init);
module_exit(mydev_driver_exit);
