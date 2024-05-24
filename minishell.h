/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:42:02 by zait-bel          #+#    #+#             */
/*   Updated: 2024/05/24 11:59:12 by mohimi           ###   ########.fr       */
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

/*--------------------------------------- colors------------------------------*/
# define PINK "\x1b[95m"
# define RED "\x1b[31m"
# define ORANGE "\x1b[38;5;214m"
# define RESET "\x1b[0m"

/*----------------------------------Tokenization-----------------------------*/

# define WORD 1
# define SPACE 2
# define DOUBLE_QUOTES 3
# define SINGLE_QUOTE 4
# define OUT 5
# define APPEND_OUT 6
# define IN 7
# define PIPE 8
# define HER_DOC 9
# define VAR 10
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
/*--------------- tokens struct---------------------------------*/
typedef struct token
{
	int				type;
	char			*content;
	struct token	*next;

}			t_token;

typedef struct env
{
	char		*key;
	char		*val;
	struct env 	*next;
}			t_env;

/*--------------------------lexer----------------------------*/
char	**ft_split(char *s, char c);
void	ft_error_message(char *mess);
int		ft_operations(char c);
int		ft_error_operation(char c);
void	handle_quotes(char *av, t_token **token, int *i);
void	handle_spaces(char *av, t_token **token, int *i);
void	skip_space(char *str, int *i);
void	out_redir(char *av, t_token **token, int *i);
void	in_redir(char *av, t_token **token, int *i);
void	pipe_word(char *av, t_token **token, int *i);
void	handle_dollar(char *av, int *i, t_token **token);

/*---------------------------- syntax error----------------------------*/
int		pipe_errors(t_token *token);
int		redirec_errors(t_token *token);
/*---------------------------libft_helper-------------------------------*/
int		ft_strlen(char *str);
int		ft_strncmp(const char *s1, const char *s2, int n);
char	*ft_substr(char *s, int start, int len);
char	*ft_strdup(char *s1);
int		ft_isalnum(int a);
/*------------------------linked list-------------------------------*/
t_token	*ft_lstnew(int type, char *content);
void	add_back(t_token **lst, t_token *new);
void	ft_lstclear(t_token **lst);
#endif