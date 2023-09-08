#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>		// 内存分配函数
#include <linux/string.h>

MODULE_LICENSE("GPL");
typedef struct student
{
	char m_name[20];
	int m_age;
	struct list_head node_student;
} student_t;


LIST_HEAD(student_list);	// 初始化链表头
LIST_HEAD(student_list2);	// 初始化链表头

// struct list_head student_list;
// 头增
static void add_student_from_head(struct list_head *head, const char *name, int age)
{
	student_t *stu = NULL;
	stu = kmalloc(sizeof(*stu), GFP_KERNEL);
	if(stu != NULL)
	{
		strcpy(stu->m_name, name);
		stu->m_age = age;
		list_add(&stu->node_student, head);	// 头增
		printk("Head add %s success\n", name);
	}

}
// 尾增
static void add_student_from_tail(struct list_head *head, const char *name, int age)
{
	student_t *stu = NULL;
	stu = kmalloc(sizeof(*stu), GFP_KERNEL);
	if(stu != NULL)
	{
		strcpy(stu->m_name, name);
		stu->m_age = age;
		list_add_tail(&stu->node_student, head);	// 尾增
		printk("Tail add %s success\n", name);
	}

}
// 正向遍历
static void show_all_student(struct list_head *head)
{
	student_t * stu = NULL;
	
	/*
	 *方法一
	struct list_head *p;
	list_for_each(p, head)
	{
		stu = list_entry(p, struct student, node_student);
		printk("name: %s; age: %d\n", stu->m_name, stu->m_age);
	}
	*/
	// 方法二
	list_for_each_entry(stu, head, node_student)
	{
		printk("name: %s; age: %d\n", stu->m_name, stu->m_age);
	}
}

// 反向遍历
static void show_all_student_reverse(struct list_head *head)
{
	student_t * stu = NULL;
	list_for_each_entry_reverse(stu, head, node_student)
	{
		printk("name: %s; age: %d\n", stu->m_name, stu->m_age);
	}
}


// 遍历并删除
static void destroy(void)
{
	student_t *watch, *next;
	// 判断链表是否为空
	if(list_empty(&student_list) == 0)
	{
		printk("student list is not empty\n");
	}
	else
	{
		printk("student list is empty\n");
		return;
	}

	if(list_empty(&student_list2) == 0)
	{
		printk("student list2 is not empty\n");
	}
	else
	{
		printk("student list2 is empty\n");
		return;
	}
	list_for_each_entry_safe(watch, next, &student_list, node_student)
	{
		printk("delete name: %s; age: %d\n", watch->m_name, watch->m_age);
		kfree(watch);
	}

	list_for_each_entry_safe(watch, next, &student_list2, node_student)
	{
		printk("delete name: %s; age: %d\n", watch->m_name, watch->m_age);
		kfree(watch);
	}
}

// 查找节点
static student_t *find_student(struct list_head * head, const char *name)
{
	struct list_head *p;
	student_t * stu = NULL;
	list_for_each(p, head)
	{
		stu = list_entry(p, struct student, node_student);
		if(strcmp(stu->m_name, name) == 0)
		{
			return stu;
		}
	}
	return NULL;
}
static int init_student(void)
{
	// INIT_LIST_HEAD(&student_list);
	student_t *stu = NULL;
	add_student_from_head(&student_list, "zhangsan", 40);
	add_student_from_tail(&student_list, "lisi", 80);
	show_all_student(&student_list);

	add_student_from_head(&student_list2, "wangwu", 18);
	add_student_from_head(&student_list2, "zhaoliu", 20);
	
	stu = find_student(&student_list2, "zhaoliu");
	if(stu != NULL)
	{
		// student_t *stu2 = container_of(&stu->node_student, student_t, node_student);		// 通过父结构中任意变量找到其父结构地址
		student_t *stu2 = list_entry(&stu->node_student, student_t, node_student);		// 通过父结构中任意变量找到其父结构地址
		if(stu2 != NULL)
		{
			printk("stu2: name: %s; age: %d\n", stu2->m_name, stu2->m_age);
		}
		// 将找到的节点头增到student_list中
		// list_move(&stu->node_student, &student_list);
	}
	/*
	stu = find_student(&student_list2, "wangwu");
	if(stu != NULL)
	{
		// 将找到的节点尾增到student_list中
		list_move_tail(&stu->node_student, &student_list);
	}
	*/
	list_splice_init(&student_list2, &student_list);
	printk("show student list\n");
	show_all_student(&student_list);
	printk("show student list2\n");
	show_all_student(&student_list2);

	printk("show student list reverse\n");
	show_all_student_reverse(&student_list);

	return 0;
}

static void exit_student(void)
{
	printk("Begin destroy student list");
	destroy();
	printk("End destroy student list");
}

module_init(init_student);
module_exit(exit_student);
