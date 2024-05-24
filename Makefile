cc = cc

FLAGS = -Wall -Wextra -Werror -g -fsanitize=address

FILE =  parser_utils.c parser_utils2.c \
		parser.c putspaces.c \
		lists.c libft_helper.c \
		syntax_errors.c ft_split.c env_list.c \

OBJECT_FILE = $(FILE:.c=.o)

NAME = minishell

all : $(NAME)

$(NAME) : $(OBJECT_FILE)
	$(CC) $(FLAGS) -lreadline $(OBJECT_FILE) -o $(NAME)
	@echo "\033[1;32mCompilation Completed Successfully! ✅\033[0;m"

%.o : %.c minishell.h
	@echo "\033[0;34mCompiling $< .\033[0;m"
	$(CC) $(FLAGS) -c $< -o $@

clean :
	@echo "\033[0;31mRemoving object files.\033[0;m"
	rm -f $(OBJECT_FILE)
	@echo "\033[1;32mCleaning DONE ✅\033[0;m"

re : fclean all

fclean : clean
	@echo "\033[0;31mRemoving executable file.\033[0;m"
	rm -f $(NAME)
	@echo "\033[1;32mFClean DONE ✅\033[0;m"

.PHONY : clean