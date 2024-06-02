/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 14:43:38 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/02 14:57:46 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_atoi(char *str)
{
	int i = 0;
	int sign = 1;
	int res = 0;
	while((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '+' || str[i] == '-')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while(str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + str[i] - 48;
		i++;
	}
	return(res * sign);
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
