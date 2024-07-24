/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/13 07:46:32 by mohimi            #+#    #+#             */
/*   Updated: 2024/07/24 12:18:53 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	ft_open_files(t_token *list, int mode)
{
	int	fd;

	fd = 0;
	if (list->content[0] == '$')
		return (-2);
	else if (mode == 0)
		fd = open(list->content, O_RDONLY);
	else if (mode == 1)
		fd = open(list->content, O_WRONLY | O_TRUNC | O_CREAT, 0664);
	else if (mode == 2)
		fd = open(list->content, O_WRONLY | O_APPEND | O_CREAT, 0664);
	if (fd < 0)
		return (-1);
	if (list->type == HER_DOC || list->type == IN)
		(dup2(fd, 0), close(fd));
	else
		(dup2(fd, 1), close(fd));
	return (mode);
}

int	check_red_files(t_list *list_com)
{
	t_token	*tmp;

	tmp = NULL;
	if (list_com)
		tmp = list_com->red;
	while (tmp)
	{
		if (tmp->type == HER_DOC || tmp->type == IN)
			return (ft_open_files(tmp, 0));
		else if (tmp->type == OUT)
			return (ft_open_files(tmp, 1));
		else if (tmp->type == APPEND_OUT)
			return (ft_open_files(tmp, 2));
		tmp = tmp->next;
	}
	return (0);
}

void	ft_perror(char *str)
{
	perror(str);
	exit(1);
}
