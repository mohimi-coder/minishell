/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:42:02 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/21 17:29:54 by mohimi           ###   ########.fr       */
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

/*--------------------------------------- colors------------------------------*/
# define PINK "\x1b[95m"
# define ORANGE "\x1b[38;5;214m"
# define RESET "\x1b[0m"

/*=================================Tokenization===============================*/

# define WORDS 1
# define SPACE 2
# define DOUBLE_QUOTES 3
# define SINGLE_QUOTE 4
# define RED_OUT 5
# define ARED_OUT 6
# define RED_IN 7
# define PIPE 8
# define HER_DOC 9
# define VARIABLE 10
# define DOUBLE_DOLLAR 11
# define DOLLAR_WHY 12

typedef struct var
{
	int		i;
	int		j;
	int		len;
	int		start;
	int		count;
}			t_var;

typedef struct token
{
	int				type;
	char			*content;
	struct token	*next;

}			t_token;
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
void	handle_quotes(char *av, t_token **token, int *i);
void	handle_spaces(char *av, t_token **token, int *i);
void	skip_space(char *str, int *i);
char	*ft_substr(char *s, unsigned int start, size_t len);
char	*ft_strdup(char *s1);
//linked list
t_token	*ft_lstnew(int type, char *content);
void	add_back(t_token **lst, t_token *new);
void	ft_lstclear(t_token **lst);

#endif