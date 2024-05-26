/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_herdoc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 09:45:20 by mohimi            #+#    #+#             */
/*   Updated: 2024/05/26 13:17:25 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int	generate_name(char **name)
{
	int		i;
	char	*base;
	char	*tmp;

	i = 1;
	tmp = NULL;
	base = "/tmp/herd-";
	*name = ft_strdup("/tmp/herd");
	if (!*name)
		return (-1);
	while (!access(*name, F_OK))
	{
		i++;
		tmp = join(base, ft_itoa(i));
		if (!tmp)
			return (-1);
		free (*name);
		*name = tmp;
	}
	free(tmp);
	retun (0);
}

void	ft_putendl_fd(char *s, int fd)
{
	int	i;

	i = 0;
	while (s[i] != '\0')
	{
		write(fd, &s[i], 1);
		i++;
	}
	write(fd, "\n", 1);
}

int	open_file_name(char **str, int *fd)
{
	char	*file_name;

	file_name = NULL;
	if (*fd && !generate_name(&file_name))
	{
		*fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC, 0666);
		if (*fd < 0)
		{
			free(file_name);
			ft_putendl_fd(strerror(errno), 2);
			return (-1);
		}
		*str = file_name;
		return (0);
	}
	return (-1);
}
