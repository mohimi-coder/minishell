/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 11:04:30 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/21 13:28:44 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_get_env(char *key, t_env *lst)
{
	while (lst)
	{
		if (!strcmp(lst->key, key))
			return (lst->val + 1);
		lst = lst->next;
	}
	return (NULL);
}

void	ft_update_pwd(t_env *old, t_env *pwd, char *buf)
{
	if (old)
	{
		(free(old->val), old->val = ft_strjoin(ft_strdup("="),
				ft_strdup(buf)));
	}
	if (pwd)
	{
		(free(pwd->val), getcwd(buf, 4096));
		pwd->val = ft_strjoin(ft_strdup("="), ft_strdup(buf));
	}
}

void	ft_update_pwd_home(t_env *old, t_env *node, char *buf, t_env **env)
{
	if (old)
		(free (old->val), getcwd(buf, 4096),
			old->val = ft_strjoin(ft_strdup("="), ft_strdup(buf)));
	chdir(ft_get_env("HOME", *env));
	if (node)
		(free(node->val), getcwd(buf, 4096),
			node->val = ft_strjoin(ft_strdup("="), ft_strdup(buf)));
}

void	ft_cd(char **cmd, t_env **env)
{
	t_env	*node;
	t_env	*old;
	char	*cwd;
	char	buf[4096];
	int		test;

	1 && (node = ft_check_var("PWD", *env), old = ft_check_var("OLDPWD", *env));
	if ((!ft_check_var("HOME", *env)) && (!cmd[1] || !ft_strcmp(cmd[1], "~")))
	{
		printf("%s cd: HOME not set ❗ %s\n", RED BOLD, RESET);
		return ;
	}
	if (!cmd[1] || !ft_strcmp(cmd[1], "~"))
		return (ft_status(0, true), ft_update_pwd_home(old, node, buf, env));
	(getcwd(buf, 4096), test = chdir(cmd[1]), cwd = getcwd(NULL, 0));
	if (!access(cmd[1], F_OK) && access(cmd[1], R_OK))
		(printf("Permission denied: %s❗\n", cmd[1]), ft_status(1, true));
	else if (!ft_strcmp(cmd[1], "..") && (test < 0 || !cwd))
		(printf("getcwd : No such file or direct❗\n"), ft_status(1, true),
			ft_update_pwd(old, node, buf));
	else if (test >= 0)
		(ft_update_pwd(old, node, buf), ft_status(0, true));
	else if (test < 0)
		(printf("No such file or direct: %s❗\n", cmd[1]), ft_status(1, true));
	free(cwd);
}
