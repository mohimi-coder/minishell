/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/13 07:46:32 by mohimi            #+#    #+#             */
/*   Updated: 2024/07/23 07:50:54 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_open_files(t_token *list, int mode)
{
	int	fd;

	fd = 0;
	if (mode == 0)
		fd = open(list->content, O_RDONLY);
	else if (mode == 1)
		fd = open(list->content, O_WRONLY | O_TRUNC | O_CREAT, 0664);
	else if (mode == 2)
		fd = open(list->content, O_WRONLY | O_APPEND | O_CREAT, 0664);
	if (list->type < 0)
		return (-1);
	if (list->type == HER_DOC || list->type == IN)
		dup2(fd, 0);
	else
		dup2(fd, 1);
	return (mode);
}

int	check_red_files(t_list *list_com)
{
	t_token	*tmp;

	tmp = NULL;
	if (list_com)
		tmp = list_com->red;
	// printf("tmp : %s\n", list_com->red->content);
	while (tmp)
	{
		// printf(" type : %d\n", tmp->type);
		if (tmp->type == HER_DOC || tmp->type == IN)
		{
			if (ft_open_files(tmp, 0) < 0)
				return (-1);
		}
		else if (tmp->type == OUT)
		{
			if (ft_open_files(tmp, 1) < 0)
				return (-1);
		}
		else if (tmp->type == APPEND_OUT)
		{
			if (ft_open_files(tmp, 2) < 0)
				return (-1);
		}
		tmp = tmp->next;
	}
	return (0);
}
