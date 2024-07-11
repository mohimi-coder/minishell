/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_helper.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:22:08 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/07 16:36:23 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	**ft_free2(char **arr)
{
	size_t	j;

	if (!arr)
		return (NULL);
	j = 0;
	while (arr[j])
	{
		free (arr[j]);
		j++;
	}
	free(arr);
	return (NULL);
}

char	*get_path(char	*argv, char	**env)
{
	char		**path;
	char		*command;
	char		*res;
	int			i;
	struct stat	file;

	i = 0;
	if (stat(argv, &file) == 0 && S_ISDIR(file.st_mode))
		(dprintf(2, "%s%s :is a directory❗%s\n", RED BOLD, argv, RESET),
			exit(126));
	if (argv[0] == '/')
		return (ft_strdup(argv));
	else if (argv[0] == '.')
		return (ft_strdup(argv + 2));
	path = split_path(env);
	command = ft_strjoin(ft_strdup("/"), ft_strdup(argv));
	while (path[i])
	{
		res = ft_strjoin(ft_strdup(path[i++]), ft_strdup(command));
		if (access(res, F_OK) == 0)
			return (free(command), ft_free2(path), res);
		free(res);
	}
	(dprintf(2, "%s%s: command not found❗%s\n", RED BOLD, argv, RESET),
		exit(1));
}

char	**split_path(char **envp)
{
	int		i;
	char	**path;

	path = NULL;
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
		{
			path = ft_split(envp[i] + 5, ':');
			break ;
		}
		i++;
	}
	return (path);
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

void	ft_perror(char *str)
{
	perror(str);
	exit(1);
}
