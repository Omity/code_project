#define LOG_TAG "adildemo#cdd_advanced"
#include <linux/sched.h>
#include <linux/hardirq.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#undef pr_fmt
#define pr_fmt(fmt) "[%d] %s: %s:%d " fmt, in_interrupt() ? 0 : task_pid_vnr(current), LOG_TAG, __func__, __LINE__
#undef CDBG
#define CDBG(fmt, args...) pr_info(fmt, ##args)

struct basic_sample_info {
    int    sub_type;
    long   size;
    char  *buffer;
};

typedef struct {
    struct basic_sample_info info;
    struct cdev cdd_advanced_cdev;
    struct mutex cdd_mutex;
    dev_t cdd_advanced_dev_number;
    u32   cdd_advanced_major;
    struct class *cdd_advanced_class;
    struct device *cdd_advanced_dev;
} sample_info_t;

#define CDD_ADVANCED_DEMO_SET_READ       _IOR('x', 0x100, struct basic_sample_info)
#define MAX_NUM   26

sample_info_t *g_cdd_advanced_info = NULL;

int cdd_advanced_open(struct inode *inode, struct file *filp)
{
    CDBG("enter cdd_advanced_open!\n");
    mutex_init(&g_cdd_advanced_info->cdd_mutex);
    return 0;
}

ssize_t cdd_advanced_read(struct file *filp, char __user *userspace_buf,
                          size_t userspace_request_count, loff_t *offset)
{
    char alpha[MAX_NUM + 1];
    int i, cnt;

    mutex_lock(&g_cdd_advanced_info->cdd_mutex);
    CDBG("enter cdd_advanced_read!\n");

    memset(alpha, '\0', MAX_NUM + 1);

    for (i = 0; i < MAX_NUM; i++) {
        alpha[i] = 'a' + i;
    }

    if (userspace_request_count > MAX_NUM) {
        cnt = MAX_NUM;
    } else {
        cnt = userspace_request_count;
    }

    // 无论是 copy_from_user 还是 copy_to_user ，第一个参数都是 to ，第二个参数都是 from
    // copy_from_user(void *to, const void __user *from, unsigned long n)
    // copy_to_user(void __user *to, const void *from, unsigned long n)

    // copy_to_user () : copy data from "kernel driver" to "userspace"
    // copy_to_user(void __user *to, const void *from, unsigned long n)
    if (!copy_to_user((char *)userspace_buf, alpha, cnt)) {
        CDBG("copy string to userspace\n");
        mutex_unlock(&g_cdd_advanced_info->cdd_mutex);
        return cnt;
    } else {
        mutex_unlock(&g_cdd_advanced_info->cdd_mutex);
        return -1;
    }

}

ssize_t cdd_advanced_write(struct file *filp, const char __user *userspace_buf,
                           size_t userspace_request_count, loff_t *offset)
{
    char alpha[MAX_NUM + 1];
    int cnt;

    mutex_lock(&g_cdd_advanced_info->cdd_mutex);
    CDBG("enter cdd_advanced_write!\n");

    memset(alpha, '\0', MAX_NUM + 1);

    if (userspace_request_count > MAX_NUM) {
        cnt = MAX_NUM;
    } else {
        cnt = userspace_request_count;
    }

    // copy_from_user () : copy data from "userspace" to "kernel driver"
    if (!copy_from_user((char *)alpha, userspace_buf, cnt)) {
        CDBG("%s\n", alpha);
        mutex_unlock(&g_cdd_advanced_info->cdd_mutex);
        return cnt;
    } else {
        mutex_unlock(&g_cdd_advanced_info->cdd_mutex);
        return -1;
    }
}

long cdd_advanced_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    char *temp_buf;
    struct basic_sample_info *info = &g_cdd_advanced_info->info;
    mutex_lock(&g_cdd_advanced_info->cdd_mutex);

    switch (cmd) {
    case CDD_ADVANCED_DEMO_SET_READ:
        CDBG("enter cdd_advanced_ioctl!\n");

        /* copy_from_user(void *to, const void __user *from, unsigned long n) */
        if (copy_from_user(info, (void *)arg,
                           sizeof(struct basic_sample_info))) {
            CDBG("%s: failed to copy data from userspace\n", __func__);
            goto failed;
        }

        // 此时，info.buffer 指向的是用户空间的地址
        CDBG("00000 info buffer : %s\n", info->buffer);

        temp_buf = info->buffer;

        // 最好，还是申请一块内核空间的 buffer
        info->buffer = (char *)kzalloc(info->size, GFP_KERNEL);

        /* copy_from_user(void *to, const void __user *from, unsigned long n) */
        if (copy_from_user(info->buffer, (void *)temp_buf, info->size)) {
            CDBG("%s: failed to copy info.buffer from userspace\n", __func__);
            goto failed;
        }

        CDBG("11111 info buffer : %s\n", info->buffer);
        kfree(info->buffer);
        break;

    default:
        break;
    }

    mutex_unlock(&g_cdd_advanced_info->cdd_mutex);
    return 0;

failed:
    mutex_unlock(&g_cdd_advanced_info->cdd_mutex);
    return -1;
}

/* release 函数会在下面的时机被调用：
 * 1. 用户程序显式地调用 close(fd) 时。
 * 2. 用户程序被 信号杀掉时，例如，Ctrl+C，那么 fd 资源被自动回收时。
*/
int cdd_advanced_release(struct inode *inode, struct file *filp)
{
    CDBG("enter cdd_advanced_release!\n");
    mutex_destroy(&g_cdd_advanced_info->cdd_mutex);
    return 0;
}

