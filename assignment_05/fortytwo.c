/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fortytwo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 18:58:36 by rbourgea          #+#    #+#             */
/*   Updated: 2022/05/26 14:24:33 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Sources: https://embetronicx.com/tutorials/linux/device-drivers/misc-device-driver/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/miscdevice.h> // API MISC DEVICE

#define LOGIN "rbourgea"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raphaël Bourgeat <rbourgea@student.42.fr>");
MODULE_DESCRIPTION("A simple Linux module.");
MODULE_VERSION("1.0");

static ssize_t etx_misc_write(struct file *file, const char __user *buf,
               size_t len, loff_t *ppos)
{
	pr_info("EtX misc device write\n");

	return len; 
}
 
static ssize_t etx_misc_read(struct file *filp, char __user *buf,
                    size_t count, loff_t *f_pos)
{
	pr_info("EtX misc device read\n");

	return 0;
}

static const struct file_operations fops = {
	.owner          = THIS_MODULE,
	.write          = fortytwo_misc_write,
	.read           = fortytwo_misc_read,
	// .open           = fortytwo_misc_open,
	// .release        = fortytwo_misc_close,
	// .llseek         = no_llseek,
};

struct miscdevice fortytwo_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "fortytwo",
	.fops = &fops,
};

static int __init misc_init(void)
{
	int error;

	error = misc_register(&fortytwo_misc_device);
	if (error) {
		pr_err("misc_register failed!!!\n");
		return error;
	}

	pr_info("misc_register init done!!!\n");
	return 0;
}

static void __exit misc_exit(void)
{
	misc_deregister(&fortytwo_misc_device);
	pr_info("misc_register exit done!!!\n");
}

module_init(misc_init);
module_exit(misc_exit);