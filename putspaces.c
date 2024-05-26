/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   putspaces.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:44:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/26 16:54:00 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_puspaces(char *av, t_env *env)
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
		ft_error_message(RED BOLD "➥  syntax error❗" RESET);
	(ft_expand(token, env), ft_lstclear(&token));
	return ;
}

void	ft_fill_env(char **env, t_env **envr)
{
	int		i;
	char	**split;
	t_env	*new;

	i = 0;
	while (env[i])
	{
		split = ft_split(env[i], '=');
		if (!split)
			return ;
		new = malloc(sizeof(t_env));
		if (!new)
		{
			ft_free_leak(split);
			return ;
		}
		new->key = ft_strdup(split[0]);
		new->val = ft_strdup(split[1]);
		new->next = NULL;
		add_back_env(envr, new);
		ft_free_leak(split);
		i++;
	}
	return ;
}

void	minishell_loop(char *input, char **env)
{
	t_env	*envr;

	envr = NULL;
	ft_fill_env(env, &envr);
	while (1)
	{
		input = readline(PURPLE"╰┈➤ Shell-Z.M ✗ " RESET);
		if (!input)
			(printf("exit\n"), exit(0));
		else if (ft_strncmp(input, "exit", 4) == 0)
			(printf("exit\n"), exit(0));
		ft_puspaces(input, envr);
		add_history(input);
		free(input);
	}
}

void f(void)
{
	system("leaks minishell");
}

int	main(int ac, char **av, char **env)
{
	char	*str;
	atexit(f);
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
