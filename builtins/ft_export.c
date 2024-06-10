/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 19:09:19 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/10 09:29:07 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_export(t_env *env)
{
	t_env	*tmp;
	int		i;

	tmp = env;
	i = 0;
	ft_index(tmp, env);
	while (tmp)
	{
		if (tmp->index == i)
		{
			printf(YELLOW"declare -x %s"RESET, tmp->key);
			if (tmp->val)
			{
				printf("%c", tmp->val[0]);
				printf(GREEN"\"%s\""RESET, tmp->val + 1);
			}
			printf("\n");
			i++;
			tmp = env;
		}
		else
			tmp = tmp->next;
	}
}

void	ft_concatenate(char **sp, char *str, t_env *new, t_env *env)
{
	char	*key;

	key = ft_substr(sp[0], 0, ft_strlen(sp[0]) - 1);
	if (ft_check_var(key, env))
	{
		new = ft_check_var(key, env);
		free (key);
		if (!new->val)
			new->val = ft_strdup(ft_strchr(str, '='));
		else
			new->val = ft_strjoin(new->val, ft_strdup(ft_strchr(str, '=') + 1));
		return ;
	}
	new = lstnew_env(ft_strdup(key),
			ft_strdup(ft_strchr(str, '=')));
	(add_back_env(&env, new), free(key));
}

int	add_export(char **s, char *str, t_env *new, t_env *env)
{
	int	i;

	i = 0;
	while (s[0][i])
	{
		if (s[0][i] == '+' && !s[0][i + 1] && ft_strchr(str, '='))
		{
			ft_concatenate(s, str, new, env);
			ft_free_leak(s);
			return (1);
		}
		if ((!(f_alnum(s[0][i]) || s[0][i] == '_'))
			|| (s[0][0] >= '0' && s[0][0] <= '9'))
		{
			printf("%s ➥ %s: not an identifier❗%s\n", RED BOLD, s[0], RESET);
			return (1);
		}
		i++;
	}
	return (0);
}

void	ft_update(char *str, t_env *env, t_env *new, char **s)
{
	if (ft_strchr(str, '='))
	{
		new = ft_check_var(s[0], env);
		free(new->val);
		new->val = ft_strdup(ft_strchr(str, '='));
	}
	ft_free_leak(s);
}

void	ft_export_var(t_env *env, char **cmd)
{
	char	**s;
	t_env	*new;
	int		i;

	if (!cmd[1])
		return (ft_export(env));
	i = 1;
	while (cmd[i])
	{
		(1) && (new = NULL, s = ft_split(cmd[i], '='));
		if (!s)
			return ;
		if (!add_export(s, cmd[i], new, env))
		{
			if (ft_check_var(s[0], env))
				ft_update(cmd[i], env, new, s);
			else
			{
				new = lstnew_env(ft_strdup(s[0]),
						ft_strdup(ft_strchr(cmd[i], '=')));
				(add_back_env(&env, new), ft_free_leak(s));
			}
		}
		i++;
	}
}
