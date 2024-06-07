/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 11:04:30 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/06 19:05:51 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_cd(t_token *token)
{
	char	*path;

	if (!token->next || !ft_strcmp(token->next->content, "~"))
	{
		chdir(getenv("HOME"));
		return ;
	}
	path = token->next->content;
	if (!access(path, F_OK) && access(path, R_OK))
		printf("%s Permission denied: %s❗ %s\n", RED BOLD, path, RESET);
	else if (chdir(path) < 0)
		printf("%s No such file or directory: %s❗ %s\n", RED BOLD, path, RESET);
}
