NAME=gmbu

SRCS=gbmu.c cart.c ram.c cpu.c instructions.c gpu.c
OBJS=$(SRCS:.c=.o)

HEADERS=gbmu.h cart.h ram.h cpu.h instructions.h gpu.h

C=gcc
CFLAGS=-Wall -Werror -Wextra -lSDL

all: $(NAME)

$(NAME): $(OBJS)
	$(C) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADERS)
	$(C) $(CFLAGS) -c $*.c

clean:
	rm $(OBJS)

fclean: clean
	rm $(NAME)

re: fclean all

.PHONY: all clean fclean re
