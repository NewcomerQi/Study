/*
 ** hook_open.c for this function
 **
 ** 
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/string.h>
#include <linux/fdtable.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/dcache.h>
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hook sys_call_open by change sys_open entry");
MODULE_AUTHOR("curtis");

void	**sys_call_table_ptr;
unsigned long copied;

char buf[256];
char fullpath[256];
char filename[256];
asmlinkage long (*original_open)( const char __user *, int , mode_t);
asmlinkage long (*original_openat)(int fd, const char __user *, int , mode_t);
asmlinkage int ( *original_write ) ( unsigned int, const char __user *, size_t );
asmlinkage int ( *original_read ) ( unsigned int, const char __user *, size_t );
asmlinkage long (*orig_execve)(const char __user *filename, const char __user *const __user *argv, const char __user *const __user *envp);
asmlinkage long (*orig_unlinkat)(int dfd, const char __user *pathname, int flag);
asmlinkage long (*orig_rename)(const char __user *oldname, const char __user *newname);
asmlinkage long (*orig_creat)(const char __user *pathname, umode_t mode);


struct task_struct *get_proc(pid_t pid)
{
	struct pid *pid_struct = NULL;

	struct task_struct *mytask = NULL;
	pid_struct = find_get_pid(pid);
	if (!pid_struct)
		return NULL;
	mytask = pid_task(pid_struct, PIDTYPE_PID);
	return mytask;

}

int get_path(struct task_struct *mytask, int fd)
{
	struct file *myfile = NULL;
	struct files_struct *files = NULL;
	char path[100] = {'\0'};
	char *ppath = path;
	files = mytask->files;
	if (!files) {
		printk("files is null..\n");
		return -1;
	}
	myfile = files->fdt->fd[fd];
	if (!myfile) {
		printk("myfile is null..\n");
		return -1;
	}
	ppath = d_path(&(myfile->f_path), ppath, 100);
	printk("path:%s\n", ppath);
	return 0;

}
asmlinkage long new_rename(const char __user *oldname, const char __user *newname)
{
	char user_oldname[256];
	char user_newname[256];
	memset(user_oldname,0,sizeof(user_oldname));
	memset(user_newname,0,sizeof(user_newname));
	strncpy_from_user(user_oldname, oldname, sizeof(user_oldname));
	strncpy_from_user(user_newname, newname, sizeof(user_newname));
	printk("old pathname: %s\n",user_oldname);
	printk("new pathname: %s\n",user_newname);
	if(strstr(user_oldname, "hook_sys_call") != NULL)
	{
		printk("have it");
		return -EPERM;
	}
	return (*orig_rename)(oldname, newname);
}
asmlinkage long new_creat(const char __user *pathname, umode_t mode)
{
	char user_msg[256];
	memset(user_msg,0,sizeof(user_msg));
	strncpy_from_user(user_msg, pathname, sizeof(user_msg));
	printk("creat pathname: %s\n",user_msg);
	return (*orig_creat)(pathname, mode);
}
asmlinkage long new_unlinkat(int dfd, const char __user *pathname, int flag)
{
	char user_msg[256];
	memset(user_msg,0,sizeof(user_msg));
	copied = strncpy_from_user(user_msg, pathname, sizeof(user_msg));
	printk("pathname: %s\n",user_msg);
	return (*orig_unlinkat)(dfd, pathname, flag);
}
asmlinkage long new_execve(const char __user *filename, const char __user *const __user *argv, const char __user *const __user *envp)
{
	char user_msg[256];
	printk("%s\n",__FUNCTION__);
	memset(user_msg,0,sizeof(user_msg));
	copied = strncpy_from_user(user_msg, filename, sizeof(user_msg));
	printk("copied: %ld\n", copied);
	printk("pathname: %s\n",user_msg);
	if(strcmp(user_msg, "/usr/bin/dpkg") == 0)
	{
		return -1;
	}
	printk("------\n");
	return (*orig_execve)(filename, argv, envp);
}
asmlinkage int	new_write( unsigned int fd, const char __user *buf, size_t count )
{
	if(fd > 2)
	{
		printk("fd = %d", fd);
		get_path(get_proc(1), fd);
	}
	return ( *original_write )( fd, buf, count );
}

asmlinkage int	new_read( unsigned int fd, const char __user *buf, size_t count )
{
	printk("%s\n",__FUNCTION__);
	printk("------\n");
	return ( *original_read )( fd, buf, count );
}


asmlinkage long new_open( const char __user * pathname, int flags, mode_t mode)
{
	char user_msg[256];
	printk("%s\n",__FUNCTION__);
	memset(user_msg,0,sizeof(user_msg));
	copied = strncpy_from_user(user_msg, pathname, sizeof(user_msg));
	printk("copied:%ld\n", copied);
	printk("pathname%s\n",user_msg);
	printk("------\n");
	return (*original_open)(pathname, flags, mode );
}


asmlinkage long new_openat(int fd, const char __user * pathname, int flags, mode_t mode )
{
	char *start = NULL;
	struct path p;
	int err;
	memset(buf,0,sizeof(buf));
	memset(filename,0,sizeof(filename));
	memset(fullpath,0,sizeof(fullpath));
	strncpy_from_user(filename, pathname, sizeof(filename));
	if(strcmp(filename, ".") == 0)
	{
		return (*original_openat)(fd, pathname, flags, mode );
	}
	if(strcmp(filename, "1.txt") == 0)
	{
		printk("filename: %s", filename);
	}
  	err = user_path_at(fd, pathname, flags, &p);
  	if (err) 
	{
		return (*original_openat)(fd, pathname, flags, mode );
  	}
	start = d_path(&p,buf,sizeof(buf));
	
	if(start == NULL)
		return (*original_openat)(fd, pathname, flags, mode );
	strcat(fullpath, start);
    strcat(fullpath, "/");
    strcat(fullpath ,filename);

	if(strstr(fullpath, "/home/huangyunqi/project") != NULL)
	{
		printk("filename: %s\n",filename);
		printk("fullpath: %s\n",fullpath);
		return -1;
	}
	return (*original_openat)(fd, pathname, flags, mode );
}
static int __init open_hook_init(void)
{
	write_cr0(read_cr0() & (~0x10000));
	sys_call_table_ptr = (void**)kallsyms_lookup_name("sys_call_table");
	original_open = sys_call_table_ptr[__NR_open];
	sys_call_table_ptr[__NR_open] = new_open;
	original_openat = sys_call_table_ptr[__NR_openat];
	sys_call_table_ptr[__NR_openat] = new_openat;
	// orig_execve = sys_call_table_ptr[__NR_execve];
	// sys_call_table_ptr[__NR_execve] = new_execve;
	// original_write = sys_call_table_ptr[__NR_write];
	// sys_call_table_ptr[__NR_write] = new_write;
	// orig_unlinkat = sys_call_table_ptr[__NR_unlinkat];
	// sys_call_table_ptr[__NR_unlinkat] = new_unlinkat;
	// orig_rename = sys_call_table_ptr[__NR_rename];
	// sys_call_table_ptr[__NR_rename] = new_rename;
	sys_call_table_ptr[__NR_create_module] = new_creat;
	orig_creat = sys_call_table_ptr[__NR_create_module];
	write_cr0(read_cr0() | (0x10000));
	return 0;
}

static void __exit open_hook_exit(void)
{
	write_cr0(read_cr0() & (~0x10000));
	sys_call_table_ptr[__NR_open] = original_open;
	sys_call_table_ptr[__NR_openat] = original_openat;
	// sys_call_table_ptr[__NR_execve] = orig_execve;
	// sys_call_table_ptr[__NR_write] = original_write;
	// sys_call_table_ptr[__NR_unlinkat] = orig_unlinkat;
	// sys_call_table_ptr[__NR_rename] = orig_rename;
	sys_call_table_ptr[__NR_create_module] = orig_creat;
	write_cr0(read_cr0() | (0x10000));
	printk("Bye bye open_hook\n");
}

module_init(open_hook_init);
module_exit(open_hook_exit);

