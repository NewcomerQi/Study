#ifndef _NF_SOCKOPTE_H_
#define _NF_SOCKOPTE_H_

//内核编程需要的头文件
#include <linux/module.h>
#include <linux/kernel.h>
//Netfilter需要的头文件
#include <linux/net.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/if_ether.h>
#include <linux/netdevice.h>  
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/icmp.h>
#include <net/protocol.h>
//netlink需要的头文件
#include <net/sock.h>
#include <net/net_namespace.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/netlink.h>

#include <linux/list.h>		// 链表
#include <linux/slab.h>		// 内存分配
#include <linux/string.h>	// 字符串操作

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
	int b_clear;			// 是否清空
	Protocol_t ptl;			// 协议
	struct list_head node_rule;		// 链表节点
} rule_t;


//NIPQUAD宏便于把数字IP地址输出
#define NIPQUAD(addr) \
((unsigned char *)&addr)[0], \
((unsigned char *)&addr)[1], \
((unsigned char *)&addr)[2], \
((unsigned char *)&addr)[3]

#define NETLINK_TEST 17         //用于自定义协议
#define MAX_PAYLOAD 1024        //最大载荷容量
#define ROUTING_INFO_LEN 100    //单个路由信息的容量

//用于将字符串IP地址转化为小端格式的数字IP地址
unsigned int kern_inet_addr(char *ip_str);

//用于将数字IP地址转化为字符串IP地址
void kern_inet_ntoa(char *ip_str , unsigned int ip_num);

// 钩子函数
unsigned int getRoutingInfo(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

//当有netlink接收到信息时,此函数将进行处理
static void nl_data_ready(struct sk_buff *skb);

//用于给用户程序发送信息
int netlink_to_user(char *msg, int len);

// 链表头增
void add_rule_from_head(const rule_t * rule);
// 销毁链表
void destroy(void);

// 清空链表
void clear(void);

#endif
