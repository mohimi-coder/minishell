/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 11:04:30 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/10 09:29:22 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_cd(char **cmd)
{
	char	*path;

	if (!cmd[1] || !ft_strcmp(cmd[1], "~"))
	{
		chdir(getenv("HOME"));
		return ;
	}
	path = cmd[1];
	if (!access(path, F_OK) && access(path, R_OK))
		printf("%s Permission denied: %s❗ %s\n", RED BOLD, path, RESET);
	else if (chdir(path) < 0)
		printf("%s No such file or directory: %s❗ %s\n", RED BOLD, path, RESET);
}
