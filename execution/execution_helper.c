/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_helper.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/05 18:22:08 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/26 12:06:08 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	print_no_cmd(char *cmd, char *msg)
{
	write(2, cmd, ft_strlen(cmd));
	write(2, msg, ft_strlen(msg));
}

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

static char	*check_path(char **path, int *i, char *res, char *command)
{
	while (path && path[*i])
	{
		res = ft_strjoin(ft_strdup(path[(*i)++]), ft_strdup(command));
		if (access(res, X_OK) == 0)
			return (free(command), ft_free2(path), res);
		free(res);
	}
	return (NULL);
}

char	*get_path(char	*argv, char	**env)
{
	char		**path;
	char		*command;
	char		*res;
	int			i;
	struct stat	file;

	res = NULL;
	if (!argv)
		exit(0);
	if (argv && !argv[0])
		(print_no_cmd("", ": command not found\n"), exit(127));
	if (stat(argv, &file) == 0 && S_ISDIR(file.st_mode))
		(print_no_cmd(argv, ": is a directory\n"), exit(126));
	if (argv && argv[0] == '/')
		return (ft_strdup(argv));
	else if (argv && argv[0] == '.')
		return (ft_strdup(argv + 2));
	1 && (path = split_path(env), i = 0);
	command = ft_strjoin(ft_strdup("/"), ft_strdup(argv));
	res = check_path(path, &i, res, command);
	if (res)
		return (res);
	(print_no_cmd(argv, ": command not found\n"), exit(127));
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
