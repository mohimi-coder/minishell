/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:38:20 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/20 18:11:33 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_substr(char *s, unsigned int start, int len)
{
	char	*str;
	int		i;

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

int	ft_count(char *s, char c)
{
	int	i;
	int	count;

	i = 0;
	count = 0;
	while (s[i])
	{
		if (s[i] && (s[i] == 39 || s[i] == 34))
		{
			count++;
			i++;
			while (s[i] && (s[i] != 39 && s[i] != 34))
				i++;
			i++;
		}
		while (s[i] && s[i] == c)
			i++;
		if (s[i] && s[i] != c && (s[i] != 39 && s[i] != 34))
			count ++;
		while (s[i] && s[i] != c && (s[i] != 39 && s[i] != 34))
			i++;
	}
	return (count);
}

void	**ft_free(char **arr, int count)
{
	int	j;

	j = 0;
	while (j < count)
	{
		free (arr[j]);
		j++;
	}
	free(arr);
	return (NULL);
}

char	**ft_fill_arr(char **arr, char c, char	*s)
{
	t_var	v;

	(1 == 1) && (v.i = 0, v.j = -1, v.count = ft_count(s, c));
	while (++v.j < v.count)
	{
		while (s[v.i] && s[v.i] == c)
			v.i++;
		if (s[v.i] && (s[v.i] == 39 || s[v.i] == 34))
		{
			v.start = v.i++;
			while (s[v.i] && s[v.i] != 39 && s[v.i] != 34)
				v.i++;
			v.i++;
		}
		else if (s[v.i] && s[v.i] != c && s[v.i] != 39 && s[v.i] != 34)
		{
			v.start = v.i;
			while (s[v.i] && s[v.i] != c && s[v.i] != 39 && s[v.i] != 34)
				v.i++;
		}
		arr[v.j] = ft_substr(s, v.start, (v.i - v.start));
		if (!arr[v.j])
			return (ft_free(arr, v.j), NULL);
	}
	return (arr[v.j] = 0, arr);
}

char	**ft_split(char *s, char c)
{
	char	**arr;
	int		count;

	if (s == NULL)
		return (NULL);
	count = ft_count(s, c);
	arr = (char **)malloc((count + 1) * sizeof(char *));
	if (!arr)
		return (NULL);
	arr = ft_fill_arr(arr, c, s);
	return (arr);
}
