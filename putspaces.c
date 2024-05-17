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
	int len;
	char *new_str;

	i = 0;
	j = 0;
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
		if(av[i] == 39 || av[i] == 34)
		{
			i++;
			while(av[i] != 39 && av[i] != 34)
				i++;
		}
		if((av[i] != 32 && !(av[i] >= 9 && av[i] <= 13) && av[i] ))
		{
			if (av[i - 1] == 32 || (av[i - 1] >= 9 && av[i - 1] <= 13))
				new_str[j++] = 32;
			new_str[j++] = av[i];
		}
		i++;
	}
	if(*new_str == 32 || (*new_str >= 9 && *new_str <= 13))
		*new_str++;
	return(new_str);
}

int main()
{
	char *str = "heloo           '     kak'     ls cat      gg     k  ";
	printf("%s\n", ft_puspaces(str));
}
