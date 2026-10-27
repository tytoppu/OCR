CC ?= gcc
CFLAGS = -Wall -Wextra -Wvla -Werror -g -fsanitize=address -lm

SRCS = src/forward.c src/image.c src/ia1.c src/main.c src/saving_progress.c  src/back_propagation.c
OBJS = $(SRCS:.c=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ 


clean:
	$(RM) $(TARGET)
	$(RM) $(OBJS)
	$(RM) main

.PHONY: all clean


