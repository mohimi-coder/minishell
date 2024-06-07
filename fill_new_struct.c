/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_new_struct.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 11:32:27 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/07 12:05:05 by zait-bel         ###   ########.fr       */
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

t_token	*new_list(t_token *head)
{
	t_token	*new;
	char	*join;

	new = NULL;
	while (head)
	{
		if (is_cmd(head->type))
		{
			join = NULL;
			while (head && is_cmd(head->type))
			{
				join = ft_strjoin2(join, head->content);
				head = head->next;
			}
			add_back(&new, ft_lstnew(CMD, ft_strdup(join)));
		}
		else if (head->type == SPC)
			head = head->next;
		else
		{
			add_back(&new, ft_lstnew(head->type, ft_strdup(head->content)));
			head = head->next;
		}
	}
	return (new);
}
