/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   putspaces.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:44:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/06 18:55:14 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_puspaces(char *av, t_env **env)
{
	t_token	*token;
	t_token	*new;
	int		i;

	(1) && (token = NULL);
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
	ft_herdoc(token, *env);
	ft_expand(token, *env);
	new = new_list(token);
	ft_lstclear(&token);
	(ft_builtins(new, env));
	ft_lstclear(&new);
	return ;
}

void	ft_fill_env(char **env, t_env **envr)
{
	int		i;
	char	**sp;
	t_env	*new;

	i = 0;
	while (env[i])
	{
		sp = ft_split(env[i], '=');
		if (!sp)
			return ;
		new = lstnew_env(ft_strdup(sp[0]), ft_strdup(ft_strchr(env[i], '=')));
		add_back_env(envr, new);
		ft_free_leak(sp);
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
		signals();
		input = readline(PURPLE"╰┈➤ Shell-Z.M ✗ "RESET);
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
