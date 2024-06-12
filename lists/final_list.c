/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 08:58:03 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/12 16:49:04 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_cmd(t_token *tmp, int count_cmd, t_list **final)
{
	char	**str;
	t_token	*redire;
	int		i;

	(1) && (str = NULL, redire = NULL, i = 0);
	str = malloc(sizeof(char *) * (count_cmd + 1));
	if (!str)
		return ;
	while (tmp && tmp->type != PIPE)
	{
		if (tmp->type == CMD)
		{
			str[i] = ft_strdup(tmp->content);
			i++;
		}
		else if (tmp->type == APPEND_OUT || tmp->type == IN \
			|| tmp->type == OUT || tmp->type == HER_DOC)
			add_back(&redire, ft_lstnew(tmp->type, ft_strdup(tmp->content)));
		tmp = tmp->next;
	}
	str[i] = NULL;
	add_back_mdl(final, ft_lstnew_mdl(str, redire));
}

t_list	*ft_finall(t_token *head)
{
	t_list	*final;
	t_token	*tmp;
	int		count_cmd;
	t_token	*tmp_tmp;

	(1) && (final = NULL, tmp = head);
	while (tmp)
	{
		(1) && (tmp_tmp = tmp, count_cmd = 0);
		while (tmp && tmp->type != PIPE)
		{
			if (tmp->type == CMD)
				count_cmd++;
			tmp = tmp->next;
		}
		ft_cmd(tmp_tmp, count_cmd, &final);
		if (tmp)
			tmp = tmp->next;
	}
	return (final);
}
