/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/31 19:09:19 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/02 11:47:23 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
			printf("declare -x %s", tmp->key);
			if (tmp->val)
				printf("%s", tmp->val);
			printf("\n");
			i++;
			tmp = env;
		}
		else
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

void	ft_export_var(t_env *env, t_token *token)
{
	char	**s;
	t_env	*new;
	int		i;

	new = NULL;
	if (!token->next || !token->next->next || token->next->next->type != WORD)
	{
		ft_export(env);
		return ;
	}
	(1) && (i = 0, s = ft_split(token->next->next->content, '='));
	if (!s)
		return ;
	while (s[0][i])
	{
		if (s[0][i] == '+' && !s[0][i + 1] && s[1])
		{
			ft_concatenate(s, token->next->next->content, new, env);
			ft_free_leak(s);
			return ;
		}
		if ((!(f_alnum(s[0][i]) || s[0][i] == '_'))
			|| (s[0][0] >= '0' && s[0][0] <= '9'))
		{
			ft_error_message(RED BOLD "➥  not a valid identifier❗" RESET);
			return ;
		}
		i++;
	}
	if (ft_check_var(s[0], env))
	{
		new = ft_check_var(s[0], env);
		free(new->val);
		new->val = ft_strdup(ft_strchr(token->next->next->content, '='));
		ft_free_leak(s);
		return ;
	}
	new = lstnew_env(ft_strdup(s[0]),
			ft_strdup(ft_strchr(token->next->next->content, '=')));
	(add_back_env(&env, new), ft_free_leak(s));
}
