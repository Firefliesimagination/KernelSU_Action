#include <linux/module.h>
#include <linux/tty.h>
#include <linux/miscdevice.h>
#include "comm.h"
#include "memory.h"
#include "process.h"
#include <linux/random.h>

#define DEVICE_NAME "wanbai"

int dispatch_open(struct inode *node, struct file *file)
{
    return 0;
}

int dispatch_close(struct inode *node, struct file *file)
{
    return 0;
}

long dispatch_ioctl(struct file* const file, unsigned int const cmd, unsigned long const arg)
{
    static COPY_MEMORY cm;
    static MODULE_BASE mb;
    static char key[0x100] = {0};
    static char name[0x100] = {0};
    static bool is_verified = false;

    if(cmd == OP_INIT_KEY && !is_verified) {
        if (copy_from_user(key, (void __user*)arg, sizeof(key)-1) != 0) {
            return -1;
        }
    }
    switch (cmd) {
        case OP_READ_MEM:
            {
                if (copy_from_user(&cm, (void __user*)arg, sizeof(cm)) != 0) {
                    return -1;
                }
                if (read_process_memory(cm.pid, cm.addr, cm.buffer, cm.size) == false) {
                    return -1;
                }
            }
            break;
        case OP_WRITE_MEM:
            {
                if (copy_from_user(&cm, (void __user*)arg, sizeof(cm)) != 0) {
                    return -1;
                }
                if (write_process_memory(cm.pid, cm.addr, cm.buffer, cm.size) == false) {
                    return -1;
                }
            }
            break;
        case OP_MODULE_BASE:
            {
                if (copy_from_user(&mb, (void __user*)arg, sizeof(mb)) != 0 
                ||  copy_from_user(name, (void __user*)mb.name, sizeof(name)-1) !=0) {
                    return -1;
                }
                mb.base = get_module_base(mb.pid, name);
                if (copy_to_user((void __user*)arg, &mb, sizeof(mb)) !=0) {
                    return -1;
                }
            }
            break;
        default:
            break;
    }
    return 0;
}

struct file_operations dispatch_functions = {
    .owner   = THIS_MODULE,
    .open    = dispatch_open,
    .release = dispatch_close,
    .unlocked_ioctl = dispatch_ioctl,
};

struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dispatch_functions,
};

int __init driver_entry(void)
{
    int ret;
    printk("[+] driver_entry");
    int i;
    // declare a char array of size 5
    char dev_name[5];
    // use a loop to fill the char array with random lowercase letters
    for (i = 0; i < 4; i++) {
        // get a random byte
        unsigned char c;
        get_random_bytes(&c, 1);
        
        // convert it to a lowercase letter using ASCII values
        c = 'a' + c % 26;
        
        // store it in the char array
        dev_name[i] = c;
    }
    
    // add a null terminator at the end of the char array
    dev_name[4] = '\0';
    
    // assign the char array to the misc.name field
    misc.name = dev_name;

	ret = misc_register(&misc);
	return ret;
}

void __exit driver_unload(void)
{
    printk("[+] driver_unload");
	misc_deregister(&misc);
}

module_init(driver_entry);
module_exit(driver_unload);

MODULE_DESCRIPTION("Linux Kernel.");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wanbai");
