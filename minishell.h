/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:42:02 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/21 10:02:46 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <limits.h>
# include <stdbool.h>

/*--------------------------------------- colors---------------------------------------------*/
# define RED "\x1b[31m"
# define GREEN "\x1b[32m"
# define YELLOW "\x1b[33m"
# define BLUE "\x1b[34m"
# define MAGENTA "\x1b[35m"
# define CYAN "\x1b[36m"
# define PINK "\x1b[95m"
# define RESET "\x1b[0m"
# define ORANGE "\x1b[38;5;214m"

// # ifndef BUFFER_SIZE
// #  define BUFFER_SIZE 2
// # endif

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