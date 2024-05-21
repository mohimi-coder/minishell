cc = cc

FLAGS = -Wall -Wextra -Werror

FILE = parser_utils.c parser_utils2.c parser.c putspaces.c lists.c libft_helper.c

OBJECT_FILE = $(FILE:.c=.o)

NAME = minishell

GREEN = \033[0;32m
RESET = \033[0m

all : $(NAME)

$(NAME) : $(OBJECT_FILE)
	$(CC) $(FLAGS) -lreadline $(OBJECT_FILE) -o $(NAME)
	@echo "$(GREEN)$(NAME) created$(RESET)"

%.o : %.c minishell.h
	$(CC) $(FLAGS) -c $< -o $@

clean :
	rm -f $(OBJECT_FILE)
	@echo "$(GREEN)$(NAME) cleaned$(RESET)"

re : fclean all

fclean : clean
	rm -f $(NAME)
	@echo "$(GREEN)$(NAME) fcleaned$(RESET)"

.PHONY : clean