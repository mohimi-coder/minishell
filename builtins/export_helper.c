/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_helper.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/12 18:01:57 by mohimi            #+#    #+#             */
/*   Updated: 2024/07/04 10:49:58 by zait-bel         ###   ########.fr       */
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
	if ((cmd[0] >= '0' && cmd[0] <= '9') || (cmd[0] == '+' || cmd[0] == '=')
		|| cmd[0] == '\0')
		return (printf("%s ➥ %s: not an identifier❗%s\n", \
			RED BOLD, cmd, RESET), 1);
	return (0);
}