/*
1. compat_ioctl：当有 32bit 的 userspace application 调用 64bit kernel 的 IOCTL 的时候，这个 callback 会被调用到。
如果驱动没有实现 compat_ioctl，那么32位的用户程序 在64位的kernel上 执行ioctl时，会返回错误：Not a typewriter

2. 如果是 64位 的用户程序 运行在64位的kernel上，调用的是unlocked_ioctl
3. 如果是 32位 的用户程序 运行在32位的kernel上，调用的也是unlocked_ioctl

*    .unlocked_ioctl = cdd_advanced_ioctl,  // for 64-->64 , 32-->32
*    .compat_ioctl = cdd_advanced_ioctl,    // for 32-->64
*/

struct file_operations cdd_advanced_fops = {
    .owner = THIS_MODULE,
    .open = cdd_advanced_open,
    .read = cdd_advanced_read,
    .write = cdd_advanced_write,
    .unlocked_ioctl = cdd_advanced_ioctl,
    .compat_ioctl = cdd_advanced_ioctl,
    .release = cdd_advanced_release,
};

#define _DYNAMIC_
#ifdef _DYNAMIC_
    #define CDD_ADVANCED_MAJOR  0
#else
    #define CDD_ADVANCED_MAJOR  200 /* cat /proc/devices (should not used in /proc/devices) */
#endif

#define CDD_ADVANCED_MINOR   0
#define CDD_ADVANCED_COUNT   1

int __init cdd_advanced_init(void)
{
    int ret = 0;

    g_cdd_advanced_info = kzalloc(sizeof(*g_cdd_advanced_info), GFP_KERNEL);

    if (!g_cdd_advanced_info) {
        return -ENOMEM;
    }

    if (CDD_ADVANCED_MAJOR) {
        g_cdd_advanced_info->cdd_advanced_dev_number = MKDEV(CDD_ADVANCED_MAJOR, CDD_ADVANCED_MINOR);
        ret = register_chrdev_region(g_cdd_advanced_info->cdd_advanced_dev_number,
                                     CDD_ADVANCED_COUNT,
                                     "cdd_advanced_demo");
    } else {
        ret = alloc_chrdev_region(&g_cdd_advanced_info->cdd_advanced_dev_number,
                                  CDD_ADVANCED_MINOR,
                                  CDD_ADVANCED_COUNT,
                                  "cdd_advanced_demo.dynamic");
    }

    if (ret < 0) {
        CDBG("register_chrdev_region failed!\n");
        goto failure_register_chrdev;
    }

    CDBG(" ... dev_number = 0x%x ...\n", g_cdd_advanced_info->cdd_advanced_dev_number);

    g_cdd_advanced_info->cdd_advanced_major = MAJOR(g_cdd_advanced_info->cdd_advanced_dev_number);
    CDBG(" ... cdd_advanced_major = %d\n", g_cdd_advanced_info->cdd_advanced_major);

    cdev_init(&g_cdd_advanced_info->cdd_advanced_cdev, &cdd_advanced_fops);
    CDBG(" ... cdev_init ...\n");

    ret = cdev_add(&g_cdd_advanced_info->cdd_advanced_cdev, g_cdd_advanced_info->cdd_advanced_dev_number, CDD_ADVANCED_COUNT);
    CDBG(" ... cdev_add ...\n");

    if (ret < 0) {
        CDBG("cdev_add failed!\n");
        goto failure_cdev_add;
    }

    /* should create a char device at first with the below command.
     * cat /proc/devices  ==> 248 cdd_advanced_demo
     * mknod /dev/cdd c 248 0
    */

    /*
     * OR, use create a char device by sysfs functions .
    */
    g_cdd_advanced_info->cdd_advanced_class = class_create(THIS_MODULE, "cdd_advanced_class");

    g_cdd_advanced_info->cdd_advanced_dev = device_create(g_cdd_advanced_info->cdd_advanced_class,
                                                          NULL,
                                                          MKDEV(g_cdd_advanced_info->cdd_advanced_major, 0),
                                                          NULL,
                                                          "cdd_advanced_device");


    CDBG(" ... cdd_advanced_init success ... \n");
    return 0;

failure_cdev_add:
    unregister_chrdev_region(g_cdd_advanced_info->cdd_advanced_dev_number, CDD_ADVANCED_COUNT);

failure_register_chrdev:
    kfree(g_cdd_advanced_info);
    return ret;
}

void __exit cdd_advanced_exit(void)
{
    device_destroy(g_cdd_advanced_info->cdd_advanced_class, MKDEV(g_cdd_advanced_info->cdd_advanced_major, 0));
    class_destroy(g_cdd_advanced_info->cdd_advanced_class);

    cdev_del(&g_cdd_advanced_info->cdd_advanced_cdev);

    unregister_chrdev_region(g_cdd_advanced_info->cdd_advanced_dev_number, CDD_ADVANCED_COUNT);

    if (g_cdd_advanced_info->info.buffer) {
        kfree(g_cdd_advanced_info->info.buffer);
    }

    kfree(g_cdd_advanced_info);
    CDBG(" ... cdd_advanced_exit ...\n");
}

module_init(cdd_advanced_init);
module_exit(cdd_advanced_exit);

MODULE_LICENSE("GPL");
