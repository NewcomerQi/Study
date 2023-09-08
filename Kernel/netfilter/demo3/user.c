#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <unistd.h>
#define NETLINK_TEST 17         //用于自定义协议
#define MAX_PAYLOAD 1024        //最大载荷容量
#define RECEIVE_CNT 10          //接受路由信息的数量

int n = RECEIVE_CNT;                    //接受路由信息的数量
int sock_fd, store_fd;                   //套接字描述符, 文件描述符
struct iovec iov;                       //
struct msghdr msg;                      //存储发送的信息
struct nlmsghdr *nlh = NULL;            //用于封装信息的头部
struct sockaddr_nl src_addr, dest_addr; //源地址,目的地址(此处地址实际上就是pid)


typedef enum Protocol
{
	TCP = 0x00000001,
	UDP,
	ICMP,
	ALL
} Protocol_t;

typedef struct rule
{
	unsigned int src_ip;	// 用于保存源IP地址,小端格式
	unsigned int des_ip;	// 用于保存目的IP地址,小端格式
	unsigned short port;	// 端口
	int b_forbit;			// 是否禁止，1表示禁止
	int b_clear;			// 是否清空规则
	Protocol_t ptl;			// 协议
} rule_t;

unsigned int kern_inet_addr(char *ip_str){
	unsigned int val = 0, part = 0;
	int i = 0;
	char c;
	for(i=0; i<4; ++i){
		part = 0;
		while ((c=*ip_str++)!='\0' && c != '.'){
			if(c < '0' || c > '9') return -1;//字符串存在非数字
			part = part*10 + (c-'0');
		}
		if(part>255) return -1;//单部分超过255
		val = ((val << 8) | part);//以小端格式存储数字IP地址
		if(i==3){
			if(c!='\0') //  结尾存在额外字符
				return -1;
		}else{
			if(c=='\0') //  字符串过早结束
				return -1;
		}//结束非法字符串判断
	}//结束for循环
	printf("%d\n", val);
	return val;
}

int main(int argc, char *argv[])
{
	sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST);
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;        //协议族
	src_addr.nl_pid = getpid();             //本进程pid
	src_addr.nl_groups = 0;                 //多播组,0表示不加入多播组
	bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;       //协议族
	dest_addr.nl_pid = 0;                   //0表示kernel的pid
	dest_addr.nl_groups = 0;                //多播组,0表示不加入多播组

	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);  //设置缓存空间
	nlh->nlmsg_pid = getpid();                  //本进程pdi
	nlh->nlmsg_flags = 0;                       //额外说明信息

	rule_t *rule = (rule_t *)malloc(sizeof(rule_t));
	rule->b_clear = 0;
	do
	{
		if(argc == 2)
		{
			if(strcmp(argv[1], "F") == 0 || strcmp(argv[1], "-F") == 0)
			{
				rule->b_clear = 1;	
				printf("clear all clear = %d\n", rule->b_clear);
				break;
			}
			else
			{
				printf("Usage : %s <protocol> <ip> <ACCEPT|DROP>\n", argv[0]);
				free(rule);
				exit(1);
			}
		}
		if(argc != 4){
			printf("Usage : %s <protocol> <ip> <ACCEPT|DROP>\n", argv[0]);
			free(rule);
			exit(1);
		}

		rule->des_ip = 0;
		rule->port = 0;
		if(strcmp(argv[1], "TCP") == 0)
		{
			rule->ptl = TCP;
		}
		else if(strcmp(argv[1], "UDP") == 0)
		{
			rule->ptl = UDP;
		}
		else if(strcmp(argv[1], "ICMP") == 0)
		{
			rule->ptl = ICMP;
		}
		else
		{
			rule->ptl = ALL;
		}
		rule->src_ip = kern_inet_addr(argv[2]); // 源IP
		if(strcmp(argv[3], "DROP") == 0)
		{
			rule->b_forbit = 1;
		}
		else
		{
			rule->b_forbit = 0;
		}

	}while(0);
	memcpy(NLMSG_DATA(nlh), rule, sizeof(rule_t));
	free(rule);

	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	sendmsg(sock_fd, &msg, 0);  // 发送信息到kernel
	close(sock_fd);
	return 0;
}

