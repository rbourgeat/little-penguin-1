/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 16:07:48 by rbourgea          #+#    #+#             */
/*   Updated: 2022/05/28 12:00:13 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Sources: https://www.kernel.org/doc/html/latest/filesystems/debugfs.html
// and for lock https://www.kernel.org/doc/html/latest/locking/mutex-design.html

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raphaël Bourgeat <rbourgea@student.42.fr>");
MODULE_DESCRIPTION("A simple debugfs module.");
MODULE_VERSION("1.0");

#define LOGIN "rbourgea"

static DEFINE_MUTEX(lock);
static struct dentry *fortytwo = 0;
static char foo_data[PAGE_SIZE];

static ssize_t id_read(struct file *file, char __user *buf,
			size_t count, loff_t *f_pos)
{
	pr_info("fortytwo misc device read\n");
	return simple_read_from_buffer(buf, count, f_pos, LOGIN,
					strlen(LOGIN));
}
 
static ssize_t id_write(struct file *file, const char __user *buf,
			size_t count, loff_t *f_pos)
{
	char specified_msg[8];
	ssize_t retval = -EINVAL;

	pr_info("fortytwo misc device write\n");
	if (count != strlen(LOGIN))
		return retval;
	retval = simple_write_to_buffer(specified_msg, count, f_pos, buf,
					count);
	if (retval < 0)
		return retval;
	
	retval = strncmp(LOGIN, specified_msg, count) ? -EINVAL : count;
	return retval;
}

static ssize_t jiffies_read(struct file *file, char __user *to, size_t count,
			loff_t *f_pos)
{
	char result[11];

	memset(result, 0, sizeof(result));
	snprintf(result, 11, "%ld", jiffies);
	return simple_read_from_buffer(to, count, f_pos, result, sizeof(result));
}

static ssize_t foo_read(struct file *file, char __user *to, size_t count,
			loff_t *f_pos)
{
	int ret = 0;

	ret = mutex_lock_interruptible(&lock);
	if (ret != 0)
		return ret;
	ret = simple_read_from_buffer(to, count, f_pos, foo_data, strlen(foo_data));
	mutex_unlock(&lock);
	return ret;
}

static ssize_t foo_write(struct file *file, const char __user *from,
			size_t count, loff_t *f_pos)
{
	ssize_t ret;

	if (count >= sizeof(foo_data))
		return -EINVAL;
	ret = mutex_lock_interruptible(&lock);
	if (ret != 0)
		return ret;
	memset(foo_data, 0, sizeof(foo_data));
	ret = simple_write_to_buffer(foo_data, count, f_pos, from, count);
	mutex_unlock(&lock);
	if (ret < 0)
		return ret;
	return count;
}

struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write
};

struct file_operations jiffies_fops = {
	.owner = THIS_MODULE,
	.read = jiffies_read,
};

struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write
};

static int __init lkm_hello_init(void)
{
	printk(KERN_INFO "Hello, World !\n");

	mutex_init(&lock);
	fortytwo = debugfs_create_dir("fortytwo", NULL);
	if (!fortytwo)
	{
		printk(KERN_ERR "CONFIG_DEBUG_FS not set\n");
		return -ENODEV;
	}
	debugfs_create_file("id", 0666, fortytwo, NULL, &id_fops);
	debugfs_create_file("jiffies", 0444, fortytwo, NULL, &jiffies_fops);
	debugfs_create_file("foo", 0644, fortytwo, NULL, &foo_fops);
	return 0;
}

static void __exit lkm_hello_exit(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
	debugfs_remove_recursive(fortytwo);
}

module_init(lkm_hello_init);
module_exit(lkm_hello_exit);
