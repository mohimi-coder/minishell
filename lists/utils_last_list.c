/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_last_list.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mohimi <mohimi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/09 08:59:39 by mohimi            #+#    #+#             */
/*   Updated: 2024/06/10 09:41:57 by mohimi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_list	*ft_lstnew_mdl(char **cmond, t_token *dir)
{
	t_list	*new_node;

	new_node = malloc(sizeof(t_list));
	if (new_node == NULL)
		return (NULL);
	new_node->cmond = cmond;
	new_node->red = dir;
	new_node->next = NULL;
	return (new_node);
}

t_list	*ft_lstlast_mdl(t_list *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next != NULL)
		lst = lst->next;
	return (lst);
}

void	add_back_mdl(t_list **lst, t_list *new)
{
	t_list	*last;

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
