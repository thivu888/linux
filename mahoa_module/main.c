#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define MEM_SIZE 1024
char *kernel_buffer; 
char xauro[1024],xauma[1024];
char data[1024];
int k, choice;

char MA_TT_UPPERCASE[26] = "GHIJKLMNOPQRSTUVWXYZABCDEF"; 
char MA_TT_LOWERASE[26] = "ghijklmnopqrstuvwxyzabcdef";
int MAHOA = 0;
int GIAIMA = 1;

struct c_driver {
	dev_t dev_num;
	struct class *dev_class;
	struct device *dev;
	struct cdev *vcdev;
} driver_encode;

static int my_open(struct inode *inode, struct file *filp);
static int my_release(struct inode *inode, struct file *filp);
static ssize_t my_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release,
};

int my_open(struct inode *inode, struct file *filp) {
	pr_info("Mo device file thanh cong!!!");
	return 0;
}

int my_release(struct inode *inode, struct file *filp) {
	pr_info("Dong device file thanh cong!!!");
	return 0;
}

ssize_t my_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off) {
    int a = copy_to_user(user_buf, data, 1024);
	return MEM_SIZE;
}

ssize_t my_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off) {
    int i,y,a,j;
    // copy bo nho tu usser xuong nhan
    a = copy_from_user(kernel_buffer, user_buf, len);
    choice = *kernel_buffer;
    switch (choice)
    {
    case 1: // case 0 ma hoa
        i = 0;
		kernel_buffer++;

        while(*kernel_buffer != '\0') {
			xauro[i++] = *kernel_buffer;
			kernel_buffer++;
		}
		xauro[i] = '\0';
        i = 0;
		while(xauro[i] != '\0') {
            if( xauro[i] >= 65 && xauro[i] <= 90) { // char uppercase
                y = xauro[i] - 65;
                y = MA_TT_UPPERCASE[y];
                data[i] = y;
            } else if( xauro[i] >= 97 && xauro[i] <= 122) {
                y = xauro[i] - 97;
                y = MA_TT_LOWERASE[y];
                data[i] = y;
            } else {
                data[i] = xauro[i];
            }
            i++;
		}
        break;
    case 2:
        i = 0;
		kernel_buffer++;

        while(*kernel_buffer != '\0') {
			xauma[i++] = *kernel_buffer;
			kernel_buffer++;
		}
		xauma[i] = '\0';
        i = 0;
		while(xauma[i] != '\0') {
            if( xauma[i] >= 65 && xauma[i] <= 90) { // char uppercase
                for(j = 0; j< 26; j++){
						if(MA_TT_UPPERCASE[j] == xauma[i]){
							data[i] = xauma[i] + 65;
							break;
						}
					}
            } else if( xauma[i] >= 97 && xauma[i] <= 122) {
                for(j = 0; j< 26; j++){
						if(MA_TT_LOWERASE[j] == xauma[i]){
							data[i] = xauma[i] + 97;
							break;
						}
					}
            } else {
                data[i] = xauma[i];
            }
            i++;
		}
        break;
    default:
        break;
    }
	return len;
}

static int __init char_driver_init(void) {
	int ret = 0;
	driver_encode.dev_num = 0;

	ret = alloc_chrdev_region(&driver_encode.dev_num, 0, 1, "driver_encode"); 
    	if(ret < 0) {
        printk("Can't allocate character driver\n");
		goto failed_register_devnum;
	}
	printk("Insert character driver successfully. major(%d), minor(%d)\n", MAJOR(driver_encode.dev_num), MINOR(driver_encode.dev_num));
	driver_encode.dev_class = class_create(THIS_MODULE, "driver_encode_class");
	if(IS_ERR(driver_encode.dev_class)) {
		printk("Can't create class\n");
		goto failed_create_class;
	}
	driver_encode.dev = device_create(driver_encode.dev_class, NULL, driver_encode.dev_num, NULL, "driver_encode_device");
	if(IS_ERR(driver_encode.dev)) {
		printk("Can't create device file\n");
		goto failed_create_device;
	}
	kernel_buffer = kmalloc(MEM_SIZE,GFP_KERNEL);
	driver_encode.vcdev = cdev_alloc();
	cdev_init(driver_encode.vcdev, &fops);
	cdev_add(driver_encode.vcdev, driver_encode.dev_num, 1);
	return 0;

failed_create_device:
	class_destroy(driver_encode.dev_class);
failed_create_class:
	unregister_chrdev_region(driver_encode.dev_num, 1);
failed_register_devnum:
	return ret;
}

static void __exit driver_encode_exit(void) {
	cdev_del(driver_encode.vcdev);
	kfree(kernel_buffer);
	device_destroy(driver_encode.dev_class, driver_encode.dev_num);
	class_destroy(driver_encode.dev_class);
	pr_info("Bye lab 6!!!\n");
}

module_init(char_driver_init);
module_exit(driver_encode_exit);

MODULE_AUTHOR("Phan Quang Vu");
MODULE_DESCRIPTION("driver_encode");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");