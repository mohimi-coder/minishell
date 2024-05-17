#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int	ft_strlen(char *str)
{
	int i;

	i = 0;
	while(str[i])
		i++;
	return(i);
}

char	*ft_puspaces(char *av)
{
	int i;
	int j;
	int	c;
	int len;
	char *new_str;

	i = 0;
	j = 0;
	c = 0;
	len = ft_strlen(av);
	new_str = malloc(len + 1);
	if (!new_str)
		return(NULL);
	while(j <= len)
		new_str[j++] = '\0';
	j = 0;
	while(av[i] == 32 || ((av[i] >= 9 && av[i] <= 13) && av[i]))
		i++;
	while(av[i])
	{
		if(av[i] == 39)
		{
			new_str[j++] = av[i++];
			while(av[i] != 39 && av[i])
				new_str[j++] = av[i++];
			if(!av[i])
				(write(2, "syntax error!\n", 15), exit(1));
		}
		if(av[i] == 34)
		{
			new_str[j++] = av[i++];
			while(av[i] != 34 && av[i])
				new_str[j++] = av[i++];
			if(!av[i])
				(write(2, "syntax error!\n", 15), exit(1));
		}
		while(av[i] != ' ' && !(av[i] >= 9 && av[i] <= 13) && av[i])
			{
				new_str[j++] = av[i];
				i++;
			}
			while(av[i] == ' ' || av[i] >= 9 && av[i] <= 13)
			{
				c = 1;
				i++;
			}
			if(c == 1 && av[i])
				(new_str[j++] = 32, c = 0);
	}
	return(new_str);
}

int main()
{ 
	char *str = "                   heloo               kak    ' ls cat      gg \"    k        laak    llll  llll   ";
	printf("===%s\n", ft_puspaces(str));
}
