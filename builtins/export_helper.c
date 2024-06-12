/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export_helper.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/12 18:01:57 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/12 18:08:11 by mohimi           ###   ########.fr       */
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

int	check_empty(char **cmd)
{
	int	i;
	int	j;

	i = 1;
	j = 1;
	while (cmd[j])
	{
		if (cmd[j][0] == '\0')
			i++;
		j++;
	}
	if (i == j)
		return (0);
	return (1);
}

int	check_cmd(char *cmd)
{
	if (!cmd[0])
		return (1);
	if ((cmd[0] >= '0' && cmd[0] <= '9') || (cmd[0] == '+' || cmd[0] == '='))
		return (printf("%s ➥ %s: not an identifier❗%s\n", \
			RED BOLD, cmd, RESET), 1);
	return (0);
}
