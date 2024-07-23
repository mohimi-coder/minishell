/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/04 20:22:19 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/23 19:01:43 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	g_flag = 0;

void	child1_func(int	*end, t_list *list, char *env[], t_env **tenv)
{
	char	*cmd;
	int		id;

	id = fork();
	if (id < 0)
		(perror("fork"), exit(EXIT_FAILURE));
	if (id == 0)
	{
		if (list->next && dup2(end[1], 1) < 0)
			((close(end[1]), close(end[0])), ft_perror("dup Error"));
		(close(end[1]), close(end[0]));
		if (check_red_files(list) == -1)
			(ft_perror("error in opening files"), exit(ft_status(1, true)));
		if (check_red_files(list) == -2)
			(write(2, "ambiguous redirect\n", 20), exit(ft_status(1, true)));
		if (ft_builtins(list, tenv))
			exit(ft_status(1, true));
		cmd = get_path(list->cmd[0], env);
		execve(cmd, list->cmd, env);
	}
	if (!list->next)
		exit_status(id);
}

void	child2_func(t_pipe	p, t_list *list, char *env[], t_env **tenv)
{
	char	*cmd;
	int		id;

	id = fork();
	if (id < 0)
		(perror("fork"), exit(EXIT_FAILURE));
	if (id == 0)
	{
		if (dup2(p.in, 0) < 0)
			(close(p.in), ft_perror("dup3 Error"));
		(close(p.in));
		if (check_red_files(list) == -1)
			(ft_perror("error in opening files"), exit(ft_status(1, true)));
		if (check_red_files(list) == -2)
			(write(2, "ambiguous redirect\n", 20), exit(ft_status(1, true)));
		if (ft_builtins(list, tenv))
			exit(ft_status(1, true));
		cmd = get_path(list->cmd[0], env);
		execve(cmd, list->cmd, env);
		ft_perror(list->cmd[0]);
	}
	exit_status(id);
}

void	ft_child(t_list *tmp, char *env[], t_pipe	p, t_env **tenv)
{
	char	*cmd;
	pid_t	pid1;

	pid1 = fork();
	if (pid1 < 0)
		ft_perror("error in pid1");
	if (pid1 == 0)
	{
		close(p.end2[0]);
		if (dup2(p.in, 0) < 0 || dup2(p.end2[1], 1) < 0)
			ft_perror("dup Error");
		(close(p.in), close(p.end2[1]));
		if (check_red_files(tmp) == -1)
			(ft_perror("error in opening files"), exit(ft_status(1, true)));
		if (check_red_files(tmp) == -2)
			(write(2, "ambiguous redirect\n", 20), exit(ft_status(1, true)));
		if (ft_builtins(tmp, tenv))
			exit(ft_status(1, true));
		cmd = get_path(tmp->cmd[0], env);
		execve(cmd, tmp->cmd, env);
		ft_perror(tmp->cmd[0]);
	}
}

void	ft_pipe(t_list *list, char *env[], t_env **tenv)
{
	t_list	*tmp;
	t_pipe	p;

	tmp = list;
	if (pipe(p.end) < 0)
		ft_perror("error in pipe");
	child1_func(p.end, list, env, tenv);
	(close(p.end[1]), p.in = p.end[0], tmp = tmp->next);
	while (tmp && tmp->next)
	{
		if (pipe(p.end2) < 0)
			ft_perror("error in pipe");
		(ft_child(tmp, env, p, tenv), close(p.in), p.in = p.end2[0]);
		(close(p.end2[1]), tmp = tmp->next);
	}
	if (tmp)
		(child2_func(p, tmp, env, tenv));
	close(p.in);
}

void	execution(t_list *list, t_env **tenv)
{
	char	**env;
	int		stdin;
	int		stdout;

	if (!list)
		return ;
	stdin = dup(0);
	stdout = dup(1);
	env = get_arr_env(tenv);
	ft_pipe(list, env, tenv);
	while (wait(NULL) != -1)
		;
	dup2(stdin, 0);
	dup2(stdout, 1);
	close(stdin);
	close(stdout);
}
