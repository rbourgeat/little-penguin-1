/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 16:07:48 by rbourgea          #+#    #+#             */
/*   Updated: 2022/05/30 14:53:29 by rbourgea         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raphaël Bourgeat <rbourgea@student.42.fr>");
MODULE_DESCRIPTION("A simple Hello World Linux module.");
MODULE_VERSION("1.0");

static int __init lkm_hello_init(void)
{
	printk(KERN_INFO "Hello, World !\n");
	return 0;
}

static void __exit lkm_hello_exit(void)
{
	printk(KERN_INFO "Cleaning up module.\n");
}

module_init(lkm_hello_init);
module_exit(lkm_hello_exit);
