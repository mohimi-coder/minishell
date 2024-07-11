/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_helper.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 10:50:48 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/06 20:59:30 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	**ft_expand_var_sp(char *str, t_env *env)
{
	int		i;

	i = 0;
	while (env)
	{
		if (!ft_strcmp(env->key, str + 1))
			if (env->val)
				return (free(str), ft_split(env->val + 1, ' '));
		env = env->next;
	}
	return (NULL);
}

void	ft_delone(t_token *lst)
{
	if (!lst)
		return ;
	free(lst->content);
	free(lst);
	lst = NULL;
}

void	ft_remove_node(t_token *tmp, t_token **lst)
{
	t_token	*current;
	t_token	*previous;

	current = *lst;
	previous = NULL;
	while (current)
	{
		if (current == tmp)
		{
			if (previous)
				previous->next = current->next;
			else
				*lst = current->next;
			ft_delone(current);
			return ;
		}
		previous = current;
		current = current->next;
	}
}

void	ft_add_node(t_token *tmp, t_env *env)
{
	char	**node;
	t_token	*new;
	t_token	*next_node;
	int		i;

	node = ft_expand_var_sp(tmp->content, env);
	if (!node)
	{
		tmp->content = NULL;
		return ;
	}
	tmp->content = node[0];
	i = 1;
	next_node = tmp->next;
	while (node && node[i])
	{
		new = ft_lstnew(SPC, ft_strdup(" "));
		tmp->next = new;
		tmp = new;
		new = ft_lstnew(VAR, node[i]);
		tmp->next = new;
		tmp = new;
		i++;
	}
	tmp->next = next_node;
}
