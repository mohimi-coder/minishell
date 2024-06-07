/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 18:25:07 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/06 18:12:08 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	double_quotes(char *av, t_token **token, int *i)
{
	int		start;
	char	*str;

	start = *i;
	(*i)++;
	while (av[*i] != 34 && av[*i])
		(*i)++;
	if (!av[*i])
	{
		(ft_lstclear(token), \
			ft_error_message(RED BOLD"➥  unclosed quotes 📛" RESET));
		return ;
	}
	str = ft_substr(av, start + 1, ++(*i) - start - 2);
	add_back(token, ft_lstnew(DOUBLE_QUOTES, str));
}

void	handle_quotes(char *av, t_token **token, int *i)
{
	int		start;
	char	*str;

	start = *i;
	if (av[*i] == 39)
	{
		(*i)++;
		while (av[*i] != 39 && av[*i])
			(*i)++;
		if (!av[*i])
		{
			(ft_lstclear(token), \
				ft_error_message(RED BOLD"➥  unclosed quotes📛" RESET));
			return ;
		}
		str = ft_substr(av, start + 1, ++(*i) - start - 2);
		add_back(token, ft_lstnew(SINGLE_QUOTE, str));
	}
	else
		double_quotes(av, token, i);
}

void	ft_var(char *av, t_token **token, int *i, int *start)
{
	if (av[*i + 1] >= '0' && av[*i + 1] <= '9')
	{
		add_back(token, ft_lstnew(VAR, ft_substr(av, *start, 2)));
		(*i) += 2;
		return ;
	}
	while (av[*i + 1] && (f_alnum(av[*i + 1]) || av[*i + 1] == '_'))
		(*i)++;
	add_back(token, ft_lstnew(VAR, ft_substr(av, *start, ++(*i) - *start)));
}

void	handle_dollar(char *av, int *i, t_token **token)
{
	int	start;

	start = *i;
	if (av[*i + 1] && (av[*i + 1] == '$' || av[*i + 1] == '?'))
	{
		if (av[*i + 1] == '$')
			add_back(token, ft_lstnew(DOUBLE_DOLLAR, ft_strdup("$$")));
		else
			add_back(token, ft_lstnew(DOLLAR_WHY, ft_substr(av, start, 2)));
		(*i) += 2;
	}
	else if (av[*i + 1] && (f_alnum(av[*i + 1]) || av[*i + 1] == '_'))
		ft_var(av, token, i, &start);
	else if (av[*i + 1] && (av[*i + 1] == 39 || av[*i + 1] == 34))
		(*i)++;
	else
	{
		while (av[*i] && av[*i] != 34 && av[*i] != 39 && \
			av[*i] != ' ' && (av[*i] < 9 || av[*i] > 13) && \
			av[*i] != '|' && av[*i] != '<' && av[*i] != '>')
			(*i)++;
		add_back(token, ft_lstnew(WORD, ft_substr(av, start, (*i) - start)));
	}
}
