/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_builtins.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 18:57:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/29 16:44:19 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_putstr_fd(char *s, int fd)
{
	int	i;

	i = 0;
	while (s[i] != '\0')
	{
		write(fd, &s[i], 1);
		i++;
	}
}

void	ft_pwd(void)
{
	char	*pwd;
	char	buff[4096];

	pwd = getcwd(buff, 4096);
	printf("%s\n", buff);
	ft_status(0, true);
}

int	ft_builtins(t_list *token, t_env **env)
{
	int	i;

	i = 0;
	if (check_red_files(token) == -1)
		return (write(2, "error in opening files\n", 24), ft_status(1, true));
	if (check_red_files(token) == -2)
		return (write(2, "ambiguous redirect\n", 20), ft_status(1, true));
	if (token->cmd && token->cmd[0]
		&& !ft_strcmp(ft_tolower(token->cmd[0]), "env") && !token->cmd[1])
		(ft_env(*env), i = 1);
	if (token->cmd && token->cmd[0] && !ft_strcmp(token->cmd[0], "export"))
		(ft_export_var(*env, token->cmd), i = 1);
	if (token->cmd && token->cmd[0] && !ft_strcmp(token->cmd[0], "unset"))
		(ft_unset(env, token->cmd), i = 1);
	if (token->cmd && token->cmd[0]
		&& !ft_strcmp(ft_tolower(token->cmd[0]), "pwd"))
		(ft_pwd(), i = 1);
	if (token->cmd && token->cmd[0] && !ft_strcmp(token->cmd[0], "cd"))
		(ft_cd(token->cmd, env), i = 1);
	if (token->cmd && token->cmd[0] && !ft_strcmp(token->cmd[0], "exit"))
		(ft_exit(token->cmd), i = 1);
	if (token->cmd && token->cmd[0]
		&& !ft_strcmp(ft_tolower(token->cmd[0]), "echo"))
		(ft_echo(token), i = 1);
	return (i);
}

int	ft_strcmp(char const *s1, char const *s2)
{
	size_t	i;

	i = 0;
	if (!s1 || !s2)
		return (1);
	while (s1[i] != '\0' && s2[i] != '\0')
	{
		if (s1[i] != s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

char	*ft_strchr(char *s, char c)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == c)
		{
			return ((char *)s + i);
		}
		i++;
	}
	if (s[i] == c)
		return (s + i);
	return (0);
}
