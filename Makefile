CC ?= gcc
CFLAGS = -Wall -Wextra -Wvla -Werror -g -fsanitize=address -I include
CPPFLAGS = $(shell pkg-config --cflags sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs sdl2 SDL2_image) -lm


SRCS_treatment_detection = src/treatment/detection/cell_detection.c \
    src/treatment/detection/detection_manager.c \
    src/treatment/detection/new_grid_list_detection.c \
    src/treatment/detection/word_list_detection.c
SRCS_treatment = src/treatment/bitmap.c src/treatment/filter.c \
    src/treatment/image.c $(SRCS_treatment_detection)
SRCS_test = main.c
SRCS_image = $(SRCS_treatment) $(SRCS_test)
SRCS = src/ai/forward.c src/trash/ancient_image.c src/ai/ai1.c \
    src/main_nxor.c src/ai/saving_progress.c src/ai/back_propagation.c

OBJS = $(SRCS:.c=.o)
OBJS_image = $(SRCS_image:.c=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# the image part (treatment) is built in strict C99,
# the AI part and the solver are not
image: CFLAGS += -std=c99 -pedantic
image: $(OBJS_image)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

solver:
	$(CC) $(CFLAGS) src/solver.c -o solver

clean:
	$(RM) $(TARGET) $(OBJS) image $(OBJS_image)
	$(RM) result_test/test_image_cutting/list/letters/*.bmp
	$(RM) result_test/test_image_cutting/list/words/*.bmp
	$(RM) result_test/test_image_cutting/grid/*.bmp


.PHONY: all clean
