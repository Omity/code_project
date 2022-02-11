#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/spinlock.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <asm/hardware.h>

#define DEVICE_NAME           "custom-buttons" //定义按键设备名
#define BUTTON_MAJOR          232 //定义按键主设备号


static struct key_info {
	int irq_no;
	unsigned int gpio_port;
	int key_no;
}key_info_tab[4] = { //按键所使用的 CPU 资源
	{ IRQ_EINT1, GPIO_F1, 1 },
	{ IRQ_EINT2, GPIO_F2, 2 },
	{ IRQ_EINT3, GPIO_F3, 3 },
	{ IRQ_EINT7, GPIO_F7, 4 },
};


static int ready = 0;
static int key_value = 0;
static DECLARE_WAIT_QUEUE_HEAD(buttons_wait);

//声明、初始化等待队列 buttons_wait
static void buttons_irq(int irq, void *dev_id, struct pt_regs * reg)//中断处理程序
{
	struct key_info *k;
	int i;
	int found = 0;
	int up;
	int flags;
	for (i = 0; i < sizeof key_info_tab / sizeof key_info_tab[1]; i++) {
		k = key_info_tab + i;
		if (k->irq_no == irq) {
			found = 1;
			break;
		}
	}
	if (!found) {
		printk("bad irq %d in button\n", irq);
		return;
	}
	save_flags(fla gs);
	cli(); //禁用 所有中断
	set_gpio_mode_user(k->gpio_port, GPIO_MODE_IN);
	up = read_gpio_bit(k->gpio_port );
	set_external_irq(k->irq_no, EXT_BOTH_EDGES, GPIO_PULLUP_DIS);
	restore_flags(flags);
	if (up) {
		key_value = k->key_no + 0x80;
	} else {
		key_value = k->key_no;
	}
	ready = 1;
	wake_up_interruptible(&buttons_wait);
}

//唤醒 buttons_wait 等待对列中的进程
static int request_irqs(void) 
{
//申请系统中断，中断方式为双边延触发，即在上升沿和下降沿均发生中断
	struct key_info *k;
	int i;
	
	for (i = 0; i < sizeof key_info_tab / sizeof key_info_tab[1]; i++) {
		k = key_info_tab + i;
		set_external_irq(k->irq_no, EXT_BOTH_EDGES, GPIO_PULLUP_DIS); // 双边触发
		if (request_irq(k->irq_no, &buttons_irq, SA_INTERRUPT, DEVICE_NAME, &buttons_irq))
		//0 表示成功，负值表示出错
			return -1;
		}
	return 0;
}


static void free_irqs(void)
{
	//释放中断
	struct key_info *k;
	int i;
	for (i = 0; i < sizeof key_info_tab / sizeof key_info_tab[1]; i++) {
		k = key_info_tab + i;
		free_irq(k->irq_no, but tons_irq);
	}
}


static int matrix4_buttons_read(struct file * file, char * buffer, size_t count, loff_t *ppos )
{
	static int key;
	int f lags;
	int repeat;
	
	if (!ready)
		return -EAGAIN;
	if (count != sizeof(key_value))
		return -EINVAL;
	save_flags(flags);
	if (key != key_value) { 
		key = key_value; 
		repeat = 0;
	//*file_operations 的"读"指针函数实现
	} else {
		repeat = 1;
	}
	restore_flags(flags);
	if (repeat)
		return -EAGAIN;
	copy_to_user(buffer, &key, sizeof(key)); //使用 copy_to_user 把键值送到用户空间
	ready = 0;
	return sizeof(key_value);
}


static unsigned int matrix4_buttons_select(struct file *file, struct poll_table_struct *wait)
{
	if (ready)
		return 1; //POLLIN，设备可以无阻塞读
	poll_wait(file, &buttons_wait, wait); //把当前进程放入一个等待对列
	
	return 0;
	//使进程调用 poll 或 select 系统调用时，阻塞直到 buttons_wait 等待队列唤醒
}


static int matrix4_buttons_ioctl(struct inode *inode, struct file *file, unsigned int cmd , unsigned long arg)
{
	switch (cmd) {
		default:
		return -EINVAL;
	}
}


static struct file_operations matrix4_buttons_fops = {
	.owner = THIS_MODULE,
	.ioctl = matrix4_butto ns_ioctl,
	.poll  = matrix4_buttons_select,
	/*po ll 方法是 poll 和 select 这两个系统调用的后端实现,是在用户空间程序执行 poll 或 select 系统调
	用时被调用,这两个系统调用可用来查询设备是否可读或可写，或是否处理某种特殊状态。这两个系统调用
	是可阻塞的，直至设备变为可读或可写状态为止*/
	.read = matrix4_buttons_read,
};

static devfs_handle_t devfs_handle;

static int __init matrix4_buttons_init(void)
{ //按键初始化
	int ret;
	ready = 0;
	ret = register_chrdev(BUTTON_MAJOR, DEVICE_NAME, &matrix4_buttons_fops);//注册按键设备
	if (ret < 0) {
		printk(DEVICE_NAME " can't register major number\n");
		return ret;
	}
	ret = request_irqs();
	if (ret) {
		//请求中断
		unregister_chrdev(BUTTON_MAJOR, DEVICE_NAME);
		printk(DEVICE_NAME " can't request irqs\n");
	return ret;
	}
	devfs_handle = devfs_register(NULL, DEVICE_NAME, DEVFS_FL_DEFAULT,
	BUTTON_MAJOR, 0, S_IFCHR | S_IRUSR | S_IWUSR, &matrix4_buttons_fops, NULL);
	//使用 devfs 进行注册
	return 0;
}
static void __exit matrix4_buttons_exit(void)
{ 
	devfs_unregister(devfs_handle); 
	free_irqs();
	unregister_chrdev(BUTTON_MAJOR, DEVICE_NAME);
}

module_init(matrix4_buttons_init);
module_exit(matrix4_buttons_exit);
MODULE_LICENSE("GPL");
