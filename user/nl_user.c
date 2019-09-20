#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#define NETLINK_TEST_PROTOCOL 31
#define MAX_PAYLOAD_SIZE 1024

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
struct msghdr msg;
char user_msg[MAX_PAYLOAD_SIZE];
int sock_fd;

static void print_usage(const char *prog)
{
    fprintf(stdout,"Usage: %s [-hm]\n",prog);
    fprintf(stdout,"\t-m --message\t\t\t\t: set the message to send to the kernel.\n");
    fprintf(stdout,"\t-h --help\t\t\t\t: print this message\n");
    return;
}

static void parse_opts(int argc, char *argv[])
{
    while (1) {
        static const struct option lopts[] = {
            { "message", required_argument, 0, 'm' },
            { "help", no_argument, 0, 'h' },
            { 0, 0, 0, 0 },
        };
        int c;

        c = getopt_long(argc, argv, "hm:", lopts, NULL);

        if (c == -1)
            break;

        switch (c) {
        case 'm':
            strcpy(user_msg, optarg);
            break;;
        case 'h':
            print_usage(argv[0]);
            break;
        default:
            print_usage(argv[0]);
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    int msg_len = 0;
    memset(user_msg, 0, sizeof(user_msg));
    if ((argc <= 1) || (argc > 3))
    {
        print_usage(argv[0]);
        return -1;
    }
    parse_opts(argc, argv);
    msg_len = strlen(user_msg);
    if (msg_len > MAX_PAYLOAD_SIZE) {
        fprintf(stderr, "Message too long\n");
        return -1;
    }

    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST_PROTOCOL);
    if(sock_fd < 0) {
        fprintf(stderr, "ERROR opening socket\n");
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid();

    bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

    memset(&dest_addr, 0, sizeof(dest_addr));

    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0;
    dest_addr.nl_groups = 0;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD_SIZE));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD_SIZE));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD_SIZE);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    strcpy(NLMSG_DATA(nlh), user_msg);

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    fprintf(stdout, "User send the following message to the kernel:\n");
    fprintf(stdout, "%s\n", user_msg);
    sendmsg(sock_fd, &msg, 0);
    recvmsg(sock_fd, &msg, 0);
    fprintf(stdout, "User receive a message from kernel: %s\n", (char *)NLMSG_DATA(nlh));

    close(sock_fd);
    return 0;
}
