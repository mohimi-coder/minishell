/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 13:42:02 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/29 16:45:58 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <limits.h>
# include <errno.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/stat.h>
# include <stdbool.h>
# include <termios.h>
/*--------------------------------------- colors------------------------------*/
# define PINK "\x1b[95m"
# define RED "\x1b[31m"
# define ORANGE "\x1b[38;5;214m"
# define BLUE "\x1b[34m"
# define GREEN "\x1b[32m"
# define YELLOW "\x1b[33m"
# define MAGENTA "\x1b[35m"
# define CYAN "\x1b[36m"
# define BOLD "\033[1m"
# define UNDERLINE "\033[4m"
# define PURPLE "\033[01;35m"
# define RESET "\x1b[0m"
/*----------------------------------Tokenization-----------------------------*/

# define WORD 1
# define SPC 2
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
# define CMD 13

int	g_flag;
typedef struct pipe
{
	int		end[2];
	int		end2[2];
	int		indice;
	int		in;
}	t_pipe;

typedef struct var
{
	int		i;
	int		start;
	char	*tmp;
	char	*st;
	char	*join;
}			t_var;
/*--------------- tokens struct---------------------------------*/
typedef struct token
{
	int				type;
	char			*content;
	struct token	*next;

}			t_token;
/*-------------------- environment struct------------------------*/
typedef struct env
{
	char		*key;
	char		*val;
	int			index;
	struct env	*next;
}			t_env;

/*---------------------new struct-----------------------------*/
typedef struct end_list
{
	char				**cmd;
	t_token				*red;
	struct end_list		*next;	
}					t_list;

typedef struct s_fd
{
	int		fd1;
	int		fd2;
}	t_fd;

/*--------------------------lexer----------------------------*/
void	ft_sign(void);
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
void	ft_free_leak(char **str);

/*---------------------------- syntax error----------------------------*/
int		pipe_errors(t_token *token);
int		redirec_errors(t_token *token);
/*---------------------------libft_helper-------------------------------*/
int		ft_strlen(char *str);
int		ft_strncmp(const char *s1, const char *s2, int n);
char	*ft_substr(char *s, int start, int len);
char	*ft_strdup(char *s1);
int		f_alnum(int a);
char	*ft_strjoin(char *s1, char *s2);
int		ft_strcmp(char const *s1, char const *s2);
char	*ft_strchr(char *s, char c);
char	*ft_tolower(char *c);
void	print_no_cmd(char *cmd, char *msg);
/*------------------------linked list-------------------------------*/
t_token	*ft_lstnew(int type, char *content);
void	add_back(t_token **lst, t_token *new);
void	ft_lstclear(t_token **lst);
void	ft_delone(t_token *lst);
void	ft_remove_node(t_token *tmp, t_token **lst);
/*------------------linked list env-------------------------------*/
void	add_back_env(t_env **lst, t_env *new);
t_env	*ft_lstlast_env(t_env *lst);
void	ft_lstclear_env(t_env **lst);
t_env	*lstnew_env(char *key, char *val);
void	ft_lstdelone(t_env *lst);
/*------------------------Expand-----------------------------------*/
char	*ft_expand_var(char *str, t_env *env);
void	ft_expand(t_token **token, t_env *env);
void	ft_expand_dollar(t_token *tok, char *str, t_env *env);
char	**ft_expand_var_sp(char *str, t_env *env);
void	ft_add_node(t_token *tmp, t_env *env);
int		check_red(t_token *tmp, t_env *env);

/*------------------------Builtins-----------------------------------*/
int		ft_builtins(t_list *token, t_env **env);
void	ft_export_var(t_env *env, char **cmd);
t_env	*ft_check_var(char *str, t_env *env);
void	ft_cd(char **cmd, t_env **env);
void	ft_exit(char **cmd);
void	ft_unset(t_env **env, char **cmd);
void	ft_fill_env(char **env, t_env **envr);
void	ft_env(t_env *env);
void	ft_index(t_env *tmp, t_env *env);
void	ft_echo(t_list *list);
void	ft_update(char *str, t_env *env, t_env *new, char **s);
int		check_cmd(char *cmd);
/*-------------------herdoc functions-------------------------------*/
void	ft_putendl_fd(char *s, int fd);
void	ft_putstr_fd(char *s, int fd);
char	*join_her(char *s1, char *s2);
char	*ft_itoa(int n);
char	*epnd__her(char *s, t_env *env);
t_token	*ft_herdoc(t_token *token, t_env *env);
void	add_newline(char *str, char *delimiter, char **buff);
/*------------------fill midl list------------------------------------*/
t_token	*new_list(t_token *head);
t_list	*ft_lstnew_mdl(char **cmd, t_token *dir);
void	add_back_mdl(t_list **lst, t_list *new);
t_list	*ft_finall(t_token *head);
void	ft_lstclear_final(t_list **lst);
/*------------------execution------------------------------------*/
void	execution(t_list *list, t_env **tenv);
char	*get_path(char	*argv, char	**env);
char	**split_path(char **envp);
char	**get_arr_env(t_env **tenv);
void	ft_perror(char *str);
int		check_red_files(t_list *list_com);
void	sig_int_here(int sig_num);
/*------------------exet_status------------------------------------*/
char	*ft_itoa(int n);
int		ft_status(int status, bool flag);
void	exit_status(pid_t id);
void	sig_here_doc(void);

#endif