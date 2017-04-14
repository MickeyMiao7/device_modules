#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define MAJOR_NUMBER 61
#define SIZE 4 * 1024 * 1024
 
/* forward declaration */
int four_open(struct inode *inode, struct file *filep);
int four_release(struct inode *inode, struct file *filep);
ssize_t four_read(struct file *filep, char *buf, size_t
		count, loff_t *f_pos);
ssize_t four_write(struct file *filep, const char *buf,
		size_t count, loff_t *f_pos);
static void four_exit(void);

long long int total = 1;

/* definition of file_operation structure */
struct file_operations four_fops = {
     read:     four_read,
     write:    four_write,
     open:     four_open,
     release: four_release
};

char *four_data = NULL;

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
        total = *f_pos;
        printk(KERN_ALERT "4Mb device: write %d bytes into /dev/four\n", count);
        return count;
    }
    else if ((*f_pos) < SIZE){
        int left;
        left = (int)SIZE - (int)*f_pos;
        copy_from_user(four_data + (int)*f_pos, buf, left);
        *f_pos = SIZE;
        total = *f_pos;
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
     printk(KERN_ALERT "DEBUG: malloc %d bytes for /dev/four\n", (int) sizeof(char) * SIZE);
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
