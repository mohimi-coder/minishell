/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_helper.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/12 19:06:30 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/12 19:07:18 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	add_newline(char *str, char *delimiter, char **buff)
{
	char	*tmp;

	if (ft_strcmp(str, delimiter))
	{
		tmp = join_her(*buff, "\n");
		free(*buff);
		*buff = tmp;
	}
}
