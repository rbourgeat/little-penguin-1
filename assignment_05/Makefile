# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rbourgea <rbourgea@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/24 17:52:28 by rbourgea          #+#    #+#              #
#    Updated: 2022/05/27 12:38:29 by rbourgea         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MODULE_NAME = fortytwo

obj-m += $(MODULE_NAME).o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean