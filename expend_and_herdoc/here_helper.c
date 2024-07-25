/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/12 19:06:30 by mohimi            #+#    #+#             */
/*   Updated: 2024/07/26 00:26:36 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	add_newline(char *str, char *delimiter, char **buff)
{
	char	*tmp;

	if (ft_strcmp(str, delimiter))
	{
		tmp = join_her(*buff, "\n");
		free(*buff);
		*buff = tmp;
	}
}

char	**get_arr_env(t_env **tenv)
{
	char	**env;
	int		count;
	t_env	*tmp;
	int		i;

	tmp = *tenv;
	count = 0;
	i = 0;
	while (tmp)
	{
		count++;
		tmp = tmp->next;
	}
	env = malloc(sizeof(char *) * (count + 1));
	if (!env)
		return (NULL);
	tmp = *tenv;
	while (tmp)
	{
		env[i] = ft_strjoin(ft_strdup(tmp->key), ft_strdup(tmp->val));
		i++;
		tmp = tmp->next;
	}
	env[i] = NULL;
	return (env);
}
