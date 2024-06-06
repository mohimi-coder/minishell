/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fill_new_struct.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/05 11:32:27 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/06 10:36:35 by mohimi           ###   ########.fr       */
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
    t_token	*cmd ;
    t_token	*rid;
    m_list	*node;
    t_token	*tmp ;
    char	*dup_cont;
    
    (1) && (cmd = NULL, rid = NULL, node = NULL, tmp = head);
    while (tmp) 
    {
        while (tmp && tmp->type != PIPE && tmp->type != SPC) 
	{
            dup_cont = ft_strdup(tmp->content);
            if (tmp->type == HER_DOC || tmp->type == OUT || tmp->type == IN || tmp->type == APPEND_OUT) 
                add_back(&rid, ft_lstnew(tmp->type, dup_cont));
            else 
                add_back(&cmd, ft_lstnew(tmp->type, dup_cont));
            tmp = tmp->next;
        }
        add_back_mdl(&node, ft_lstnew_mdl(cmd, rid));
       (1) && (cmd = NULL, rid = NULL);
        if (tmp)
            tmp = tmp->next;
    }
    /*---------- this part just for test--------------------*/
    while (node) 
    {
        while (node->cmond) 
	{
            printf("cmd= %s\n",  node->cmond->content);
           node->cmond =  node->cmond->next;
        }
        while (node->dir) 
	{
            printf("dir= %d\n", node->dir->type);
            node->dir = node->dir->next;
        }
       node = node->next;
    }
    return (node);
}