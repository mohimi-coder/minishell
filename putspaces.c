/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   putspaces.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:44:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/23 14:37:18 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_puspaces(char *av)
{
	t_token	*token;
	int		i;

	token = NULL;
	i = 0;
	skip_space(av, &i);
	while (av[i])
	{
		if ((av[i] == ' ' || (av[i] >= 9 && av[i] <= 13)))
			handle_spaces(av, &token, &i);
		else if ((av[i] == 39 || av[i] == 34))
			handle_quotes(av, &token, &i);
		else if (av[i] == '$')
			handle_dollar(av, &i, &token);
		else if (av[i] == '>')
			out_redir(av, &token, &i);
		else if (av[i] == '<')
			in_redir(av, &token, &i);
		else
			pipe_word(av, &token, &i);
	}
	if (pipe_errors(token) || redirec_errors(token))
		ft_error_message(RED "syntax error" RESET);
	return ;
}

void	minishell_loop(char *input)
{
	while (1)
	{
		input = readline(PINK "➜  Shell-2.4 ✗ " RESET);
		if (!input)
			(printf("exit\n"), exit(0));
		else if (ft_strncmp(input, "exit", 4) == 0)
			(printf("exit\n"), exit(0));
		ft_puspaces(input);
		add_history(input);
	}
}

int	main(int ac, char **av)
{
	char	*str;

	str = NULL;
	if (ac != 1 || av[1])
	{
		printf("Error\n");
		exit(0);
	}
	printf(ORANGE "Welcome to our minishell!!\n" RESET);
	minishell_loop(str);
	return (0);
}
