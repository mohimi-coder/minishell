/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 22:39:53 by mohimi            #+#    #+#             */
/*   Updated: 2024/07/29 16:44:25 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_strchr_n(char *s, char c)
{
	int	i;

	i = 0;
	if (!s[i])
		return (0);
	while (s[i] && s[i] == c)
		i++;
	if (s[i])
		return (0);
	return (1);
}

void	ft_echo(t_list *list)
{
	int	i;
	int	count;

	(1) && (i = 1, count = 0);
	if (!list)
		return ;
	while ((list && list->cmd[i]) && (list->cmd[i][0] == '-'
		&& ft_strchr_n(list->cmd[i] + 1, 'n')))
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
