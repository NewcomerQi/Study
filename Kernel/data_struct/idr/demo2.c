#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>		// 内存分配函数
#include <linux/string.h>
#include <linux/idr.h>

MODULE_LICENSE("GPL");

typedef struct student
{
	char m_name[20];
	int m_age;
	struct list_head node_student;
} student_t;

struct idr idr_stu;
// 打印idr成员
static int print_idr(int id, void *p, void *data)
{
	student_t *stu = (student_t *)p;
	if(stu)
		printk("id: %d; name: %s; age: %d\n", id, stu->m_name, stu->m_age);

	return 0;
}
// 获取UID映射的地址
static student_t* get_stu(int id)
{
	student_t *stu = idr_find(&idr_stu, id);
	return stu;
}
// 删除idr成员
static int remove(int id, void *p, void *data)
{
	student_t *stu = (student_t *)p;
	printk("erase %s\n", stu->m_name);
	idr_remove(&idr_stu, id);
	return 0;
}
// 分配一个新的UID
static int Insert(const char *name, int age)
{
	student_t *stu = NULL;
	int ret, next_id = 10;
	stu = kmalloc(sizeof(student_t), GFP_KERNEL);
	if(stu != NULL)
	{
		strcpy(stu->m_name, name);
		stu->m_age = age;
	}
	idr_preload(GFP_KERNEL);	// 调用idr_alloc之前执行
	do 
	{
		// ret = idr_alloc_cyclic(&idr_stu, (void *)stu, 0, next_id, GFP_KERNEL);	// 按顺序往后分配，即使前面有UID被移除了
		ret = idr_alloc(&idr_stu, (void *)stu, 0, next_id, GFP_KERNEL);	// 分配最小的UID
		if(ret == -ENOMEM)	// 内存不够
		{
			idr_preload_end();		// 调用完成idr_alloc之后执行
			return ret;
		}
		next_id++;
	}while(ret == -ENOSPC);	// 空间不够
	idr_preload_end();		// 调用完成idr_alloc之后执行
	return ret;
}

// 查找UID
static int count(int id)
{
	student_t *stu = idr_find(&idr_stu, id);
	if(!stu)
	{
		return -EINVAL;		// 错误
	}
	return 1;
}


// 删除UID
static void erase(int id)
{
	idr_remove(&idr_stu, id);
}

// 释放所有UID
static void remove_all(void)
{
	idr_for_each(&idr_stu, remove, NULL);
}
static int begin_student(void)
{
	student_t *stu = NULL;
	int test_id = 1;
	int array[5] = {0};
	// 初始化idr结构
	idr_init(&idr_stu);
	array[0] = Insert("zhangsan", 20);
	printk("zhangsan id = %d", array[0]);

	array[1] = Insert("lisi", 21);
	printk("lisi id = %d", array[1]);

	array[2] = Insert("wangwu", 22);
	printk("wangwu id = %d", array[2]);

	array[3] = Insert("zhaoliu", 23);
	printk("zhaoliu id = %d", array[3]);

	idr_remove(&idr_stu, array[2]);
	array[4] = Insert("hanmeimei", 24);
	printk("hanmeimei id = %d", array[4]);
	
	// 遍历idr所有成员
	idr_for_each(&idr_stu, print_idr, NULL);

	stu = idr_get_next(&idr_stu, &test_id);
	if(stu != NULL)
	{
		printk("%d next name: %s\n", test_id, stu->m_name);
	}
	idr_replace(&idr_stu,(void *)stu, array[0]);
	// 移除idr所有成员
	remove_all();
	test_id = Insert("zhangwu", 200);
	printk("zhangwu: %d\n", test_id);

	return 0;
}

static void exit_student(void)
{
	// 强制删除所有的UID
	remove_all();
	// 撤销idr
	idr_destroy(&idr_stu);
}

module_init(begin_student);
module_exit(exit_student);
