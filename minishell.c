/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:44:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/07 12:13:53 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_puspaces(char *av, t_env **env)
{
	t_token	*token;
	t_token	*new;
	t_token	*rep_herd;
	t_list	*final_list;
	int		i;

	(1) && (token = NULL, i = 0, final_list = NULL), skip_space(av, &i);
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
	rep_herd = ft_herdoc(token, *env);
	(ft_expand(&rep_herd, *env)), (new = new_list(rep_herd));
	final_list = ft_finall(new);
	if (final_list && !final_list->next && ft_builtins(final_list, env))
		final_list = final_list->next;
	execution(final_list, env);
	return (ft_lstclear(&token), ft_lstclear(&new), ft_lstclear(&rep_herd), ft_lstclear_final(&final_list));
}

void	minishell_loop(char *input, char **env)
{
	t_env	*envr;

	envr = NULL;
	ft_fill_env(env, &envr);
	while (1)
	{
		signals();
		input = readline(PURPLE BG_MAGENTA"╰┈➤ Shell-Z.M ✗ "BG_MAGENTA RESET);
		if (!input)
			(printf("exit\n"), exit(0));
		ft_puspaces(input, &envr);
		if (ft_strcmp(input, ""))
			add_history(input);
		free(input);
	}
}

int	main(int ac, char **av, char **env)
{
	char	*str;

	str = NULL;
	if (ac != 1 || av[1])
	{
		printf("Error\n");
		exit(0);
	}
	printf(ORANGE BOLD"➤  Welcome to our minishell 🤗\n" RESET);
	minishell_loop(str, env);
	return (0);
}