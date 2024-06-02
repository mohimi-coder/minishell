/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_builtins.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 18:57:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/02 18:56:35 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_env(t_env *env)
{
	while (env)
	{
		if (env->val)
			printf("%s%s\n", env->key, env->val);
		env = env->next;
	}
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

void	ft_unset(t_env **env, t_token *token)
{
	char	*s;
	int		i;
	t_env	*tmp;

	if (!token->next)
	{
		ft_error_message(RED BOLD "➥  not enough arguments❗" RESET);
		return ;
	}
	s = token->next->next->content;
	i = 0;
	while (s[i])
	{
		if ((!(f_alnum(s[i]) || s[i] == '_')) || (s[0] >= '0' && s[0] <= '9'))
		{
			ft_error_message(RED BOLD "➥  invalid parameter name❗" RESET);
			return ;
		}
		i++;
	}
	tmp = ft_check_var(s, *env);
	if (tmp)
		ft_unset_var(tmp, env);
}

void	ft_pwd(void)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	printf("%s\n", pwd);
	free (pwd);
}

void	ft_builtins(t_token *token, t_env **env)
{
	while (token)
	{
		if (!ft_strcmp(token->content, "env"))
			ft_env(*env);
		if (!ft_strcmp(token->content, "export"))
			ft_export_var(*env, token);
		if (!ft_strcmp(token->content, "unset"))
			ft_unset(env, token);
		if (!ft_strcmp(token->content, "pwd"))
			ft_pwd();
		if (!ft_strcmp(token->content, "cd"))
			ft_cd(token);
		if (!ft_strcmp(token->content, "exit"))
			ft_exit(token);
		token = token->next;
	}
}
