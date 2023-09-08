#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>		// 内存分配函数
#include <linux/string.h>
#include <linux/rbtree.h>

MODULE_LICENSE("GPL");

typedef struct student
{
	char m_name[20];
	int m_age;
	struct rb_node node;
} student_t;

struct rb_root root = RB_ROOT;

student_t * rb_insert(const char *name, int age)
{
	struct rb_node **new = &(root.rb_node), *parent = NULL;
	student_t *temp_stu, *stu;
	while(*new)
	{
		temp_stu = rb_entry(*new, student_t, node);
		parent = *new;

		if(age < temp_stu->m_age)
		{
			new = &((*new)->rb_left);
		}
		else if(age > temp_stu->m_age)
		{
			new = &((*new)->rb_right);
		}
		else
			return temp_stu;
	}
	stu = kmalloc(sizeof(student_t), GFP_KERNEL);
	if(stu != NULL)
	{
		strcpy(stu->m_name, name);
		stu->m_age = age;
	}
	rb_link_node(&stu->node, parent, new);
	rb_insert_color(&stu->node, &root);
	return stu;
}

// 递归遍历打印
void rb_print(struct rb_node *new)
{
	student_t *temp_stu;
	if(new)
	{
		temp_stu = rb_entry(new, student_t, node);
		printk("name: %s; age: %d\n", temp_stu->m_name, temp_stu->m_age);
		rb_print(new->rb_left);
		rb_print(new->rb_right);
	}
}

// 通过 rb_first和rb_next方法进行遍历
void rb_print2(void)
{
	student_t *temp_stu;
	struct rb_node *it;
	for(it = rb_first(&root); it; it = rb_next(it))
	{
		temp_stu = rb_entry(it, student_t, node);
		if(temp_stu)
			printk("name: %s; age: %d\n", temp_stu->m_name, temp_stu->m_age);
	}
}

// 递归遍历销毁
void rb_destroy(struct rb_node *new)
{
	student_t *temp_stu;
	if(new)
	{
		temp_stu = rb_entry(new, student_t, node);
		if(temp_stu)
		{
			printk("erase name: %s; age: %d\n", temp_stu->m_name, temp_stu->m_age);
			kfree(temp_stu);
			rb_erase(new, &root);
		}
		rb_destroy(new->rb_left);
		rb_destroy(new->rb_right);
	}
}
void rb_destroy2(void)
{
	student_t *temp_stu;
	struct rb_node *it;
	for(it = rb_first(&root); it; it = rb_next(it))
	{
		temp_stu = rb_entry(it, student_t, node);
		if(temp_stu)
		{
			printk("erase name: %s; age: %d\n", temp_stu->m_name, temp_stu->m_age);
			rb_erase(it, &root);
		}
	}

}

int rb_my_erase(int age)
{
	struct rb_node **new = &(root.rb_node), *parent = NULL;
	student_t *temp_stu;
	while(*new)
	{
		temp_stu = rb_entry(*new, student_t, node);
		parent = *new;

		if(age < temp_stu->m_age)
		{
			new = &((*new)->rb_left);
		}
		else if(age > temp_stu->m_age)
		{
			new = &((*new)->rb_right);
		}
		else
		{
			printk("erase name: %s\n", temp_stu->m_name);
			kfree(temp_stu);
			temp_stu = NULL;
			rb_erase(parent, &root);
			return true;
		}
	}
	return false;
}

student_t *rb_search(int age)
{
	struct rb_node **new = &(root.rb_node), *parent = NULL;
	student_t *temp_stu;
	while(*new)
	{
		temp_stu = rb_entry(*new, student_t, node);
		parent = *new;

		if(age < temp_stu->m_age)
		{
			new = &((*new)->rb_left);
		}
		else if(age > temp_stu->m_age)
		{
			new = &((*new)->rb_right);
		}
		else
		{
			return temp_stu;
		}
	}
	return NULL;
}

static int begin_student(void)
{
	student_t *stu = NULL, *stu_2 = NULL, *stu_3 = NULL, *stu_4 = NULL, *stu_5 = NULL;
	rb_insert("zhangsan", 20);
	rb_insert("lisi", 21);
	rb_insert("wangwu", 22);
	rb_insert("zhaoliu", 23);
	stu = rb_search(21);
	if(stu)
		printk("find: name: %s; age: %d\n", stu->m_name, stu->m_age);
	rb_print(root.rb_node);
	rb_my_erase(22);
	rb_print2();
	// 替换操作
	stu_2 = kmalloc(sizeof(student_t), GFP_KERNEL);
	if(stu_2 != NULL)
	{
		strcpy(stu_2->m_name, "hanmeimei");
		stu_2->m_age = 24;
	}
	rb_replace_node(&stu->node, &stu_2->node, &root);
	rb_print2();
	
	stu_3 = rb_entry(rb_first_postorder(&root), student_t, node);
	if(stu_3)
		printk("stu_3: name: %s; age: %d\n", stu_3->m_name, stu_3->m_age);
	stu_4 = rb_entry(rb_next_postorder(&stu_3->node), student_t, node);
	if(stu_4)
		printk("stu_4: name: %s; age: %d\n", stu_4->m_name, stu_4->m_age);
	stu_5 = rb_entry(rb_next_postorder(&stu_4->node), student_t, node);
	if(stu_5)
		printk("stu_5: name: %s; age: %d\n", stu_5->m_name, stu_5->m_age);
	return 0;
}

static void exit_student(void)
{
	// rb_destroy(root.rb_node);
	rb_destroy2();
}

module_init(begin_student);
module_exit(exit_student);
