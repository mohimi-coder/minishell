/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/07 13:04:28 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/22 18:52:32 by mohimi           ###   ########.fr       */
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

void	initialize_default_env(t_env **envr)
{
	add_back_env(envr, lstnew_env(ft_strdup("PWD"), \
		ft_strjoin(ft_strdup("="), ft_strdup(getcwd(NULL, 0)))));
	add_back_env(envr, lstnew_env(ft_strdup("SHLVL"), ft_strdup("=2")));
	add_back_env(envr, lstnew_env(ft_strdup("OLDPWD"), NULL));
	add_back_env(envr, lstnew_env(ft_strdup("PATH"), \
ft_strdup("=/usr/gnu/bin:/usr/local/bin:/bin:/usr/bin:.")));
	add_back_env(envr, lstnew_env(ft_strdup("_"), ft_strdup("=/usr/bin/env")));
}

void	ft_fill_env(char **env, t_env **envr)
{
	int		i;
	char	**sp;

	i = 0;
	if (!env[i])
	{
		initialize_default_env(envr);
		return ;
	}
	while (env[i])
	{
		sp = ft_split(env[i], '=');
		if (!sp)
			return ;
		if (!ft_strcmp(sp[0], "OLDPWD"))
			add_back_env(envr, lstnew_env(ft_strdup("OLDPWD"), NULL));
		else
			(add_back_env(envr, lstnew_env(ft_strdup(sp[0]),
						ft_strdup(ft_strchr(env[i], '=')))));
		ft_free_leak(sp);
		i++;
	}
	return ;
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
