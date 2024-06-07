/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_builtins.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 18:57:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/07 13:06:13 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_pwd(void)
{
	char	*pwd;
	char	buff[4096];

	pwd = getcwd(buff, 4096);
	printf("%s\n", buff);
	// free (pwd);
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
