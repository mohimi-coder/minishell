/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 14:43:38 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/09 19:22:37 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_atoi(char *str)
{
	int	i;
	int	sign;
	int	res;

	i = 0;
	sign = 1;
	res = 0;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	if (!str[i])
		(ft_error_message(RED BOLD"numeric argument required❗" RESET), exit(0));
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + str[i] - 48;
		i++;
	}
	if (str[i])
		(ft_error_message(RED BOLD"numeric argument required❗" RESET), exit(0));
	return (res * sign);
}

void	ft_exit(char **cmd)
{
	if (!cmd[1])
	{
		printf("exit\n");
		exit(0);
	}
	exit(ft_atoi(cmd[1]));
}
