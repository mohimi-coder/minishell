/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 14:43:38 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/04 12:37:52 by zait-bel         ###   ########.fr       */
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

void	ft_exit(t_token *token)
{
	char	*s;

	if (!token->next)
	{
		printf("exit\n");
		exit(0);
	}
	s = token->next->next->content;
	exit(ft_atoi(s));
}
