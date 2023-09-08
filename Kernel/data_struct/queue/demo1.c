#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kfifo.h>
#include <linux/slab.h>		// 内存分配函数
#include <linux/string.h>

MODULE_LICENSE("GPL");

typedef struct student
{
	char m_name[20];
	int m_age;
	struct list_head node_student;
} student_t;

struct kfifo fifo;

static int init_queue(int size)
{
	int ret;
	ret = kfifo_alloc(&fifo, size, GFP_KERNEL);
	if(ret)
	{
		printk("kfifo_alloc failed; ret = %d\n", ret);
		return ret;
	}
	return ret;
}

// 入队
static int Inqueue(const char *name, int age)
{
	student_t *stu = NULL;
	int ret;
	printk("size: %ld\n", sizeof(student_t));
	stu = kmalloc(sizeof(*stu), GFP_KERNEL);
	if(stu != NULL)
	{
		strcpy(stu->m_name, name);
		stu->m_age = age;
		ret = kfifo_in(&fifo, (void *)stu, sizeof(student_t));
	}
	return ret;
}

// 出队
static int OutQeueu(void)
{
	int ret;
	student_t * stu = kmalloc(sizeof(student_t), GFP_KERNEL);
	if(stu == NULL)
		return -1;
	ret = kfifo_out(&fifo, stu, sizeof(student_t));
	if(ret > 0)
	{
		printk("name: %s; age: %d\n", stu->m_name, stu->m_age);
		kfree(stu);
	}
	
	return ret;
}

// 获取队列对头元素
static int get_data_from_queue(void)
{
	int ret;
	student_t * stu = kmalloc(sizeof(student_t), GFP_KERNEL);
	if(stu == NULL)
		return -1;
	ret = kfifo_out_peek(&fifo, stu, sizeof(student_t));
	if(ret > 0)
	{
		printk("name: %s; age: %d\n", stu->m_name, stu->m_age);
		kfree(stu);
	}
	
	return ret;
}
// 获取队列长度
static unsigned int get_queue_len(int type)
{
	// 获取队列总长度
	if(type == 1)
	{
		return kfifo_size(&fifo);
	}
	else if(type == 2)	// 获取队列中数据的长度
	{
		return kfifo_len(&fifo);
	}
	else
	{
		return kfifo_avail(&fifo);	// 获取队列可用空间
	}
	return 0;
}

// 判断队列是否为空
static int empty(void)
{
	return kfifo_is_empty(&fifo);
}

// 判断队列是否为满
static int full(void)
{
	return kfifo_is_full(&fifo);
}

// 清空队列
static void clear(void)
{
	kfifo_reset(&fifo);
}

// 判断队列是否为满
static int begin_student(void)
{
	// 动态方法
	unsigned int length;
	int ret;
	init_queue(PAGE_SIZE);
	ret = kfifo_is_empty(&fifo);
	printk("ret = %d\n", ret);
	Inqueue("zhangsan", 18);
	length = get_queue_len(1);
	printk("length: %d\n", length);
	length = get_queue_len(2);
	printk("length: %d\n", length);
	length = get_queue_len(3);
	printk("length: %d\n", length);
	get_data_from_queue();
	OutQeueu();
	return 0;
	
	// 静态方法
	/*
	DECLARE_KFIFO(student_que, struct kfifo, PAGE_SIZE);
	INIT_KFIFO(student_que);
	return 0;
	*/
}

static void exit_student(void)
{
	// 撤销队列
	kfifo_free(&fifo);
}

module_init(begin_student);
module_exit(exit_student);
