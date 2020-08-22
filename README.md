# Hello via netlink
A basic communication between kernel and user via netlink

## Get and build the software
### Download the source code
    $ cd $HOME
    $ mkdir devel
    $ cd devel
    $ git clone https://github.com/ichergui/hello-netlink.git

### Build
##### kernel side

```bash
$ cd $HOME/devel/hello-netlink/kernel
$ make all
```
##### User side
```bash
$ cd $HOME/devel/hello-netlink/user
$ gcc -o nl_user nl_user.c $(pkg-config --cflags --libs libnl-3.0)
```


## Test
#### Insert it into the Linux kernel
```bash
$ cd $HOME/devel/hello-netlink/kernel
$ sudo insmod nl_kernel.ko
```
* __verification__
```bash
$ lsmod | grep -i "nl_kernel"
nl_kernel              16384  0
$
```

#### Launch the application
```bash
$ cd $HOME/devel/hello-netlink/user
$ ./nl_user -m "Hello from Ilies"
User send the following message to the kernel:
Hello from Ilies
User receive a message from kernel: Hello from kernel
$
```

Try with the following command line to see what's happened.

```bash
$ $ tail -f /var/log/kern.log

Sep 20 22:44:48 dev-machine kernel: [13466.108923] Entering: init_module
Sep 20 22:45:42 dev-machine kernel: [13520.057427] Entering: nl_recv_msg_cb
Sep 20 22:45:42 dev-machine kernel: [13520.057430] Kernel receive a message from user:Hello from Ilies
```


#### Remove it from Linux kernel
```bash
$ sudo rmmod nl_kernel
```
