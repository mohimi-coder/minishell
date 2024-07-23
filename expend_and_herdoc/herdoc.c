/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   herdoc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 19:04:48 by mohimi            #+#    #+#             */
/*   Updated: 2024/07/23 07:57:49 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*read_and_expand(char *delimiter, int flag, t_env *env)
{
	char	*str;
	char	*expn;
	char	*buff;
	char	*tmp;
	int		sec_in;

	(sig_here_doc(), rl_catch_signals = 1);
	(1) && (sec_in = dup(0), buff = NULL, str = readline("heredoc 📝➥ "));
	while (str && ft_strcmp(str, delimiter))
	{
		if (flag == 0)
		{
			(1) && (expn = epnd__her(str, env), tmp = join_her(buff, expn));
			(free(expn), free(buff), buff = tmp);
		}
		else
		{
			tmp = join_her(buff, str);
			(free(buff), buff = tmp);
		}
		(1) && (free(str), str = readline("heredoc 📝➥"));
		add_newline(str, delimiter, &buff);
	}
	(ft_sign(), dup2(sec_in, 0), close(sec_in));
	return (free(delimiter), free(str), buff);
}

char	*create_temp_file(char *buff, int *i)
{
	char	*file_name;
	int		fd;
	char	*nbr;

	nbr = ft_itoa(*i);
	file_name = join_her("/tmp/file_", nbr);
	free(nbr);
	fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC, 0777);
	if (fd < 0)
		return (free(buff), free(file_name), NULL);
	ft_putendl_fd(buff, fd);
	close(fd);
	(*i)++;
	return (free(buff), file_name);
}

char	*func(char *delimiter, int flag, t_env *env)
{
	static int	i;
	char		*buff;
	char		*file_name;

	(1) && (buff = NULL, file_name = NULL);
	buff = read_and_expand(delimiter, flag, env);
	file_name = create_temp_file(buff, &i);
	return (file_name);
}

void	process_herdoc(t_token **new, t_token **tmp, t_env *env)
{
	char	*join;
	int		flag;
	char	*filename;

	(1) && (join = NULL, flag = 0);
	if (*tmp && (*tmp)->type == SPC)
	{
		add_back(new, ft_lstnew((*tmp)->type, ft_strdup((*tmp)->content)));
		(*tmp) = (*tmp)->next;
	}
	while (*tmp && (*tmp)->type != SPC && (*tmp)->type != OUT && \
		(*tmp)->type != APPEND_OUT && (*tmp)->type != IN && \
		(*tmp)->type != PIPE && (*tmp)->type != HER_DOC)
	{
		join = join_her(join, (*tmp)->content);
		if ((*tmp)->type == DOUBLE_QUOTES || (*tmp)->type == SINGLE_QUOTE)
			flag = 1;
		*tmp = (*tmp)->next;
	}
	filename = func(join, flag, env);
	add_back(new, ft_lstnew(WORD, ft_strdup(filename)));
	free(filename);
}

t_token	*ft_herdoc(t_token *token, t_env *env)
{
	t_token	*tmp;
	t_token	*new;

	(1) && (new = NULL, tmp = token);
	if (pipe_errors(token) || redirec_errors(token))
	{
		(ft_error_message(RED BOLD "➥  syntax error❗" RESET), ft_status(258, true));
		return (NULL);
	}
	while (g_flag != -1 && tmp)
	{
		if (tmp->type != HER_DOC)
		{
			add_back(&new, ft_lstnew(tmp->type, ft_strdup(tmp->content)));
			tmp = tmp->next;
		}
		else
		{
			add_back(&new, ft_lstnew(tmp->type, ft_strdup(tmp->content)));
			tmp = tmp->next;
			process_herdoc(&new, &tmp, env);
		}
	}
	g_flag = 0;
	return (new);
}
