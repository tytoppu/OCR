CC ?= gcc
CFLAGS = -Wall -Wextra -Wvla -Werror -g -fsanitize=address -std=c99 -pedantic -I include
CPPFLAGS = $(shell pkg-config --cflags sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs sdl2 SDL2_image) -lm


SRCS_treatment_detection = src/treatment/detection/cell_detection.c src/treatment/detection/detection_manager.c src/treatment/detection/dominant_color.c src/treatment/detection/grid_detection.c src/treatment/detection/letter_detection.c src/treatment/detection/word_list_detection.c 
SRCS_treatment = src/treatment/bitmap.c src/treatment/filter.c src/treatment/image.c $(SRCS_treatment_detection)
SRCS_test = test/image_decouping.c

SRCS_image = $(SRCS_treatment) $(SRCS_test)
SRCS = src/back_propagation.c src/bitmap.c src/filter.c src/forward.c src/ia1.c src/image.c src/main.c src/saving_progress.c

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
