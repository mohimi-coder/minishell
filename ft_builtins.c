/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_builtins.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 18:57:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/09 19:34:39 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	// free (pwd);
}

void	ft_builtins(t_list *token, t_env **env)
{
	int	i;

	i = 0;
	while (token)
	{
	
		if (!ft_strcmp(token->cmond[0], "env"))
			ft_env(*env);
		if (!ft_strcmp(token->cmond[0], "export"))
			ft_export_var(*env, token->cmond);
		if (!ft_strcmp(token->cmond[0], "unset"))
			ft_unset(env, token->cmond);
		if (!ft_strcmp(token->cmond[0], "pwd"))
			ft_pwd();
		if (!ft_strcmp(token->cmond[0], "cd"))
			ft_cd(token->cmond);
		if (!ft_strcmp(token->cmond[0], "exit"))
			ft_exit(token->cmond);
		token = token->next;
	}
}

int	ft_strcmp(char const *s1, char const *s2)
{
	size_t	i;

	i = 0;
	if (!s1 || !s2)
		return (1);
	while ((s1[i] && s2[i]))
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
