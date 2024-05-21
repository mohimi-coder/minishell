/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   putspaces.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:44:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/21 09:57:06 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_puspaces(char *av)
{
	char	*avv;
	char	**sp;
	int		i;

	i = 0;
	ft_operation_handller(av);
	avv = ft_operation_spaces(av);
	handle_quotes(avv);
	sp = ft_split(avv, '|');
	while (sp[i])
	{
		printf("%s\n", sp[i]);
		i++;
	}
	return (avv);
}

void	minishell_loop(char *input)
{
	while (1)
	{
		input = readline(PINK "➜  Shell-2.4$ " RESET);
		if (!input)
			printf("exit\n"), exit(0);
		else if (ft_strncmp(input, "exit", 4) == 0)
			printf("exit\n"), exit(0);
		// else if (input[0] == '\0' )
		// 	continue;
		ft_puspaces(input);
		add_history(input);
	}
}

int	main(int ac, char **av)
{
	char	*str = NULL;

	if (ac != 1 || av[1])
	{
		printf("Error\n");
		exit(0);
	}
	printf(ORANGE "Welcome to our minishell!!\n" RESET);
	minishell_loop(str);
	return (0);
}