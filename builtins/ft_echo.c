/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 22:39:53 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/10 12:27:49 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_echo(t_list *list)
{
	int i;
	int count;

	(1) && (i = 1, count = 0);
	while(list && list->cmond[i] && !ft_strcmp(list->cmond[i], "-n"))
	{
		count++;
		i++;
	}
	while(list && list->cmond && list->cmond[i])
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
