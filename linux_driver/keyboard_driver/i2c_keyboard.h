#ifndef __I2C_KEYBOARD_H__
#define __I2C_KEYBOARD_H__


#if DEBUG_EN
#define KBD_DEBUG(fmt, args...) do { \
    printk("[I2C_KBD]%s:"fmt"\n", __func__, ##args); \
} while (0)
#else
#define KBG_DEBUG(fmt, args...)


#define KBD_INFO(fmt, args...) do { \
    printk(KERN_INFO "[I2C_KBD/I]%s:"fmt"\n", __func__, ##args); \
} while (0)

#define KBD_ERROR(fmt, args...) do { \
    printk(KERN_ERR "[I2C_KBD/E]%s:"fmt"\n", __func__, ##args); \
} while (0)

#endif /*__I2C_KEYBOARD_H__*/
