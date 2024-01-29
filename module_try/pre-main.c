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
ssize_t bytes_read;
char match_input(void);
int open_serial(void);
int my_atoi(const char*);

int serial_port;

int device_open_count;
int major_num;

static struct file_operations file_ops = {.read = device_read,
                                          .write = device_write,
                                          .open = device_open,
                                          .release = device_release};


//While this dont return 1 it will keep the read open
static ssize_t device_read(struct file *flip, char *buffer, size_t len,
                           loff_t *offset) {
  //if (*msg_ptr == 0) {
  //  msg_ptr = msg_buffer;
  //}
  int index = 0;
  while (1) {
    bytes_read = kernel_read(serial_port, &buffer, sizeof(buffer),0);
    if (bytes_read == 1) {
      if (buffer[0] == '\n') {
        buffer_f[index] = '\0';
        index = 0;
        char res = match_input();
        int y = copy_to_user(buffer, res, 1);
        //continue;
        return 2; //return to write the values
      }
      buffer_f[index] = buffer[0];
      index++;
    }
  }
  return 2;
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
  if (open_serial()>0){
    return 1;
  }

  printk(KERN_INFO "lkm_example module loaded with device major number %d\n ",
         major_num);
  return 0;
}

static void __exit lkm_example_exit(void) {
  filp_close(serial_port,0);
  printk(KERN_INFO "Serial connection closed.\n");
  unregister_chrdev(major_num, DEVICE_NAME);
  printk(KERN_INFO "Goodbye, World !\n");
}

char match_input() {
  int value;
  char pressed = 'p';
  char released = 'r';
  char up = 'u';
  char down = 'd';
  char left = 'l';
  char rigth = 'r';
  char v = buffer_f[0];
  if (v == 'X') {
    value = my_atoi(((char *)buffer_f) + 1);
    if (value > 580) { // rigth
      return rigth;
    } else if (value < 400) { // left
      return left;
    }
    return '\0';
  }
  if (v == 'Y') {
    value = my_atoi(((char *)buffer_f) + 1);
    if (value > 580) { // up
      return up;
    } else if (value < 400) { // down
      return down;
    }
    return '\0';
  }
  if (v == 'b') {
      return up;
    switch (buffer_f[1]) {
    case 'p': // Presed
      return pressed;
      break;
    case 'r': // relesed
      return released;
      break;
    }
  }
    return '\0';
}

int open_serial(){
  serial_port = filp_open("/dev/ttyS0", O_RDWR,0); // Adjust the port as needed

  if (serial_port < 0) {
    printk(KERN_INFO "Joystick module: Error opening serial port\n");
    return 1;
  }

  memset(&tty, 0, sizeof(tty));
  if (tcgetattr(serial_port, &tty) != 0) {
    printk(KERN_INFO "Joystick Module: Error from tcgetattr\n");
    return 1;
  }

  cfsetospeed(&tty, B9600);
  cfsetispeed(&tty, B9600);

  tty.c_cflag &= ~PARENB; // No parity bit
  tty.c_cflag &= ~CSTOPB; // 1 stop bit
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8; // 8 data bits

  tty.c_cflag &= ~CRTSCTS;       // No hardware flow control
  tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem control lines

  tty.c_lflag &= ~ICANON; // Disable canonical mode
  tty.c_lflag &= ~ECHO;   // Disable echo
  tty.c_lflag &= ~ECHOE;  // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT, and SUSP

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control

  tty.c_oflag &= ~OPOST; // Raw output

  tcsetattr(serial_port, TCSANOW, &tty);
  return 0;
}

int my_atoi(const char *str) {
    int result = 0;
    int i = 0;
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }

  return result;
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
