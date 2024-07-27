CC = cc

FLAGS = -Wall -Wextra -Werror -g -fsanitize=address	

FILE =				./builtins/export_helper.c \
					./builtins/ft_builtins.c \
					./builtins/ft_cd.c \
					./builtins/ft_echo.c \
					./builtins/ft_env.c \
					./builtins/ft_exit.c \
					./builtins/ft_export.c \
					./builtins/ft_unset.c \
					./execution/execution.c \
					./execution/execution_helper.c \
					./execution/execution_utils.c \
					./expend_and_herdoc/expand_helper.c \
					./expend_and_herdoc/ft_expand.c \
					./expend_and_herdoc/herdoc.c \
					./expend_and_herdoc/here_helper.c \
					./expend_and_herdoc/utils_her.c \
					./ft_split.c \
					./libft_helper.c \
					./lists/env_list.c \
					./lists/fill_new_struct.c \
					./lists/final_list.c \
					./lists/lists.c \
					./lists/utils_last_list.c \
					./minishell.c \
					./parcing/exit_status.c \
					./parcing/parser.c \
					./parcing/parser_utils.c \
					./parcing/parser_utils2.c \
					./signals.c \
					./syntax_errors.c

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
