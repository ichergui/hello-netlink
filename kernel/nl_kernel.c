#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#define NETLINK_TEST_USER 31

struct sock *nlk_soc = NULL;

static void nl_recv_msg_cb(struct sk_buff *user_skb) {
    struct nlmsghdr *nlh;
    int pid;
    struct sk_buff *kernel_skb;
    int msg_size;
    char *msg = "Hello from kernel";
    int res;

    printk(KERN_DEBUG "Entering: %s\n", __func__);
    nlh = (struct nlmsghdr*)user_skb->data;
    printk(KERN_INFO "Kernel receive a message from user:%s\n", (char*)nlmsg_data(nlh));
    pid = nlh->nlmsg_pid; /*pid of sending process */

    msg_size = strlen(msg);
    kernel_skb = nlmsg_new(msg_size, 0);
    if(kernel_skb == NULL) {
        printk(KERN_ERR "create a new netlink message failed\n");
        return;
    }

    nlh = nlmsg_put(kernel_skb,0,0,NLMSG_DONE,msg_size,0);  
    NETLINK_CB(kernel_skb).dst_group = 0;
    strncpy(nlmsg_data(nlh), msg, msg_size);
    res = nlmsg_unicast(nlk_soc, kernel_skb, pid);
    if(res < 0)
        printk(KERN_ERR "unicast a netlink message failed\n");
}

struct netlink_kernel_cfg nl_kcfg = {
    .input = nl_recv_msg_cb,
};

int init_module ( void ) {
    printk(KERN_DEBUG "Entering: %s\n", __func__);
    nlk_soc = netlink_kernel_create(&init_net, NETLINK_TEST_USER, &nl_kcfg);
    if(nlk_soc == NULL) {
        printk(KERN_ERR "Kernel netlink socket create failed.\n");
        return -1;
    }
    return 0;
}

void cleanup_module() {
    printk(KERN_DEBUG "Entering: %s\n", __func__);
    if (nlk_soc != NULL)
        netlink_kernel_release(nlk_soc);
}

MODULE_DESCRIPTION("Hello via netlink kernel module");
MODULE_AUTHOR("Ilies CHERGUI <ilies.chergui@gmail.com>");
MODULE_LICENSE("GPL");
