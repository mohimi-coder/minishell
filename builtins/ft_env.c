/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 13:04:28 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/10 09:30:17 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_index(t_env *tmp, t_env *env)
{
	t_env	*envr;

	while (tmp)
	{
		tmp->index = 0;
		envr = env;
		while (envr)
		{
			if (ft_strcmp(tmp->key, envr->key) > 0)
				tmp->index++;
			envr = envr->next;
		}
		tmp = tmp->next;
	}
}

t_env	*ft_check_var(char *str, t_env *env)
{
	int		i;

	i = 0;
	while (env)
	{
		if (!ft_strcmp(env->key, str))
			return (env);
		env = env->next;
	}
	return (NULL);
}

void	ft_env(t_env *env)
{
	while (env)
	{
		if (env->val)
			printf("%s%s\n", env->key, env->val);
		env = env->next;
	}
}
