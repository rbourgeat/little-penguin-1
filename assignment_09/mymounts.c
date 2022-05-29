/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mymounts.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/28 12:13:55 by rbourgea          #+#    #+#             */
/*   Updated: 2022/05/29 17:34:46 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Sources: https://www.kernel.org/doc/htmldocs/kernel-api/index.html
// https://www.kernel.org/doc/htmldocs/kernel-api/API-list-for-each-entry.html
// https://www.kernel.org/doc/htmldocs/kernel-api/API-snprintf.html
// https://elixir.bootlin.com/linux/latest/ident/dentry_path_raw
// https://elixir.bootlin.com/linux/latest/source/fs/mount.h#L34 <== struct
// https://elixir.bootlin.com/linux/latest/source/include/linux/nsproxy.h#L31 <== namespace

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/string.h>

#include <linux/mount.h>
#include <linux/proc_fs.h>
#include <linux/nsproxy.h>
#include <linux/slab.h>
#include <../fs/mount.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raphaël Bourgeat <rbourgea@student.42.fr>");
MODULE_DESCRIPTION("A listing mount points module.");
MODULE_VERSION("1.0");

static struct proc_dir_entry *mymounts;

static int set_path(struct mount *mnt, char **buffer, int *i)
{
	char path[256];

	if (strcmp(mnt->mnt_parent->mnt_mountpoint->d_name.name, "/") != 0)
		set_path(mnt->mnt_parent, buffer, i);
	*i += snprintf(*buffer + *i, PAGE_SIZE - *i, "%s", dentry_path_raw(mnt->mnt_mountpoint, path, sizeof(path)));
	return *i;
}

static ssize_t mymounts_read(struct file *file, char __user *buf, size_t count,
			loff_t *f_pos)
{
	char *buffer;
	struct mount *mnt;
	char path[256];
	int ret = 0;
	int i = 0;

	buffer = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;
	// get all mountpoints / checking all dentries of a system
	list_for_each_entry(mnt, &current->nsproxy->mnt_ns->list, mnt_list) {
	// if (strcmp(mnt->mnt_devname, "none") == 0)
	// 	continue;
	if (strcmp(mnt->mnt_devname, "/dev/root") == 0)
		continue;
	// pr_info("[%s]", mnt->mnt_devname);
	memset(path, 0, sizeof(path));
	// 3 cases: /dev/root, / and others
	if (strcmp(mnt->mnt_devname, "/dev/root") == 0) {
		i += snprintf((&(buffer[i])), PAGE_SIZE - i, "%-8s", "root");
	}
	else if (strcmp(mnt->mnt_mountpoint->d_name.name, "/") == 0) {
		i += snprintf((&(buffer[i])), PAGE_SIZE - i, "%s%-*s", \
			mnt->mnt_parent->mnt_mountpoint->d_name.name, \
			(int)(8 - strlen(mnt->mnt_parent->mnt_mountpoint->d_name.name)), \
			mnt->mnt_mountpoint->d_name.name);
	}
	else {
		i += snprintf((&(buffer[i])), PAGE_SIZE - i, "%-8s", mnt->mnt_mountpoint->d_name.name);
	}
	// Adding path
	if (strcmp(mnt->mnt_parent->mnt_mountpoint->d_name.name, "/") != 0)
		set_path(mnt->mnt_parent, &buffer, &i);
	// Adding name
	i += snprintf((&(buffer[i])), PAGE_SIZE - i, "%s\n", \
		dentry_path_raw(mnt->mnt_mountpoint, path, sizeof(path)));
	}
	ret = simple_read_from_buffer(buf, count, f_pos, buffer, strlen(buffer));
	kfree(buffer);
	return ret;
}

struct proc_ops mymounts_ops = {
	.proc_read = mymounts_read,
};

int __init mymounts_init(void)
{
	mymounts = proc_create("mymounts", 0660, NULL, &mymounts_ops);
	return 0;
}

void __exit mymounts_exit(void)
{
	proc_remove(mymounts);
}

module_init(mymounts_init);
module_exit(mymounts_exit);