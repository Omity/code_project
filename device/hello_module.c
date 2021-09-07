#define LOG_TAG "adildemo#attribute"
#include <linux/sched.h>
#include <linux/hardirq.h>

#undef pr_fmt
#define pr_fmt(fmt) "[%d] %s: %s:%d " fmt, in_interrupt() ? 0 : task_pid_vnr(current), LOG_TAG, __func__, __LINE__
#undef CDBG
#define CDBG(fmt, args...) pr_info(fmt, ##args)


#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>

struct adil_demo_data {
    char name[64];
};

/********************************************* simple attributes demo ************************************/
static size_t val = 100;

/*
 * cat foo
 *
 */
static ssize_t foo_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    CDBG("%s\n", __func__);
    return sprintf(buf, "%zu\n", val);
}

/*
 * echo 1234 > foo
 *
 */
static ssize_t foo_store(struct device *dev, struct device_attribute *attr,
                         const char *buf, size_t count)
{
    CDBG("%s - count = %zu , buf = %s ......\n", __func__, count, buf);
    CDBG("buf[%zu] = 0x%x\n", count-1, buf[count-1]); // note: buf[count-1] == '\n'    <==> 0x0A
    sscanf(buf, "%zu\n", &val);
    CDBG("val = .%zu.\n", val);
    return count;
}

static DEVICE_ATTR(a_foo, S_IRUGO | S_IWUSR, foo_show, foo_store);            /* 在sys/devices/目录下创建文件并赋予show，store等操作以及权限 */


/********************************************* all sysfs interfaces ************************************/
/* PATH: /sys/class/adil_attr_class/adil_device */
static struct attribute *attrs[] = {
    &dev_attr_a_foo.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

/********************************************* init func & exit func ************************************/

static struct class *adil_class = NULL;
static struct device *adil_dev = NULL;

static int __init example_init(void)
{

    void *ptr_err;
    int err = 0;

    CDBG("... %s() ...\n", __func__);

    adil_class = class_create(THIS_MODULE, "adil_attr_class");

    err = IS_ERR(ptr_err = adil_class);

    adil_dev = device_create(adil_class,
                             NULL,
                             MKDEV(0, 0),
                             NULL,
                             "adil_device");

    err = IS_ERR(ptr_err = adil_dev);

    err = sysfs_create_group(&adil_dev->kobj, &attr_group);

    CDBG("... sysfs_create_group() ...\n");


    return err;
}

static void __exit example_exit(void)
{
    sysfs_remove_group(&adil_dev->kobj, &attr_group);
    device_destroy(adil_class, MKDEV(0, 0));
    class_destroy(adil_class);

    CDBG("... %s() ...\n", __func__);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adil Zhu");
