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
 
/* forward declaration */
int four_open(struct inode *inode, struct file *filep);
int four_release(struct inode *inode, struct file *filep);
ssize_t four_read(struct file *filep, char *buf, size_t
		count, loff_t *f_pos);
ssize_t four_write(struct file *filep, const char *buf,
		size_t count, loff_t *f_pos);
static void four_exit(void);

ssize_t read_count;

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
      if (*f_pos == 0 && count > 1){
	// Copy the data from kernel space to user space
	copy_to_user(buf, four_data, read_count);
	// Forward the position pointer
	*f_pos += read_count;
	return read_count;
      }
      else
	return 0;
	
  //   else{
  //      return 0;
  //   }
}

ssize_t four_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
      if (count > 0 && *f_pos == 0){
	// Copy the data from user space to kernel space
	copy_from_user(four_data, buf, count);
	*f_pos += count;
	read_count = count;
	return count;
      }
      else{
 	return -ENOSPC;
      }
          
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
     // allocate one byte of memory for storage
     // kmalloc is just like malloc, the second parameter is
     // the type of memory to be allocated.
     // To release the memory allocated by kmalloc, use kfree.
     four_data = kmalloc(sizeof(char) * 4096, GFP_KERNEL);
     if (!four_data) {
          four_exit();
          // cannot allocate memory
          // return no memory error, negative signify a failure
	  return -ENOMEM;
     }    
     // initialize 
     *four_data = ' ';
     printk(KERN_ALERT "This is a four device module\n");
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
     printk(KERN_ALERT "Onebyte device module is unloaded\n");
}
MODULE_LICENSE("GPL");
module_init(four_init);
module_exit(four_exit);
