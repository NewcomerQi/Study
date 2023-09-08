#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>


char *dev_name = "/dev/mydev";

// 信号处理函数
static void signal_handler(int signum, siginfo_t *info, void *context)
{
	// 打印接收到的信号值
    printf("signal_handler: signum = %d \n", signum);
    printf("signo = %d, code = %d, errno = %d \n",
	         info->si_signo,
	         info->si_code, 
	         info->si_errno);
}

int main(int argc, char *argv[])
{
	int fd, count = 0;
	int pid = getpid();

	// 打开GPIO
	if((fd = open(dev_name, O_RDWR | O_NDELAY)) < 0){
		printf("open dev failed! \n");
		return -1;
	}

	printf("open dev success! \n");
	
	// 注册信号处理函数
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = &signal_handler;
	sa.sa_flags = SA_SIGINFO;
	
	sigaction(SIGUSR1, &sa, NULL);

	// set PID 
	printf("call ioctl. pid = %d \n", pid);
	ioctl(fd, 100, &pid);

	// 死循环，等待接收信号
	while (1)
		sleep(1);

	// 关闭设备
	close(fd);
}
