/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_helper.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/12 18:01:57 by mohimi            #+#    #+#             */
/*   Updated: 2024/07/24 22:22:12 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_update(char *str, t_env *env, t_env *new, char **s)
{
	if (ft_strchr(str, '='))
	{
		new = ft_check_var(s[0], env);
		free(new->val);
		new->val = ft_strdup(ft_strchr(str, '='));
	}
	ft_free_leak(s);
}

int	check_cmd(char *cmd)
{
	int	st;

	st = ft_status(1, true);
	if ((cmd[0] >= '0' && cmd[0] <= '9') || (cmd[0] == '+' || cmd[0] == '=')
		|| cmd[0] == '\0')
		return ((print_no_cmd(cmd, ": not an identifier\n"), st));
	return (0);
}
