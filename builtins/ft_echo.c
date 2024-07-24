/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 22:39:53 by mohimi            #+#    #+#             */
/*   Updated: 2024/07/24 21:06:22 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_echo(t_list *list)
{
	int	i;
	int	count;

	(1) && (i = 1, count = 0);
	if (!list)
		return ;
	while ((list && list->cmd[i]) && (list->cmd[i][0] == '-'
		&& ft_strchr(list->cmd[i] + 1, 'n')))
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
