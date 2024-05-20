/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   putspaces.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:44:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/20 18:52:33 by zait-bel         ###   ########.fr       */
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
	printf("%s\n", avv);
	sp = ft_split(avv, '|');
	while (sp[i])
	{
		printf("%s\n", sp[i]);
		i++;
	}
	return (avv);
}

int	main(int ac, char **av)
{
	// char *str = "       '  ls -la'   cat|>out      'echo('hello')      $     USER   ";
	// ft_operation_spacese(av[1]);
	// char *l = ft_puspaces(str);
	if (ac > 1)
		ft_puspaces(av[1]);
	// printf("-> %s", l);
}