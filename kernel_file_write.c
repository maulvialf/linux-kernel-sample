#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/file.h>
#include <linux/fcntl.h>

MODULE_DESCRIPTION("A simple kernel module that writes to /tmp/kernelwrite");

static int __init my_module_init(void)
{
    struct file *filp;
    ssize_t ret;
    const char *text = "Hello from kernel space!\n";
    loff_t pos = 0;

    printk(KERN_INFO "my_module: Loading module and attempting to write to /tmp/kernelwrite.\n");

    /* Open (or create) the file with write permissions and truncate any existing content */
    filp = filp_open("/tmp/kernelwrite", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (IS_ERR(filp)) {
        printk(KERN_ERR "my_module: Failed to open or create file /tmp/kernelwrite.\n");
        return PTR_ERR(filp);
    }

    /* Write the message to the file using kernel_write.
       Since 'text' is in kernel space, we don't need to change the address limit.
    */
    ret = kernel_write(filp, text, strlen(text), &pos);
    if (ret < 0)
        printk(KERN_ERR "my_module: Error writing to /tmp/kernelwrite, error: %zd\n", ret);
    else
        printk(KERN_INFO "my_module: Successfully wrote %zd bytes to /tmp/kernelwrite.\n", ret);

    /* Close the file */
    filp_close(filp, NULL);

    return 0;
}

static void __exit my_module_exit(void)
{
    printk(KERN_INFO "my_module: Unloading module.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);
