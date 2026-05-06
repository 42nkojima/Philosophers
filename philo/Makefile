# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nkojima <nkojima@student.42tokyo.jp>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/05/04 23:15:36 by nkojima           #+#    #+#              #
#    Updated: 2026/05/04 23:22:01 by nkojima          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ===============================
#             Color
# ===============================
GREEN   = \033[0;32m
YELLOW  = \033[0;33m
RESET   = \033[0m

# ===============================
#        CONFIGURATION
# ===============================
NAME    = philo
CC      = cc
CFLAGS  = -Wall -Wextra -Werror
LDLIBS  = -pthread
OBJ_DIR = obj

# ===============================
#               SRC
# ===============================
SRCS    = $(wildcard *.c)
HEADERS = $(wildcard *.h)
OBJS    = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))
DEPS    = $(OBJS:.o=.d)

TEST_SRCS         = $(wildcard tests/*_test.c)
TEST_BINS         = $(patsubst tests/%.c,$(OBJ_DIR)/tests/%,$(TEST_SRCS))
TEST_SUPPORT_SRCS = $(filter-out main.c,$(SRCS))

# ===============================
#             TARGETS
# ===============================
all: $(NAME)

norm:
	norminette $(SRCS) $(HEADERS)

ci: norm all test

$(NAME): $(OBJS)
	@echo "$(YELLOW)[LD] Linking $(NAME)...$(RESET)"
	$(CC) $(CFLAGS) $(OBJS) $(LDLIBS) -o $(NAME)
	@echo "$(GREEN)Compiled: $(NAME)$(RESET)"

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
	@echo "$(GREEN)Compiled: $<$(RESET)"

test: $(TEST_BINS)
	@for test in $(TEST_BINS); do \
		echo "$(YELLOW)[TEST] $$test$(RESET)"; \
		$$test || exit 1; \
	done
	@echo "$(GREEN)All tests passed.$(RESET)"

TEST_DEPS = $(TEST_BINS:=.d)

$(OBJ_DIR)/tests/%: tests/%.c $(TEST_SUPPORT_SRCS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -I. $< $(TEST_SUPPORT_SRCS) $(LDLIBS) -o $@

-include $(TEST_DEPS)

# ===============================
#          RUN / CLEAN
# ===============================
clean:
	$(RM) -r $(OBJ_DIR)
	@echo "$(GREEN)Cleaned object files.$(RESET)"

fclean: clean
	$(RM) $(NAME)
	@echo "$(GREEN)Cleaned executable.$(RESET)"

re: fclean all

-include $(DEPS)

# ===============================
#             PHONY
# ===============================
.PHONY: all clean fclean re norm ci test
