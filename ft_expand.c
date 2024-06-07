/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_expand.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/24 12:04:23 by zait-bel          #+#    #+#             */
/*   Updated: 2024/06/07 18:26:35 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	return (free(str), ft_strdup(""));
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
	if (s[v->i] == '$' && s[v->i + 1] == '$')
	{
		v->tmp = ft_strdup("$$");
		v->i++;
	}
	else
		v->tmp = ft_substr(s, v->i, 1);
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

void	ft_expand(t_token *token, t_env *env)
{
	t_token	*tmp;

	tmp = token;
	while (tmp)
	{
		if (tmp->type == VAR)
			tmp->content = ft_expand_var(tmp->content, env);
		else if (tmp->type == DOUBLE_QUOTES)
			ft_expand_dollar(tmp, tmp->content, env);
		printf("%s\n", tmp->content);
		tmp = tmp->next;
	}
}
