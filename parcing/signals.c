/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 15:50:01 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/29 16:45:44 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	sig_int_here(int sig_num)
{
	(void)sig_num;
	ft_status(1, true);
	g_flag = -1;
	close(0);
	printf("\n");
}

void	ft_handl_quit(int sig_num)
{
	int		status;
	pid_t	pid;

	if (sig_num == SIGQUIT)
	{
		pid = wait(&status);
		if (pid > 0)
		{
			ft_status(128 + WTERMSIG(status), true);
			write(1, "Quit: 3\n", 9);
		}
	}
}

void	sig_int(int sig_num)
{
	(void)sig_num;
	if (g_flag == 0)
	{
		ft_status(1, true);
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	else
		return ;
}

void	ft_sign(void)
{
	signal(SIGINT, sig_int);
	signal(SIGQUIT, ft_handl_quit);
	rl_catch_signals = 0;
}

void	sig_here_doc(void)
{
	signal(SIGINT, sig_int_here);
	signal(SIGQUIT, SIG_IGN);
}
