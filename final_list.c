/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 08:58:03 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/09 10:13:39 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void ft_cmd(t_token *tmp_tmp, int count_cmd, t_list **final)
{
	char	**str;
	t_token	*redire;
	int		i;
	
	(1) && (str = NULL, redire = NULL, i = 0);
	str = malloc(sizeof(char *) * (count_cmd + 1));
	if (!str)
		return;
	while (tmp_tmp && tmp_tmp->type != PIPE)
	{
		if (tmp_tmp->type == CMD)
        {
			str[i] = ft_strdup(tmp_tmp->content);
			i++;
		}
		else if (tmp_tmp->type == APPEND_OUT || tmp_tmp->type == IN \
			|| tmp_tmp->type == OUT ||tmp_tmp->type == HER_DOC)
			add_back(&redire, ft_lstnew(tmp_tmp->type, ft_strdup(tmp_tmp->content)));
		tmp_tmp = tmp_tmp->next;
	}
	str[i] = NULL;
	add_back_mdl(final, ft_lstnew_mdl(str, redire));
}

t_list *ft_finall(t_token *head)
{
	t_list	*final;
	t_token	*tmp;
	int		count_cmd;
	t_token	*tmp_tmp;

	(1) && (final = NULL, tmp = head);
	while(tmp)
	{
		(1) && (tmp_tmp = tmp, count_cmd = 0);
		while(tmp && tmp->type != PIPE)
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
