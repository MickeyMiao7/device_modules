#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#define SCULL_IOC_MAGIC 'k'
#define SCULL_IOC_MAXNR 14
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_WRITE _IOW(SCULL_IOC_MAGIC, 2, char*)
#define SCULL_READ _IOR(SCULL_IOC_MAGIC, 3, char*)
#define SCULL_WRITE_READ _IOWR(SCULL_IOC_MAGIC, 4, char*)


#define MAJOR_NUMBER 61
#define SIZE 4 * 1024 * 1024
#define DEV_MSG_SIZE 100
 
/* forward declaration */
int four_open(struct inode *inode, struct file *filep);
int four_release(struct inode *inode, struct file *filep);
ssize_t four_read(struct file *filep, char *buf, size_t
		count, loff_t *f_pos);
ssize_t four_write(struct file *filep, const char *buf,
		size_t count, loff_t *f_pos);
static void four_exit(void);

loff_t four_llseek(struct file *filep, loff_t off, int whence);
long four_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

long long int total = 1;

/* definition of file_operation structure */
struct file_operations four_fops = {
     read:     four_read,
     write:    four_write,
     open:     four_open,
     release:  four_release,
     llseek:   four_llseek,
     unlocked_ioctl: four_ioctl

};

char *four_data = NULL;
char *dev_msg = NULL;

long four_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
    int err = 0, tmp;
    int retval = 0;
    /*
     *     * extract the type and number bitfields, and don't decode
     *         * wrong cmds: return ENOTTY (inappropriate ioctl) before acce    ss_ok() 
    */
    if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
    if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;
    /*
     *     * the direction is a bitmask, and VERIFY_WRITE catches R/W * tran    sfers. `Type' is user‐oriented, while
     *         * access_ok is kernel‐oriented, so the concept of "read" and     * "write" is reversed
     *            
     */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    if (err) return -EFAULT; 
    
    char *user_msg = (char *)arg;
    
    switch(cmd) {
        case SCULL_HELLO:
            printk(KERN_WARNING "hello\n");
            break;

        case SCULL_READ:
            copy_to_user(user_msg, dev_msg, DEV_MSG_SIZE);
            retval = DEV_MSG_SIZE;
            break;

        case SCULL_WRITE:
            copy_from_user(dev_msg, user_msg, DEV_MSG_SIZE);
            retval = DEV_MSG_SIZE;
            break;

        case SCULL_WRITE_READ:
            printk(KERN_ALERT "DEBUG: Before changing, dev_msg is :%s\n", dev_msg);
            char buffer[DEV_MSG_SIZE];
            copy_from_user(buffer, user_msg, DEV_MSG_SIZE);
            copy_to_user(user_msg, dev_msg, DEV_MSG_SIZE);
            int i = 0;
            for(i = 0; i < DEV_MSG_SIZE; i++)
                dev_msg[i] = buffer[i];
            
            printk(KERN_ALERT "DEBUG: After changing, dev_msg is :%s\n", dev_msg);
            retval = DEV_MSG_SIZE;
            break;
            
        default:
            /* redundant, as cmd was checked against MAXNR */
            return -ENOTTY;
    }
    return retval;
}


loff_t four_llseek(struct file *filep, loff_t off, int whence)
{
    printk(KERN_ALERT "offset: %d", (int) off);
    loff_t newops;
    switch(whence){
        case 0:
            // SEEK_SET
            newops = off;
            break;
        case 1:
            // SEEK_CUR
            newops = filep->f_pos + off;
            break;
        case 2:
            // SEEK_END
            newops = total + off;
            break;
        default:
            newops = -1;
    }
    if (newops < 0) 
        return -EINVAL;
    filep->f_pos = newops;
    return newops;
} 

int four_open(struct inode *inode, struct file *filep)
{
     return 0; // always successful
}

int four_release(struct inode *inode, struct file *filep)
{
     return 0; // always successful
}

ssize_t four_read(struct file *filep, char *buf, size_t
		count, loff_t *f_pos)
{    
    
    if(count == 0 || four_data == NULL)
        return 0;

    int left;
    left = (int)(total - (int)(*f_pos)); 
    
    if(count + (*f_pos) <= total){
	    copy_to_user(buf, four_data + (int)*f_pos, count);
	    *f_pos += count;
	    return count;
    }
    else if((*f_pos) < total){
	    copy_to_user(buf, four_data + (int)*f_pos, left);
	    (*f_pos) = total;
	    return left;
    }
    else
        return 0;
        
}

ssize_t four_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
    if (count == 0 || four_data == NULL)
        return 0;
    if (count > SIZE)
        count = SIZE;
    if ((*f_pos) + count <= SIZE){
        copy_from_user(four_data + (int)*f_pos, buf, count);
        (*f_pos) += count;
        total = *f_pos > total ? *f_pos : total;
        printk(KERN_ALERT "4Mb device: write %d bytes into /dev/four\n", count);
        return count;
    }
    else if ((*f_pos) < SIZE){
        int left;
        left = (int)SIZE - (int)*f_pos;
        copy_from_user(four_data + (int)*f_pos, buf, left);
        *f_pos = SIZE;
        total = *f_pos > total ? *f_pos : total;
        printk(KERN_ALERT "4Mb device: write %d bytes into /dev/four\n", left);
        return left;
    }
    else
        printk(KERN_ALERT "4Mb device: No Space Left");
        return -ENOSPC;
          
}

static int four_init(void)
{
     int result;
     // register the device
     result = register_chrdev(MAJOR_NUMBER, "four",
		     &four_fops);
     if (result < 0) {
	  return result;
     }
     // kmalloc is just like malloc, the second parameter is
     // the type of memory to be allocated.
     // To release the memory allocated by kmalloc, use kfree.
     four_data = kmalloc(sizeof(char) * SIZE, GFP_KERNEL);
     printk(KERN_ALERT "DEBUG: malloc %d bytes for /dev/ioctl\n", (int) sizeof(char) * SIZE);
     dev_msg = kmalloc(sizeof(char) * DEV_MSG_SIZE, GFP_KERNEL);
     printk(KERN_ALERT "DEBUG: malloc %d bytes for dev_msg\n", (int) sizeof(char) * DEV_MSG_SIZE);
     if (!four_data) {
          four_exit();
          // cannot allocate memory
          // return no memory error, negative signify a failure
	  return -ENOMEM;
     }    
     // initialize 
     *four_data = ' ';
     printk(KERN_ALERT "This is a 4Mb device module\n");
     return 0;
}

static void four_exit(void)
{
     // if the pointer is pointing to something
     if (four_data) {
          // free the memory and assign the pointer to NULL
          kfree(four_data);
	  four_data = NULL;
     }    
     // unregister the device
     unregister_chrdev(MAJOR_NUMBER, "four");
     printk(KERN_ALERT "Unload the 4Mb device module\n");
}
MODULE_LICENSE("GPL");
module_init(four_init);
module_exit(four_exit);
