/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   herdoc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 19:04:48 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/02 19:35:19 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char *read_and_expand(char *delimiter, int flag, t_env *env, char **buff)
{
	char *str;
	char *expanded;
	char *tmp;

	str = readline(YELLOW BOLD "heredoc 📝➥ " RESET);
	while (str && ft_strcmp(str, delimiter))
	{
		if (flag == 0)
		{
			expanded = ft_expand_dollar_her(str, env);
			tmp = ft_strjoin_her(*buff, expanded);
		}
		else
			tmp = ft_strjoin_her(*buff, str);
		free(*buff);
		*buff = tmp;
		free(str);
		str = readline(YELLOW BOLD "heredoc 📝➥ " RESET);
		if (ft_strcmp(str, delimiter))
		{
			tmp = ft_strjoin_her(*buff, ft_strdup("\n"));
			free(*buff), *buff = tmp;
		}
	}
	return (free(str), *buff);
}

char *create_temp_file(char *buff, int *i)
{
	char *file_name;
	int fd;

	file_name = ft_strjoin_her("/tmp/file_", ft_itoa(*i));
	fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC, 0777);
	if (fd < 0)
	{
		free(buff);
		free(file_name);
		return NULL;
	}
	ft_putendl_fd(buff, fd);
	close(fd);
	(*i)++;
	free(buff);
	return (file_name);
}

char *func(char *delimiter, int flag, t_env *env)
{
	static int i;
	char *buff;
	char *file_name;

	(1) && (i = 0, buff = NULL, file_name = NULL);
	buff = read_and_expand(delimiter, flag, env, &buff);
	if (buff)
		file_name = create_temp_file(buff, &i);
	return (file_name);
}

void process_herdoc(t_token **new, t_token **tmp, t_env *env)
{
	char *join;
	int flag;
	char *filename;

	(1) && (join = NULL, flag = 0);
	if (*tmp && (*tmp)->type == SPC)
	{
		add_back(new, ft_lstnew((*tmp)->type, (*tmp)->content));
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
	add_back(new, ft_lstnew(WORD, filename));
	free(filename);
}

void ft_herdoc(t_token *token, t_token **new, t_env *env)
{
	t_token *tmp;

	tmp = token;
	if (pipe_errors(token) || redirec_errors(token))
	{
		ft_error_message(RED BOLD "➥  syntax error❗" RESET);
		return;
	}
	while (tmp)
	{
		if (tmp->type != HER_DOC)
		{
			add_back(new, ft_lstnew(tmp->type, tmp->content));
			tmp = tmp->next;
		}
		else
		{
			add_back(new, ft_lstnew(tmp->type, tmp->content));
			tmp = tmp->next;
			process_herdoc(new, &tmp, env);
		}
	}
}
