/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 18:25:07 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/22 16:07:23 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_operation_handller(char *new_str)
{
	int	i;

	i = 0;
	while (new_str[i])
	{
		if (!ft_strncmp(new_str + i, "<<<", 3)
			|| !ft_strncmp(new_str + i, ">>>", 3))
			ft_error_message("syntax error near unexpected token");
		if (ft_error_operation(new_str[i]))
			ft_error_message("syntax error: special characters!");
		if (ft_operations(new_str[i]))
		{
			if (new_str[i] == '<' && new_str[i + 1] == '|')
				ft_error_message("syntax error near unexpected token");
			if (new_str[i] == '>' && new_str[i + 1] == '<')
				ft_error_message("syntax error near unexpected token");
			if (new_str[i] == '|' && new_str[i + 1] == '|')
				ft_error_message("syntax error near unexpected token");
		}
		i++;
	}
}

void	handle_quotes(char *av, t_token **token, int *i)
{
	int	start;

	start = *i;
	if (av[*i] == 39)
	{
		(*i)++;
		while (av[*i] != 39 && av[*i])
			(*i)++;
		if (!av[*i])
			(ft_lstclear(token), ft_error_message("unclosed quotes!"));
		add_back(token, ft_lstnew(SINGLE_QUOTE,
				ft_substr(av, start, ++(*i) - start)));
	}
	else if (av[*i] == 34)
	{
		(*i)++;
		while (av[*i] != 34 && av[*i])
			(*i)++;
		if (!av[*i])
			(ft_lstclear(token), ft_error_message("unclosed quotes!"));
		add_back(token, ft_lstnew(DOUBLE_QUOTES,
				ft_substr(av, start, ++(*i) - start)));
	}
}

void	ft_var(char *av, t_token **token, int *i, int *start)
{
	if (av[*i + 1] >= '0' && av[*i + 1] <= '9')
	{
		add_back(token, ft_lstnew(VAR, ft_substr(av, *start, 2)));
		(*i) += 2;
		return ;
	}
	while (av[*i + 1] && (ft_isalnum(av[*i + 1]) || av[*i + 1] == '_'))
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
	else if (av[*i + 1] && (ft_isalnum(av[*i + 1]) || av[*i + 1] == '_'))
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
