/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 11:04:30 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/24 20:45:18 by zait-bel         ###   ########.fr       */
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

void	ft_update_pwd(t_env *old, t_env *pwd, char *buf, t_env **env)
{
	if (old)
	{
		(free(old->val), old->val = ft_strjoin(ft_strdup("="),
				ft_strdup(buf)));
	}
	else if (!old && pwd)
		add_back_env(env, lstnew_env(ft_strdup("OLDPWD"),
				ft_strjoin(ft_strdup("="), ft_strdup(buf))));
	getcwd(buf, 4096);
	if (pwd)
		(free(pwd->val), pwd->val = ft_strjoin(ft_strdup("="), ft_strdup(buf)));
	else
		add_back_env(env, lstnew_env(ft_strdup("PWD"),
				ft_strjoin(ft_strdup("="), ft_strdup(buf))));
}

void	ft_update_pwd_home(t_env *old, t_env *node, char *buf, t_env **env)
{
	getcwd(buf, 4096);
	if (old)
		(free (old->val),
			old->val = ft_strjoin(ft_strdup("="), ft_strdup(buf)));
	else if (!old && node)
		add_back_env(env, lstnew_env(ft_strdup("OLDPWD"),
				ft_strjoin(ft_strdup("="), ft_strdup(buf))));
	chdir(ft_get_env("HOME", *env));
	getcwd(buf, 4096);
	if (node)
		(free(node->val),
			node->val = ft_strjoin(ft_strdup("="), ft_strdup(buf)));
	else
		add_back_env(env, lstnew_env(ft_strdup("PWD"),
				ft_strjoin(ft_strdup("="), ft_strdup(buf))));
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
		return (ft_status(127, true), print_no_cmd("", "cd: HOME not set\n"));
	if (!cmd[1] || !ft_strcmp(cmd[1], "~"))
		return (ft_status(0, true), ft_update_pwd_home(old, node, buf, env));
	(getcwd(buf, 4096), test = chdir(cmd[1]), cwd = getcwd(NULL, 0));
	if (!access(cmd[1], F_OK) && access(cmd[1], R_OK))
		(print_no_cmd(cmd[1], "Permission denied\n"), ft_status(1, true));
	else if (!ft_strcmp(cmd[1], "..") && (test < 0 || !cwd))
		((print_no_cmd("", "getcwd : No such file or direct\n"),
				ft_status(1, true)), ft_update_pwd(old, node, buf, env));
	else if (test >= 0)
		(ft_update_pwd(old, node, buf, env), ft_status(0, true));
	else if (test < 0)
		(print_no_cmd(cmd[1], "getcwd : No such file or direct\n"),
			ft_status(1, true));
	free(cwd);
}
