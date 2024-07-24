/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 22:39:53 by mohimi            #+#    #+#             */
/*   Updated: 2024/07/24 20:47:11 by zait-bel         ###   ########.fr       */
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
	if (!list)
		return ;
	while (list && list->cmd[i] && (list->cmd[i][0] == '-'
		&& !n_check('n', list->cmd[i] + 1)))
		1 && (count++, i++);
	while (list && list->cmd && list->cmd[i])
	{
		if (list->cmd[i])
		{
			ft_putstr_fd(list->cmd[i], 1);
			if (list->cmd[i + 1])
				ft_putstr_fd(" ", 1);
		}
		i++;
	}
	if (count == 0)
		ft_putstr_fd("\n", 1);
	ft_status(0, true);
	return ;
}
