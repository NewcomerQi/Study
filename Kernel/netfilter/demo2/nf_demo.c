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
#include <linux/string.h>
//NIPQUAD宏便于把数字IP地址输出
#define NIPQUAD(addr) \
((unsigned char *)&addr)[0], \
((unsigned char *)&addr)[1], \
((unsigned char *)&addr)[2], \
((unsigned char *)&addr)[3]


//函数声明
unsigned int getRoutingInfo(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);

//用于描述钩子函数信息
static struct nf_hook_ops nfho = {  
    .hook = getRoutingInfo,  
    .pf = PF_INET,  
    .hooknum =NF_INET_LOCAL_OUT ,  
    .priority = NF_IP_PRI_FIRST,
}; 

unsigned int getRoutingInfo(void *priv, 
                    struct sk_buff *skb, 
                    const struct nf_hook_state *state){
	char saddr_ip[20];
    struct iphdr *iph=ip_hdr(skb);  //指向struct iphdr结构体
    struct tcphdr *tcph;            //指向struct tcphdr结构体
    struct udphdr *udph;            //指向struct udphdr结构体
    int header=0;

    // if(ntohl(iph->saddr) == filterip)
    sprintf(saddr_ip, "%u.%u.%u.%u", NIPQUAD(iph->saddr));
	if(strcmp(saddr_ip, "192.168.92.147") == 0)
	{
		printk("Equal");
        if(likely(iph->protocol==IPPROTO_TCP)){
            tcph=tcp_hdr(skb);
            if(skb->len-header>0){
                printk("srcIP:%u.%u.%u.%u dstIP:%u.%u.%u.%u srcPORT:%d dstPORT:%d PROTOCOL:%s\n", 
                    NIPQUAD(iph->saddr), 
                    NIPQUAD(iph->daddr), 
                    ntohs(tcph->source), 
                    ntohs(tcph->dest), 
                    "TCP");
            }//判断skb是否有数据 结束
        }else if(likely(iph->protocol==IPPROTO_UDP)){
            udph=udp_hdr(skb);
            if(skb->len-header>0){
               	printk("srcIP:%u.%u.%u.%u dstIP:%u.%u.%u.%u srcPORT:%d dstPORT:%d PROTOCOL:%s\n", 
                    NIPQUAD(iph->saddr), 
                    NIPQUAD(iph->daddr), 
                    ntohs(udph->source), 
                    ntohs(udph->dest), 
                    "UDP");
            }//判断skb是否有数据 结束
        }//判断传输层协议分支 结束
    }//判断数据包源IP是否等于过滤IP 结束
    return NF_ACCEPT;
}
    
static int __init getRoutingInfo_init(void)  {  
    nf_register_net_hook(&init_net, &nfho);     //注册钩子函数
    printk("register getRoutingInfo mod\n");
    printk("Start...\n");
    return 0;  
}  
static void __exit getRoutingInfo_exit(void){  
    nf_unregister_net_hook(&init_net, &nfho);   //取消注册钩子函数
    printk("unregister getRoutingInfo mod\n");
    printk("Exit...\n");
}  

module_init(getRoutingInfo_init);  
module_exit(getRoutingInfo_exit);  
MODULE_AUTHOR("hyq");  
MODULE_LICENSE("GPL"); 

