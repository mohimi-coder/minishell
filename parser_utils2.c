/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 12:50:53 by mohimi            #+#    #+#             */
/*   Updated: 2024/05/26 15:57:45 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_spaces(char *av, t_token **token, int *i)
{
	int	c;

	c = 0;
	while ((av[*i] == ' ' || (av[*i] >= 9 && av[*i] <= 13)) && av[*i])
	{
		c = 1;
		(*i)++;
	}
	if (c == 1 && av[*i])
		add_back(token, ft_lstnew(SPACE, ft_strdup(" ")));
}

void	skip_space(char *str, int *i)
{
	while ((str[*i] == ' ' || (str[*i] >= 9 && str[*i] <= 13)) && str[*i])
		(*i)++;
}

void	out_redir(char *av, t_token **token, int *i)
{
	if (av[*i + 1] && av[*i + 1] == '>')
	{
		add_back(token, ft_lstnew(APPEND_OUT, ft_strdup(">>")));
		(*i)++;
	}
	else
		add_back(token, ft_lstnew(OUT, ft_strdup(">")));
	(*i)++;
}

void	in_redir(char *av, t_token **token, int *i)
{
	if (av[*i + 1] && av[*i + 1] == '<')
	{
		add_back(token, ft_lstnew(HER_DOC, ft_strdup("<<")));
		(*i)++;
	}
	else
		add_back(token, ft_lstnew(IN, ft_strdup("<")));
	(*i)++;
}

void	pipe_word(char *av, t_token **token, int *i)
{
	int	start;

	if (av[*i] == '|')
	{
		add_back(token, ft_lstnew(PIPE, ft_strdup("|")));
		(*i)++;
	}
	else
	{
		start = *i;
		while (av[*i] && av[*i] != '|' && av[*i] != '<' && av[*i] != '>'
			&& av[*i] != '$' && av[*i] != 39 && av[*i] != 34
			&& av[*i] != 32 && !(av[*i] >= 9 && av[*i] <= 13))
			(*i)++;
		add_back(token, ft_lstnew(WORD, \
			ft_strdup(ft_substr(av, start, *i - start))));
	}
}
