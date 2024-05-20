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
void	ft_error_message(char *mess)
{
	write (2, mess, ft_strlen(mess));
	write (2, "\n", 1);
	exit(1);
}
bool ft_operations(char c)
{
	return (c == '|' || c == '<' || c == '>');
}
bool ft_error_operation(char c)
{
	return(c == '&' || c == ';' || c == 92);
}
int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && (s1[i] != '\0' || s2[i] != '\0'))
	{
		if ((unsigned char) s1[i] != (unsigned char) s2[i])
			return ((unsigned char) s1[i] - (unsigned char) s2[i]);
		i++;
	}
	return (0);
}

void	ft_operation_handller(char *new_str)
{
	int i;

	i = 0;
	while(new_str[i])
	{
		if(!ft_strncmp(new_str + i, "<<<", 3) || !ft_strncmp(new_str + i, ">>>", 3))
			ft_error_message("syntax error near unexpected token");
		if(ft_error_operation(new_str[i]))
			ft_error_message("syntax error: special characters!");
		if(ft_operations(new_str[i]))
		{
			if (new_str[i] == '<' && new_str[i + 1] == '|')
				ft_error_message("syntax error near unexpected token");
			if(new_str[i] == '>' && new_str[i + 1] == '<')
				ft_error_message("syntax error near unexpected token");
			if(new_str[i] == '|' && new_str[i + 1] == '|')
				ft_error_message("syntax error near unexpected token");
		}
		i++;
	}
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
				ft_error_message("syntax error: unclosed quotes!");
		}
		else if (av[i] == 34)
		{
			i++;
			while (av[i] != 34 && av[i])
				i++;
			if (!av[i])
				ft_error_message("syntax error: unclosed quotes!");
		}
		i++;
	}
}

char	*ft_puspaces(char *av)
{
	char	*avv;

	ft_operation_handller(av);
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