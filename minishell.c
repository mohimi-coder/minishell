/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:44:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/23 17:57:24 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	parse_input(char *av, t_token **token, int *i)
{
	while (av[*i])
	{
		if ((av[*i] == ' ' || (av[*i] >= 9 && av[*i] <= 13)))
			handle_spaces(av, token, i);
		else if (av[*i] == 39 || av[*i] == 34)
			handle_quotes(av, token, i);
		else if (av[*i] == '$')
			handle_dollar(av, i, token);
		else if (av[*i] == '>')
			out_redir(av, token, i);
		else if (av[*i] == '<')
			in_redir(av, token, i);
		else
			pipe_word(av, token, i);
	}
}

void	ft_puspaces(char *av, t_env **env)
{
	t_token	*token;
	t_token	*new;
	t_token	*rep_herd;
	t_list	*final_list;
	int		i;

	(1) && (token = NULL, i = 0, final_list = NULL);
	skip_space(av, &i);
	parse_input(av, &token, &i);
	rep_herd = ft_herdoc(token, *env);
	ft_expand(&rep_herd, *env);
	new = new_list(rep_herd);
	final_list = ft_finall(new);
	check_red_files(final_list);
	if (final_list && !final_list->next && ft_builtins(final_list, env))
		final_list = final_list->next;
	g_flag = 1;
	execution(final_list, env);
	g_flag = 0;
	(ft_lstclear(&token), ft_lstclear(&new));
	(ft_lstclear(&rep_herd), ft_lstclear_final(&final_list));
	// (dup2(fd1, 0), dup2(fd2, 1), close(fd1), close(fd2));
	return ;
}

void	minishell_loop(char *input, char **env, struct termios *atr)
{
	t_env	*envr;
	// int		fd1;
	// int		fd2;

	1 && (envr = NULL);
	(ft_fill_env(env, &envr), ft_sign());
	while (1)
	{
		input = readline(PURPLE"╰┈➤ Shell-Z.M ✗ "RESET);
		if (!input)
			(printf("exit\n"), exit(ft_status(0, false)));
		ft_puspaces(input, &envr);
		tcsetattr(0, 0, atr);
		if (ft_strcmp(input, ""))
			add_history(input);
		free(input);
	}
}

int	main(int ac, char **av, char **env)
{
	char			*str;
	struct termios	atr;

	str = NULL;
	if (ac != 1 || av[1])
	{
		printf("Error\n");
		exit(0);
	}
	printf(ORANGE BOLD"➤  Welcome to our minishell 🤗\n" RESET);
	tcgetattr(0, &atr);
	minishell_loop(str, env, &atr);
	return (0);
}
