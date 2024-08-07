/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 14:43:38 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/29 16:44:31 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_atoi(char *str)
{
	int	i;
	int	sign;
	int	res;

	1 && (i = 0, sign = 1, res = 0);
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	if (!str[i])
		(write(2, "exit\n", 5), write(2, "num arg reqrd❗\n", 18), exit(255));
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + str[i] - 48;
		i++;
	}
	if (str[i])
		(write(2, "exit\n", 5), write(2, "num arg reqrd❗\n", 18), exit(255));
	return (res * sign);
}

void	ft_exit(char **cmd)
{
	if (!cmd[1])
	{
		write(2, "exit\n", 5);
		exit(ft_status(0, false));
	}
	if (ft_atoi(cmd[1]) && cmd[2])
	{
		write(2, "exit\n", 6);
		write(2, "too many arguments\n", 19);
		ft_status(1, true);
	}
	else
	{
		write(2, "exit\n", 5);
		exit(ft_atoi(cmd[1]));
	}
}
