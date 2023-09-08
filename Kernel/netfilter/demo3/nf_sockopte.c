#include "nf_sockopte.h"

static struct sock *nl_sk = NULL;   //用于标记netlink
static int userpid = -1;            //用于存储用户程序的pid
static unsigned int filterip = 0;   //用于存储需要过滤的源IP，小端格式

static struct list_head rule_list; // 定义链表头

//用于描述钩子函数信息
static struct nf_hook_ops nfho = {  
    .hook = getRoutingInfo,  
    .pf = PF_INET,  
    .hooknum =NF_INET_LOCAL_OUT ,  
    .priority = NF_IP_PRI_FIRST,
};

//用于描述Netlink处理函数信息
struct netlink_kernel_cfg cfg = {
    .input = nl_data_ready,
};

unsigned int getRoutingInfo(void *priv, 
                    struct sk_buff *skb, 
                    const struct nf_hook_state *state){
    struct iphdr *iph=ip_hdr(skb);  //指向struct iphdr结构体
    struct tcphdr *tcph;            //指向struct tcphdr结构体
    struct udphdr *udph;            //指向struct udphdr结构体
    int header=0;
    char routingInfo[ROUTING_INFO_LEN] = {0};//用于存储路由信息
    // 遍历链表
    rule_t *rule = NULL;
    struct list_head *p;
	if(list_empty(&rule_list) != 0)
	{
	 	return NF_ACCEPT;
	}
	list_for_each(p, &rule_list)
	{
		rule = list_entry(p, rule_t, node_rule);
        if(ntohl(iph->saddr) == rule->src_ip)       // 源IP地址匹配上了
        {
            printk("srcIP: %u.%u.%u.%u\n", NIPQUAD(iph->saddr));
            printk("dstIP: %u.%u.%u.%u\n", NIPQUAD(iph->daddr));
            if(rule->b_forbit == 1)
            {
                if(rule->ptl == ALL)
                {
                    return NF_DROP;
                }
                else if(rule->ptl == TCP && likely(iph->protocol==IPPROTO_TCP))
                {
                    printk("Forbit TCP");
                    return NF_DROP;
                }

                else if(rule->ptl == ICMP && likely(iph->protocol==IPPROTO_ICMP))
                {
                    printk("Forbit ICMP");
                    return NF_DROP;
                }

                if(rule->ptl == UDP && likely(iph->protocol==IPPROTO_UDP))
                {
                    printk("Forbit UDP");
                    return NF_DROP;
                }
            }
        }
	}
    return NF_ACCEPT;
}

int netlink_to_user(char *msg, int len){
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    skb = nlmsg_new(MAX_PAYLOAD, GFP_ATOMIC);
    if(!skb){
        printk(KERN_ERR"Failed to alloc skb\n");
        return 0;
    }
    nlh = nlmsg_put(skb, 0, 0, 0, MAX_PAYLOAD, 0);
    printk("sk is kernel %s\n", ((int *)(nl_sk+1))[3] & 0x1 ? "TRUE" : "FALSE");
    printk("Kernel sending routing infomation to client %d.\n", userpid);
    
    //发送信息
    memcpy(NLMSG_DATA(nlh), msg, len);
    if(netlink_unicast(nl_sk, skb, userpid, 1) < 0){    //此处设置为非阻塞,防止缓冲区已满导致内核停止工作
        printk(KERN_ERR"Failed to unicast skb\n");
        userpid = -1;
        filterip = 0;
        return 0;
    }
    return 1;
}

//当有netlink接收到信息时,此函数将进行处理
static void nl_data_ready(struct sk_buff *skb){
    struct nlmsghdr *nlh = NULL;
    rule_t * msg = NULL;
    if(skb == NULL){
        printk("skb is NULL\n");
        return;
    }
    nlh = (struct nlmsghdr *)skb->data;
    msg = (rule_t *)NLMSG_DATA(nlh);
	printk("clear = %d\n", msg->b_clear);
	if(msg->b_clear == 1)	// 清空
	{
		clear();
		printk("clear all rules\n");
		return;
	}
    printk("kernel received message from %d: %u.%u.%u.%u\n", nlh->nlmsg_pid, NIPQUAD(msg->src_ip));
    
    // filterip=kern_inet_addr((char *)NLMSG_DATA(nlh));
    // 将规则加入到列表
    add_rule_from_head(msg);
    // userpid=nlh->nlmsg_pid;
}


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
    return val;
}


void kern_inet_ntoa(char *ip_str , unsigned int ip_num){
    unsigned char *p = (unsigned char*)(&ip_num);
    sprintf(ip_str, "%u.%u.%u.%u", p[0],p[1],p[2],p[3]);
} 

// 头增
void add_rule_from_head(const rule_t * rule)
{
    rule_t *i_rule = NULL;
	i_rule = kmalloc(sizeof(rule_t), GFP_KERNEL);
	if(i_rule != NULL)
	{
        i_rule->b_forbit = rule->b_forbit;
        i_rule->des_ip = rule->des_ip;
        i_rule->port = rule->port;
        i_rule->ptl = rule->ptl;
        i_rule->src_ip = rule->src_ip;
		list_add(&i_rule->node_rule, &rule_list);	// 头增
	}

}

// 遍历并删除
void destroy(void)
{
	rule_t *watch, *next;
	// 判断链表是否为空
	if(list_empty(&rule_list) != 0)
	{
		printk("rule list is empty\n");
        return;
    }
    // 遍历销毁
	list_for_each_entry_safe(watch, next, &rule_list, node_rule)
	{
		kfree(watch);
	}
}

void clear(void)
{
	rule_t *watch, *next;
	// 判断链表是否为空
	if(list_empty(&rule_list) != 0)
	{
		printk("rule list is empty\n");
        return;
    }
    // 遍历销毁
	list_for_each_entry_safe(watch, next, &rule_list, node_rule)
	{
		kfree(watch);
	}
    INIT_LIST_HEAD(&rule_list);     // 初始化链表头
}

// 初始化
static int __init getRoutingInfo_init(void)  {
    nf_register_net_hook(&init_net, &nfho);     //注册钩子函数
    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);   //注册Netlink处理函数
    if(!nl_sk){
        printk(KERN_ERR"Failed to create nerlink socket\n");
    }
    INIT_LIST_HEAD(&rule_list);     // 初始化链表头

    printk("register getRoutingInfo mod\n");
    printk("Start...\n");
    return 0;  
}  

// 退出，清理
static void __exit getRoutingInfo_exit(void){  
    nf_unregister_net_hook(&init_net, &nfho);   //取消注册钩子函数
    netlink_kernel_release(nl_sk);              //取消注册Netlink处理函数
    destroy();  // 销毁链表
    printk("unregister getRoutingInfo mod\n");
    printk("Exit...\n");
}  

module_init(getRoutingInfo_init);  
module_exit(getRoutingInfo_exit);  
MODULE_AUTHOR("zsw");  
MODULE_LICENSE("GPL"); 

