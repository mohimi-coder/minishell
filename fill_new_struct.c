/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_new_struct.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 11:32:27 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/08 16:41:02 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_strjoin2(char *s1, char *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (s2);
	if (!s2)
		return (s1);
	i = 0;
	j = 0;
	str = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!str)
		return (NULL);
	while (s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	while (s2[j])
		str[i++] = s2[j++];
	str[i] = '\0';
	return (str);
}

int	is_cmd(int type)
{
	if (type == WORD || type == DOUBLE_QUOTES || type == SINGLE_QUOTE
		|| type == DOUBLE_DOLLAR || type == DOLLAR_WHY || type == VAR)
		return (1);
	return (0);
}

void	skip_red(t_token **lst, t_token *new)
{
	if ((*lst)->type == PIPE)
	{
		add_back(&new, ft_lstnew((*lst)->type, ft_strdup((*lst)->content)));
		*lst = (*lst)->next;
	}
	else
	{
		if ((*lst)->next->type == SPC)
		{
			add_back(&new, ft_lstnew((*lst)->type,
					ft_strdup((*lst)->next->next->content)));
			(*lst) = (*lst)->next;
		}
		else
			add_back(&new, ft_lstnew((*lst)->type,
					ft_strdup((*lst)->next->content)));
		*lst = (*lst)->next->next;
	}
}

t_token	*new_list(t_token *lst)
{
	t_token	*new;
	char	*join;

	new = NULL;
	while (lst)
	{
		if (is_cmd(lst->type))
		{
			join = NULL;
			while (lst && is_cmd(lst->type))
			{
				join = ft_strjoin2(join, lst->content);
				lst = lst->next;
			}
			add_back(&new, ft_lstnew(CMD, ft_strdup(join)));
		}
		else if (lst->type == SPC)
			lst = lst->next;
		else
			skip_red(&lst, new);
	}
	return (new);
}
