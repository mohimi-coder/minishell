/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:42:02 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/20 18:27:21 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H 

# include <unistd.h>
# include <stdlib.h>
# include <limits.h>
# include <stdbool.h>
# include <stdio.h> 

# ifndef BUFFER_SIZE 
#  define BUFFER_SIZE 2
# endif

typedef struct var
{
	int		i;
	int		j;
	int		len;
	int		start;
	int		count;
}	t_var;
//parser
char	**ft_split(char *s, char c);
int		ft_strlen(char *str);
void	ft_error_message(char *mess);
int		ft_strncmp(const char *s1, const char *s2, int n);
bool	ft_operations(char c);
bool	ft_error_operation(char c);
void	ft_operation_handller(char *new_str);
int		ft_count_lenght(char *str);
char	*allocate(char *av);
char	*ft_operation_spaces(char *str);
void	handle_quotes(char *av);

#endif