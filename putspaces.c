/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   putspaces.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:44:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/21 17:44:57 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// void	handle_dollar(char *av, int *i, t_token **token)
// {
// 	int	counter;
// 	int	len;
// 	int	start;

// 	(1 == 1) && (len = 0, start = 0, counter = 0);
// 	while (av[*i] == '$' && av[*i])
// 	{
// 		(*i)++;
// 		counter++;
// 	}
// 	if (counter % 2 == 0)
// }

char	*ft_puspaces(char *av)
{
	t_token	*token;
	int		i;
	char	*avv;

	token = NULL;
	i = 0;
	skip_space(av, &i);
	while (av[i])
	{
		if ((av[i] == ' ' || (av[i] >= 9 && av[i] <= 13)) && av[i])
			handle_spaces(av, &token, &i);
		if ((av[i] == 39 || av[i] == 34) && av[i])
			handle_quotes(av, &token, &i);
		if (av[i] == '$' && av[i])
			handle_dollar(av, &token, &i);
		if (av[i] == '>' && av[i + 1] == '>')
			add_back(&token, ft_lstnew(ARED_OUT, ft_strdup(">>")));
		if (av[i] == '<' && av[i + 1] == '<')
			add_back(&token, ft_lstnew(HER_DOC, ft_strdup("<<")));
		if (av[i] == '>' && av[i + 1] != '>')
			add_back(&token, ft_lstnew(RED_OUT, ft_strdup(">")));
		if (av[i] == '<' && av[i + 1] != '<')
			add_back(&token, ft_lstnew(RED_IN, ft_strdup("<")));
		if (av[i] == '|' && av[i])
			add_back(&token, ft_lstnew(PIPE, ft_strdup("|")));
		if (av[i] != ' ' && av[i] != 39 && av[i] != 34 && av[i] != '$' \
			&& av[i] != '>' && av[i] != '<' && av[i] != '|')
			add_back(&token, ft_lstnew(WORDS, \
				ft_strdup(ft_substr(av, i, 1))));
		i++;
	}
	ft_operation_handller(av);
	avv = ft_operation_spaces(av);
	return (avv);
}

void	minishell_loop(char *input)
{
	while (1)
	{
		input = readline(PINK "➜  Shell-2.4$ " RESET);
		if (!input)
			(printf("exit\n"), exit(0));
		else if (ft_strncmp(input, "exit", 4) == 0)
			printf("exit\n"), exit(0);
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
