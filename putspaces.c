#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int ft_strlen(char *str)
{
    int i = 0;
    while (str[i])
        i++;
    return i;
}

char *allocate(char *av)
{
	int		len;
	char	*new_str;
	int		i;
	int		j;

	len = ft_strlen(av);
	new_str = malloc(len + 1);
	if (!new_str)
		return NULL;
	while(j <= len)
		new_str[j++] = '\0';
	return (new_str);
}

void handle_quotes(char *av, char *new_str, int *i, int *j)
{
	if (av[*i] == 39)
	{
		new_str[(*j)++] = av[(*i)++];
		while (av[*i] != 39 && av[*i])
			new_str[(*j)++] = av[(*i)++];
		if (!av[*i])
			write(2, "syntax error!\n", 15), exit(1);
	}
	if (av[*i] == 34)
	{
		new_str[(*j)++] = av[(*i)++];
		while (av[*i] != 34 && av[*i])
			new_str[(*j)++] = av[(*i)++];
		if (!av[*i])
			write(2, "syntax error!\n", 15), exit(1);
	}
	while (av[*i] != ' ' && !(av[*i] >= 9 && av[*i] <= 13) && av[*i])
		new_str[(*j)++] = av[(*i)++];
}

void	skip_spaces(char *av, int *i)
{
	while (av[*i] == ' ' || (av[*i] >= 9 && av[*i] <= 13))
		(*i)++;
}

void handle_spaces(char *av, char *new_str, int *i, int *j)
{
	int c;

	while ((av[*i] == ' ' || (av[*i] >= 9 && av[*i] <= 13)) && av[*i])
	{
		c = 1;
		(*i)++;
	}
	if (c == 1 && av[*i])
		new_str[(*j)++] = ' ';
}

char	*ft_puspaces(char *av)
{
	char	*new_str;
	int		i;
	int		j;

	i = 0;
	j = 0;
	new_str = allocate(av);
	if (!new_str)
		return NULL;
	skip_spaces(av, &i);
	while (av[i])
	{
		handle_quotes(av, new_str, &i, &j);
		handle_spaces(av, new_str, &i, &j);
	}
	return new_str;
}

int main()
{
	char str[] = "   '  ls -la'   cat|>out      '   echo('hello')  $     USER   ";
	char *l = ft_puspaces(str);
	printf("->%s", l);
	// ft_operation_spacese(l);
}