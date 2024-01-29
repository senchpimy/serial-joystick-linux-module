#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
//#include <linux/fcntl.h>
//#include <string.h>
#include <termios.h>
MODULE_AUTHOR("senchpimy");
MODULE_DESCRIPTION("dsa");
MODULE_VERSION("0.1");
MODULE_LICENSE("GPL");
#define DEVICE_NAME "lkm_example"
//#define EXAMPLE_MSG "Hello, World !\n"
//#define MSG_BUFFER_LEN 15 /* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
 
struct termios tty;
char buffer[2];
char buffer_f[5];
char match_input(void);
int open_serial(void);
int my_atoi(const char*);

struct file* serial_port;

int device_open_count;
int major_num;

static struct file_operations file_ops = {.read = device_read,
                                          .write = device_write,
                                          .open = device_open,
                                          .release = device_release};


//While this dont return 1 it will keep the read open
static ssize_t device_read(struct file *flip, char *buffer, size_t len,
                           loff_t *offset) {
  return 1;
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len,
                            loff_t *offset) {
  printk(KERN_ALERT "This operation is not supported.\n");
  return -EINVAL;
}

static int device_open(struct inode *inode, struct file *file) {
  if (device_open_count) {
    return -EBUSY;
  }
  device_open_count++;
  try_module_get(THIS_MODULE);
  return 0;
}

static int device_release(struct inode *inode, struct file *file) {
  device_open_count--;
  module_put(THIS_MODULE);
  return 0;
}

#define MAJOR_NUM 0
static int __init lkm_example_init(void) {
  //strncpy(msg_buffer, "JOYSTICK MODULE",15 );
  major_num = register_chrdev(MAJOR_NUM, "lkm_example", &file_ops);
  if (major_num < 0) {
    printk(KERN_ALERT "Could not register device : % d\n", major_num);
    return major_num;
  }
  if (open_file()>0){
    return 1;
  }

  printk(KERN_INFO "lkm_example module loaded with device major number %d\n ",
         major_num);
  return 0;
}

static void __exit lkm_example_exit(void) {
  filp_close(serial_port,0);
  unregister_chrdev(major_num, DEVICE_NAME);
  printk(KERN_INFO "Goodbye, World !\n");
}

int open_file(){
  serial_port = filp_open("/dev/ttyS0", O_RDWR,0); // Adjust the port as needed

  if (serial_port < 0) {
    printk(KERN_INFO "Joystick module: Error opening serial port\n");
    return 1;
  }

  return 0;
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
