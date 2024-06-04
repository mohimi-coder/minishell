CC = cc

FLAGS = -Wall -Wextra -Werror #-fsanitize=address

FILE =  parser_utils.c parser_utils2.c \
		parser.c putspaces.c \
		lists.c libft_helper.c \
		syntax_errors.c ft_split.c env_list.c \
		ft_expand.c ft_builtins.c ft_export.c \
		ft_cd.c ft_exit.c signals.c herdoc.c \
		utils_her.c \

OBJECT_FILE = $(FILE:.c=.o)

NAME = minishell

READLINE_INC = -I$(shell brew --prefix readline)/include

READLINE_LIB = -L$(shell brew --prefix readline)/lib -lreadline

all : $(NAME)

$(NAME) : $(OBJECT_FILE)
	$(CC) $(FLAGS) $(READLINE_LIB) $(OBJECT_FILE) -o $(NAME)
	@echo "\033[1;32mCompilation Completed Successfully! ✅\033[0;m"

%.o : %.c minishell.h
	@echo "\033[0;34mCompiling $< .\033[0;m"
	$(CC) $(FLAGS) $(READLINE_INC) -c $< -o $@

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