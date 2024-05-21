/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/21 12:50:53 by mohimi            #+#    #+#             */
/*   Updated: 2024/05/21 17:30:31 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_spaces(char *av, t_token **token, int *i)
{
	int	c;

	c = 0;
	while ((av[*i] == ' ' || (av[*i] >= 9 && av[*i] <= 13)) && av[*i])
	{
		c = 1;
		(*i)++;
	}
	if (c == 1 && av[*i])
		add_back(token, ft_lstnew(SPACE, ft_strdup(" ")));
}

void	skip_space(char *str, int *i)
{
	while ((str[*i] == ' ' || (str[*i] >= 9 && str[*i] <= 13)) && str[*i])
		(*i)++;
}

char	*ft_substr(char *s, unsigned int start, size_t len)
{
	char	*str;
	size_t	i;

	i = 0;
	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		len = 0;
	if (len > ft_strlen(s) - start)
		len = ft_strlen(s) - start;
	str = (char *)malloc(len + 1);
	if (!str)
		return (NULL);
	while (i < len)
	{
		str[i] = s[start];
		i++;
		start++;
	}
	str[i] = '\0';
	return (str);
}

char	*ft_strdup(char *s1)
{
	char	*ptr;
	size_t	i;

	i = 0;
	ptr = (char *)malloc(ft_strlen(s1) + 1);
	if (!ptr)
		return (NULL);
	while (s1[i])
	{
		ptr[i] = s1[i];
		i++;
	}
	ptr[i] = '\0';
	return (ptr);
}
