/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 11:04:30 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/12 16:14:01 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_cd(char **cmd, t_env **env)
{
	t_env	*node;
	t_env	*old;
	char	buf[4096];

	node = ft_check_var("PWD", *env);
	old = ft_check_var("OLDPWD", *env);
	if (!cmd[1] || !ft_strcmp(cmd[1], "~"))
	{
		(free (old->val), getcwd(buf, 4096));
		old->val = ft_strjoin(ft_strdup("="), ft_strdup(buf));
		(chdir(getenv("HOME")), free(node->val), getcwd(buf, 4096));
		node->val = ft_strjoin(ft_strdup("="), ft_strdup(buf));
		return ;
	}
	getcwd(buf, 4096);
	if (!access(cmd[1], F_OK) && access(cmd[1], R_OK))
		printf("%s Permission denied: %s❗ %s\n", RED BOLD, cmd[1], RESET);
	else if (chdir(cmd[1]) >= 0)
	{
		(free(old->val), old->val = ft_strjoin(ft_strdup("="), ft_strdup(buf)));
		(free(node->val), getcwd(buf, 4096));
		node->val = ft_strjoin(ft_strdup("="), ft_strdup(buf));
	}
	else if (chdir(cmd[1]) < 0)
		printf("%sNo such file or directory: %s❗%s\n", RED BOLD, cmd[1], RESET);
}
