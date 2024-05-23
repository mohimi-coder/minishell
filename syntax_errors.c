/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_errors.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/23 11:47:23 by mohimi            #+#    #+#             */
/*   Updated: 2024/05/23 12:48:35 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	pipe_errors(t_token *token)
{
	t_token	*tmp;

	tmp = token;
	while (tmp)
	{
		if (tmp->type == PIPE)
		{
			tmp = tmp->next;
			if (tmp && (tmp->type == IN || tmp->type == OUT \
				|| tmp->type == APPEND_OUT || tmp->type == HER_DOC))
				ft_error_message(RED "syntax error" RESET);
			if (tmp && tmp->type == SPACE)
				tmp = tmp->next;
			if (!tmp)
			{
				ft_error_message(RED "syntax error" RESET);
				break ;
			}
			if (tmp->type == PIPE)
				ft_error_message(RED "syntax error" RESET);
		}
		tmp = tmp->next;
	}
	return ;
}

void	redirec_errors(t_token *token)
{
	t_token	*tmp;

	tmp = token;
	while (tmp)
	{
		if (tmp->type == IN || tmp->type == OUT || tmp->type == APPEND_OUT \
			|| tmp->type == HER_DOC)
		{
			tmp = tmp->next;
			if (tmp && tmp->type == SPACE)
				tmp = tmp->next;
			if (!tmp)
			{
				ft_error_message(RED "syntax error" RESET);
				break ;
			}
			if (tmp->type == IN || tmp->type == OUT || tmp->type == APPEND_OUT \
				|| tmp->type == HER_DOC || tmp->type == PIPE)
				ft_error_message(RED "syntax error" RESET);
		}
		tmp = tmp->next;
	}
	return ;
}
