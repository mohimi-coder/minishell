/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   herdoc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 19:04:48 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/07 18:11:13 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*read_and_expand(char *delimiter, int flag, t_env *env)
{
	char	*str;
	char	*expanded;
	char	*buff;
	char	*tmp;

	(1) && (buff = NULL, str = readline(YELLOW BOLD "heredoc 📝➥ " RESET));
	while (str && ft_strcmp(str, delimiter))
	{
		if (flag == 0)
		{
			expanded = ft_expand_dollar_her(str, env);
			tmp = ft_strjoin_her(buff, expanded);
			(free(expanded), free(buff), buff = tmp);
		}
		else
		{
			tmp = ft_strjoin_her(buff, str);
			(free(buff), buff = tmp);
		}
		(1) && (free(str), str = readline(YELLOW BOLD "heredoc 📝➥ " RESET));
		if (ft_strcmp(str, delimiter))
			(tmp = ft_strjoin_her(buff, "\n"), (free(buff), buff = tmp));
	}
	return (free(delimiter), free(str), buff);
}

char	*create_temp_file(char *buff, int *i)
{
	char	*file_name;
	int		fd;
	char 	*nbr;

	nbr = ft_itoa(*i);
	file_name = ft_strjoin_her("/tmp/file_", nbr);
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
	while (*tmp && (*tmp)->type != SPC && (*tmp)->type != OUT &&
		   (*tmp)->type != APPEND_OUT && (*tmp)->type != IN &&
		   (*tmp)->type != PIPE && (*tmp)->type != HER_DOC)
	{
		join = ft_strjoin_her(join, (*tmp)->content);
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
	t_token *new = NULL;

	tmp = token;
	if (pipe_errors(token) || redirec_errors(token))
	{
		ft_error_message(RED BOLD "➥  syntax error❗" RESET);
		return(NULL);
	}
	while (tmp)
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
	return(new);
}
