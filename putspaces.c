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
bool ft_operations(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

int ft_count_lenght(char *str)
{
	int len = 0;
	int i = 0;
	while(str[i])
	{
		if(ft_operations(str[i]))
		{
			if(str[i - 1] != ' ' && str[i - 1] != '>' && str[i - 1] != '<')
				len++;
			if(str[i + 1] != ' ' && str[i + 1] != '>' && str[i + 1] != '<')
				len++;
		}
		len++;
		i++;
	}
	return(len);
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

char *ft_operation_spacese(char *str)
{
	int i;
	int j;
	int len;
	char *dest;

	i = 0;
	j = 0;
	len = ft_count_lenght(str);
	dest = malloc(len + 1);
	while(str[i] && j < len)
	{
		if(ft_operations(str[i]))
		{
			if(str[i - 1] != ' ' && str[i - 1] != '>' && str[i - 1] != '<')
				dest[j++] = ' ';
			dest[j++] = str[i];
			if(str[i + 1] != ' ' && str[i + 1] != '>' && str[i + 1] != '<')
				dest[j++] = ' ';
		}
		else
			dest[j++] = str[i];
		i++;
	}
	dest[j] = '\0';
	printf("===%s\n", dest);
	return(dest);
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

int main(int ac, char **av)
{
	// char *str = "       '  ls -la'   cat|>out      'echo('hello')      $     USER   ";
	ft_operation_spacese(av[1]);
	// char *l = ft_puspaces(str);
	// printf("-> %s", l);
}