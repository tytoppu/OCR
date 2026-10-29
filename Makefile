CC ?= gcc
CFLAGS = -Wall -Wextra -Wvla -Werror -g -fsanitize=address -std=c99 -pedantic -I include
CPPFLAGS = $(shell pkg-config --cflags sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs sdl2 SDL2_image) -lm

SRCS = src/back_propagation.c src/bitmap.c src/filter.c src/forward.c src/ia1.c src/image.c src/main.c src/saving_progress.c
SRCS_image = src/bitmap.c src/filter.c src/image.c src/cell_detection.c src/detection_manager.c src/dominant_color.c src/grid_detection.c src/letter_detection.c src/word_list_detection.c test/image_decouping.c

OBJS = $(SRCS:.c=.o)
OBJS_image = $(SRCS_image:.c=.o)
TARGET = exec

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

image: $(OBJS_image)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(TARGET) $(OBJS) image $(OBJS_image)

.PHONY: all clean
