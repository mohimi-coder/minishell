/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_expand.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zait-bel <zait-bel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 12:04:23 by zait-bel          #+#    #+#             */
/*   Updated: 2024/07/25 21:34:57 by zait-bel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_expand_var(char *str, t_env *env)
{
	int		i;

	i = 0;
	while (env)
	{
		if (!ft_strcmp(env->key, str + 1))
			if (env->val)
				return (free(str), ft_strdup(env->val + 1));
		env = env->next;
	}
	return (free(str), NULL);
}

void	handle_variable(t_var *v, char *s, t_env *env)
{
	v->start = v->i;
	if (s[v->i + 1] >= '0' && s[v->i + 1] <= '9')
		v->i++;
	else
		while (s[v->i + 1] && (f_alnum(s[v->i + 1]) || s[v->i + 1] == '_'))
			v->i++;
	v->st = ft_substr(s, v->start, ++v->i - v->start);
	v->tmp = ft_expand_var(v->st, env);
	v->join = ft_strjoin(v->join, v->tmp);
}

void	handle_dollar_sign(t_var *v, char *s)
{
	char	*nbr;

	nbr = ft_itoa(ft_status(0, false));
	if (s[v->i] == '$' && s[v->i + 1] == '$')
	{
		v->tmp = ft_strdup("$$");
		v->i++;
	}
	else if (s[v->i] == '$' && s[v->i + 1] == '?')
	{
		v->tmp = ft_strdup(nbr);
		v->i++;
	}
	else
		v->tmp = ft_substr(s, v->i, 1);
	free(nbr);
	v->join = ft_strjoin(v->join, v->tmp);
	v->i++;
}

void	ft_expand_dollar(t_token *tok, char *s, t_env *env)
{
	t_var	v;

	v.i = 0;
	v.join = NULL;
	v.tmp = NULL;
	while (s[v.i])
	{
		if (s[v.i] == '$' && (f_alnum(s[v.i + 1]) || s[v.i + 1] == '_'))
			handle_variable(&v, s, env);
		else
			handle_dollar_sign(&v, s);
	}
	if (!v.join)
		v.join = ft_strdup("");
	free(tok->content);
	tok->content = v.join;
}

void	ft_expand(t_token **token, t_env *env)
{
	t_token	*tmp;
	t_token	*next;

	tmp = *token;
	while (tmp)
	{
		next = tmp->next;
		if ((tmp->type == OUT || tmp->type == IN || tmp->type == APPEND_OUT))
		{
			if (next->type == SPC && next->next->type == VAR)
				next = next->next;
			if (next->type == VAR && check_red(next, env))
				next = next->next;
		}
		else if (tmp->type == VAR)
		{
			ft_add_node(tmp, env);
			if (!tmp->content)
				ft_remove_node(tmp, token);
		}
		else if (tmp->type == DOUBLE_QUOTES || tmp->type == DOLLAR_WHY)
			ft_expand_dollar(tmp, tmp->content, env);
		tmp = next;
	}
}
