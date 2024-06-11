/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 22:39:53 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/10 17:40:28 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	n_check(char c, char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (c != str[i])
			return (1);
		i++;
	}
	return (0);
}

void	ft_echo(t_list *list)
{
	int	i;
	int	count;

	(1) && (i = 1, count = 0);
	while (list && list->cmond[i] && (list->cmond[i][0] == '-'
		&& !n_check('n', list->cmond[i] + 1)))
	{
		count++;
		i++;
	}
	while (list && list->cmond && list->cmond[i])
	{
		if (list->cmond[i])
		{
			ft_putstr_fd(list->cmond[i], 1);
			if (list->cmond[i + 1])
				ft_putstr_fd(" ", 1);
		}
		i++;
	}
	if (count == 0)
		ft_putstr_fd("\n", 1);
	return ;
}
