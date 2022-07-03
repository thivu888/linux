#include <linux/module.h>
#include <linux/crypto.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/ctype.h>

#define MEM_SIZE 1024

uint8_t *kernel_buffer;
dev_t dev_num;
struct class *device_class;
struct cdev *char_device;
struct crypto_cipher *tfm;
char key[20] = "0123456789abcdef";
char type[100];
char data[MEM_SIZE];
size_t data_len = 0;


// convert tu hex sang string 
int hextostring(char *in, int len, char *out)
{
    int i;

    for (i = 0; i < len; i++)
    {
        // convert char sang hex
        sprintf(out, "%s%02hhx", out, in[i]);
    }
    return 0;
}

// convert tu string sang hex
int stringtohex(char *in, int len, char *out)
{
    int i;
    int converter[105];
    converter['0'] = 0;
    converter['1'] = 1;
    converter['2'] = 2;
    converter['3'] = 3;
    converter['4'] = 4;
    converter['5'] = 5;
    converter['6'] = 6;
    converter['7'] = 7;
    converter['8'] = 8;
    converter['9'] = 9;
    converter['a'] = 10;
    converter['b'] = 11;
    converter['c'] = 12;
    converter['d'] = 13;
    converter['e'] = 14;
    converter['f'] = 15;
    
    //  chuyen tu ma hex sang char

    for (i = 0; i < len; i = i + 2)
    {
        char byte = converter[(int)in[i]] << 4 | converter[(int)in[i + 1]];
        out[i / 2] = byte;
    }

    return 0;
}

static int open_fun(struct inode *inode, struct file *file)
{
    return 0;
}

static int release_fun(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t read_fun(struct file *file, char *user_buf, size_t len, loff_t *off)
{
    char cipher[1000];
    char hex_cipher[1000];
    int i, j;

    printk("data_len: %ld\n", data_len);

    memset(cipher, 0, sizeof(cipher));
    memset(hex_cipher, 0, sizeof(hex_cipher));

    // thuc hien ma hoa hoac giai ma
    // ma hoa tung khoi mot chia data thanh cac khoi 16 bit
    for (i = 0; i < data_len / 16; i++)
    {
        char one_data[20], one_cipher[20];

        memset(one_data, 0, sizeof(one_data));
        memset(one_cipher, 0, sizeof(one_cipher));

        // load tung khoi mot vao one_data de ma hoa
        for (j = 0; j < 16; j++)
            one_data[j] = data[i * 16 + j];

        printk("one data: %s\n", one_data);

	// kiem tra lenh: encypt thi ma hoa, decrypt thi giai ma
        if (strcmp(type, "encrypt") == 0)
          crypto_cipher_encrypt_one(tfm, one_cipher, one_data);
        if (strcmp(type, "decrypt") == 0)
            crypto_cipher_decrypt_one(tfm, one_cipher, one_data);

	// luu khoi da duoc ma hoa hoac giai ma vao buffer
        for (j = 0; j < 16; j++)
            cipher[i * 16 + j] = one_cipher[j];

        printk("one cipher: %s\n", one_cipher);
    }

    // convert tu hex sang string
    hextostring(cipher, data_len, hex_cipher);
    printk("hex cipher: %s\n", hex_cipher);

    // copy du lieu da ma hoa hoac giai ma cho nguoi dung
    copy_to_user(user_buf, hex_cipher, data_len * 2);

    return 0;
}


// copy du lieu tu user xuong nhan 
static ssize_t write_fun(struct file *file, const char *user_buff, size_t len, loff_t *off)
{
    char buffer[1000], hex_data[1000];
    int i, j;

    // setup bo nho
    memset(buffer, 0, sizeof(buffer));
    memset(data, 0, sizeof(data));
    memset(type, 0, sizeof(type));
    memset(hex_data, 0, sizeof(hex_data));

    // copy bo nho tu usser xuong nhan
    copy_from_user(buffer, user_buff, len);

    // cat lenh tu buffer
    i = 0;
    j = 0;
    while (buffer[i] != '\n' && j < len)
    {
        type[i] = buffer[j];
        i++;
        j++;
    }

    // cat data tu buffer
    i = 0;
    j++;
    while (j < len)
    {
        hex_data[i] = buffer[j];
        i++;
        j++;
    }

    printk("type: %s\n", type);
    printk("hex_data: %s\n", hex_data);

    memset(buffer, 0, sizeof(buffer));

    // convert tu hex sang string
    stringtohex(hex_data, strlen(hex_data), data);
    printk("data: %s\n", data);

    // add padding cho aes
    if (strlen(hex_data) % 32 == 0)
        data_len = ((uint16_t)(strlen(hex_data) / 32)) * 16;
    else
        data_len = ((uint16_t)((strlen(hex_data) / 32) + 1)) * 16;
    return 0;
}


// gan ham chuc nang cho file driver
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = read_fun,
    .write = write_fun,
    .open = open_fun,
    .release = release_fun};

static int md_init(void)
{
    printk("cai dat module\n");

    // cap phat mo ma hoa aes
    tfm = crypto_alloc_cipher("aes", 0, 0);// cap phat mac dinh

    // set key 16 ky tu = 128 bit
    crypto_cipher_setkey(tfm, key, 16);
   
    // tao file driver
    alloc_chrdev_region(&dev_num, 0, 1, "tenthietbi");// cap phat name
    device_class = class_create(THIS_MODULE, "class"); // ''' class
    device_create(device_class, NULL, dev_num, NULL, "aes_encrypt"); // tao file drive aes_encrypt

    kernel_buffer = kmalloc(MEM_SIZE, GFP_KERNEL); // khoi tao bo nho cho nhan
 
    char_device = cdev_alloc();  // khoi tao char driver
    cdev_init(char_device, &fops); // gan fops cho char driver 
    cdev_add(char_device, dev_num, 1); // add char driver vao he thong

    return 0;
}

static void md_exit(void)
{

    crypto_free_cipher(tfm); // giai phong bo ma hoa aes
    cdev_del(char_device); // xoa char dirver khoi he thong
    kfree(kernel_buffer);  // giai phong bo nho nhan
    device_destroy(device_class, dev_num); // huy thiet bi
    class_destroy(device_class); // huy class
    unregister_chrdev_region(dev_num, 1); // huy dang ki char dirver
    printk("thoat module\n");
}

module_init(md_init);
module_exit(md_exit);

MODULE_LICENSE("GPL");
