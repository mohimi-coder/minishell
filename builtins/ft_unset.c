/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 13:05:34 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/24 13:37:46 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_lstdelone(t_env *lst)
{
	if (!lst)
		return ;
	free(lst->key);
	free(lst->val);
	free(lst);
	lst = NULL;
}

void	ft_unset_var(t_env *tmp, t_env **env)
{
	t_env	*current;
	t_env	*previous;

	current = *env;
	previous = NULL;
	while (current)
	{
		if (current == tmp)
		{
			if (previous)
				previous->next = current->next;
			else
				*env = current->next;
			ft_lstdelone(current);
			return ;
		}
		previous = current;
		current = current->next;
	}
}

bool	is_invalid_parameter_name(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if ((!(f_alnum(s[i]) || s[i] == '_')) || (s[0] >= '0' && s[0] <= '9'))
		{
			(ft_error_message("invalid parameter name❗"), (ft_status(1, true)));
			break ;
		}
		i++;
	}
	return (false);
}

void	ft_unset(t_env **env, char **cmd)
{
	char	*s;
	int		i;
	int		j;

	if (!cmd[1])
	{
		(ft_error_message("➥ not enough arguments❗"), (ft_status(1, true)));
		return ;
	}
	j = 1;
	while (cmd[j])
	{
		(1) && (s = cmd[j], i = 0);
		if (is_invalid_parameter_name(s))
			break ;
		if (ft_check_var(s, *env))
			(ft_unset_var(ft_check_var(s, *env), env), ft_status(0, true));
		j++;
	}
}
