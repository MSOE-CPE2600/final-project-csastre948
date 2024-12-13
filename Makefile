CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -O2 `pkg-config --cflags libpng`
LDFLAGS = `pkg-config --libs libpng`

SRC = main.c image_processing.c
OBJ = $(SRC:.c=.o)
TARGET = image_pipeline

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
