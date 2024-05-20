/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 18:25:07 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/20 18:25:51 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_operation_handller(char *new_str)
{
	int	i;

	i = 0;
	while (new_str[i])
	{
		if (!ft_strncmp(new_str + i, "<<<", 3)
			|| !ft_strncmp(new_str + i, ">>>", 3))
			ft_error_message("syntax error near unexpected token");
		if (ft_error_operation(new_str[i]))
			ft_error_message("syntax error: special characters!");
		if (ft_operations(new_str[i]))
		{
			if (new_str[i] == '<' && new_str[i + 1] == '|')
				ft_error_message("syntax error near unexpected token");
			if (new_str[i] == '>' && new_str[i + 1] == '<')
				ft_error_message("syntax error near unexpected token");
			if (new_str[i] == '|' && new_str[i + 1] == '|')
				ft_error_message("syntax error near unexpected token");
		}
		i++;
	}
}

int	ft_count_lenght(char *str)
{
	int	len;
	int	i;

	len = 0;
	i = 0;
	while (str[i])
	{
		if (ft_operations(str[i]))
		{
			if (str[i - 1] != ' ' && str[i - 1] != '>' && str[i - 1] != '<')
				len++;
			if (str[i + 1] != ' ' && str[i + 1] != '>' && str[i + 1] != '<')
				len++;
		}
		len++;
		i++;
	}
	return (len);
}

char	*allocate(char *av)
{
	int		len;
	char	*new_str;
	int		i;
	int		j;

	i = 0;
	j = 0;
	len = ft_strlen(av);
	new_str = malloc(len + 1);
	if (!new_str)
		return (NULL);
	while (j <= len)
		new_str[j++] = '\0';
	return (new_str);
}

char	*ft_operation_spaces(char *str)
{
	t_var	v;
	int		i;
	char	*dest;

	i = 0;
	v.j = 0;
	v.len = ft_count_lenght(str);
	dest = malloc(v.len + 1);
	if (!dest)
		return (NULL);
	while (str[i] && v.j < v.len)
	{
		if (ft_operations(str[i]))
		{
			if (str[i - 1] != ' ' && str[i - 1] != '>' && str[i - 1] != '<')
				dest[v.j++] = ' ';
			dest[v.j++] = str[i];
			if (str[i + 1] != ' ' && str[i + 1] != '>' && str[i + 1] != '<')
				dest[v.j++] = ' ';
		}
		else
			dest[v.j++] = str[i];
		i++;
	}
	return (dest[v.j] = '\0', dest);
}

void	handle_quotes(char *av)
{
	t_var	v;

	v.i = -1;
	v.j = 0;
	while (av[++v.i])
	{
		if (av[v.i] == 39)
		{
			v.i++;
			while (av[v.i] != 39 && av[v.i])
				v.i++;
			if (!av[v.i])
				ft_error_message("syntax error: unclosed quotes!");
		}
		else if (av[v.i] == 34)
		{
			v.i++;
			while (av[v.i] != 34 && av[v.i])
				v.i++;
			if (!av[v.i])
				ft_error_message("syntax error: unclosed quotes!");
		}
	}
}
