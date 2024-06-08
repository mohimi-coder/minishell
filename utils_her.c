/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_her.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 19:02:29 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/08 17:16:09 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_putendl_fd(char *s, int fd)
{
	int	i;

	i = 0;
	if (!s)
		return ;
	while (s[i] != '\0')
	{
		write(fd, &s[i], 1);
		i++;
	}
	write(fd, "\n", 1);
}

char	*ft_strjoin_her(char *s1, char *s2)
{
	char	*str;
	size_t	i;
	size_t	j;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	i = 0;
	j = 0;
	str = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!str)
		return (NULL);
	while (s1[i])
	{
		str[i] = s1[i];
		i++;
	}
	while (s2[j])
		str[i++] = s2[j++];
	str[i] = '\0';
	return (str);
}

char	*ft_itoa(int n)
{
	char	arr[12];
	long	b;
	int		i;
	int		divisor;

	b = n;
	if (n == 0)
		return (ft_strdup("0"));
	if (b < 0)
		b *= -1;
	divisor = 1000000000;
	arr[0] = '0';
	i = 0;
	while (++i <= 10)
	{
		arr[i] = b / divisor + '0';
		b %= divisor;
		divisor /= 10;
	}
	i = 0;
	while (arr[i] == '0')
		i++;
	if (n < 0)
		arr[--i] = '-';
	return (arr[11] = '\0', ft_strdup(&arr[i]));
}

static void	handle_variable(t_var *v, char *s, t_env *env)
{
	v->start = v->i;
	if (s[v->i + 1] >= '0' && s[v->i + 1] <= '9')
		v->i++;
	else
		while (s[v->i + 1] && (f_alnum(s[v->i + 1]) || s[v->i + 1] == '_'))
			v->i++;
	v->st = ft_substr(s, v->start, ++v->i - v->start);
	v->tmp = ft_expand_var(v->st, env);
	v->join = ft_strjoin(v->join, v->tmp);
}

char	*ft_expand_dollar_her(char *s, t_env *env)
{
	t_var	v;

	v.i = 0;
	v.join = NULL;
	v.tmp = NULL;
	while (s[v.i])
	{
		if (s[v.i] == '$' && (f_alnum(s[v.i + 1]) || s[v.i + 1] == '_'))
			handle_variable(&v, s, env);
		else
		{
			v.tmp = ft_substr(s, v.i, 1);
			v.join = ft_strjoin(v.join, v.tmp);
			v.i++;
		}
	}
	if (!v.join)
		v.join = ft_strdup("");
	return (v.join);
}
