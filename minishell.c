/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:44:50 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/11 21:46:10 by mohimi           ###   ########.fr       */
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

	(1) && (token = NULL, i = 0, final_list = NULL);
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
	rep_herd = ft_herdoc(token, *env);
	ft_expand(rep_herd, *env);
	new = new_list(rep_herd);
	final_list = ft_finall(new);
	// t_list *tmp = final_list;
	// i = 0;
	// while(tmp)
	// {
	// 	i = 0;
	// 	while (tmp->cmond && tmp->cmond[i])
	// 		printf(GREEN"cmd= [%s]\n"RESET, tmp->cmond[i++]);
	// 	printf(RED"------------------------------------------\n"RESET);
	// 	while(tmp->red)
	// 	{
	// 		printf(YELLOW"red= [%s]\n"RESET, tmp->red->content);
	// 		tmp->red = tmp->red->next;
	// 	}
	// 	tmp = tmp->next;
	// }
	ft_lstclear(&token);
	(ft_builtins(final_list, env));
	ft_lstclear(&new);
	ft_lstclear(&rep_herd);
	return ;
}

void	ft_fill_env(char **env, t_env **envr)
{
	int		i;
	char	**sp;
	t_env	*new;

	i = 0;
		if (!env[i])
	{
		add_back_env(envr, lstnew_env(ft_strdup("PATH"), ft_strdup\
("=/Users/mohimi/.docker/bin:/usr/gnu/bin:/usr/local/bin:/bin:/usr/bin:.")));
		add_back_env(envr, lstnew_env(ft_strdup("PWD"), ft_strdup(getcwd(NULL, 0))));
		add_back_env(envr, lstnew_env(ft_strdup("SHLVL"), ft_strdup("=2")));
		add_back_env(envr, lstnew_env(ft_strdup("OLDPWD"),  NULL));
		add_back_env(envr, lstnew_env(ft_strdup("_"), ft_strdup("=/usr/bin/env")));
		return ;
	}
	while (env[i])
	{
		sp = ft_split(env[i], '=');
		if (!sp)
			return ;
		new = lstnew_env(ft_strdup(sp[0]), ft_strdup(ft_strchr(env[i], '=')));
		(add_back_env(envr, new), ft_free_leak(sp));
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
