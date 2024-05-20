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

	i = 0;
	j = 0;
	len = ft_strlen(av);
	new_str = malloc(len + 1);
	if (!new_str)
		return NULL;
	while(j <= len)
		new_str[j++] = '\0';
	return (new_str);
}

char *ft_operation_spaces(char *str)
{
	int i;
	int j;
	int len;
	char *dest;

	i = 0;
	j = 0;
	len = ft_count_lenght(str);
	dest = malloc(len + 1);
	if (!dest)
		return (NULL);
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
	return(dest);
}
void handle_quotes(char *av)
{
	int i = 0;
	int j = 0;
	while(av[i])
	{
		if (av[i] == 39)
		{
			i++;
			while (av[i] != 39 && av[i])
				i++;
			if (!av[i])
				write(2, "syntax error!\n", 15), exit(1);
		}
		else if (av[i] == 34)
		{
			i++;
			while (av[i] != 34 && av[i])
				i++;
			if (!av[i])
				write(2, "syntax error!\n", 15), exit(1);
		}
		i++;
	}
}

char	*ft_puspaces(char *av)
{
	char	*avv;

	avv = ft_operation_spaces(av);
	handle_quotes(avv);
	printf("%s", avv);
	return avv;
}

int main(int ac, char **av)
{
	// char *str = "       '  ls -la'   cat|>out      'echo('hello')      $     USER   ";
	// ft_operation_spacese(av[1]);
	// char *l = ft_puspaces(str);
	ft_puspaces(av[1]);
	// printf("-> %s", l);
}