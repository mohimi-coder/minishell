/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_errors.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 11:47:23 by mohimi            #+#    #+#             */
/*   Updated: 2024/07/29 16:45:48 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	pipe_errors(t_token *token)
{
	t_token	*tmp;

	tmp = token;
	if (tmp && tmp->type == PIPE)
		return (1);
	while (tmp)
	{
		if (tmp->type == PIPE)
		{
			tmp = tmp->next;
			if (tmp && (tmp->type == IN || tmp->type == OUT \
				|| tmp->type == APPEND_OUT || tmp->type == HER_DOC))
				return (1);
			if (tmp && tmp->type == SPC)
				tmp = tmp->next;
			if (!tmp || tmp->type == PIPE)
				return (1);
		}
		tmp = tmp->next;
	}
	return (0);
}

int	redirec_errors(t_token *token)
{
	t_token	*tmp;

	tmp = token;
	while (tmp)
	{
		if (tmp->type == IN || tmp->type == OUT || tmp->type == APPEND_OUT \
			|| tmp->type == HER_DOC)
		{
			tmp = tmp->next;
			if (tmp && tmp->type == SPC)
				tmp = tmp->next;
			if (!tmp)
				return (1);
			if (tmp->type == IN || tmp->type == OUT || tmp->type == APPEND_OUT \
				|| tmp->type == HER_DOC || tmp->type == PIPE)
				return (1);
		}
		tmp = tmp->next;
	}
	return (0);
}
