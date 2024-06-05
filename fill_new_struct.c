/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_new_struct.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 11:32:27 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/05 23:31:12 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

m_list	*ft_lstnew_mdl(t_token *cmond, t_token *dir)
{
	m_list	*new_node;

	new_node = malloc(sizeof(m_list));
	if (new_node == NULL)
		return (NULL);
	new_node->cmond = cmond;
	new_node->dir = dir;
	new_node->next = NULL;
	return (new_node);
}

m_list	*ft_lstlast_mdl(m_list *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next != NULL)
		lst = lst->next;
	return (lst);
}

void	add_back_mdl(m_list **lst, m_list *new)
{
	m_list	*last;

	if (!lst || !new)
		return ;
	if (*lst == NULL)
	{
		*lst = new;
		return ;
	}
	last = ft_lstlast_mdl(*lst);
	last->next = new;
}

m_list *midl_list(t_token *head)
{
        t_token *cmd = NULL;
        t_token *dir = NULL;
        m_list  *node;
	t_token	*tmp;
	char	*content_dup;

	(1) && (node = NULL, tmp = head);
        while (tmp)
        {
		cmd = NULL;
		dir = NULL;
		while(tmp && tmp->type != PIPE)
              	{
			content_dup = ft_strdup(tmp->content);
			if (!content_dup)
				return(NULL);
                	if (tmp->type == HER_DOC || tmp->type == OUT || tmp->type == IN || tmp->type == APPEND_OUT)
                        	add_back(&dir, ft_lstnew(tmp->type, content_dup));
                	else
                        	add_back(&cmd, ft_lstnew(tmp->type, content_dup));
                	tmp = tmp->next;  
              	}
		if (cmd && dir)
			add_back_mdl(&node, ft_lstnew_mdl(cmd, dir));
		if (tmp)
        		tmp = tmp->next;
        }
	m_list *new = node;
	while(new)
	{
		printf("cmd= %s dir= %d\n", new->cmond->content, new->dir->type);
		new = new->next;
	}
	return (node);
}

