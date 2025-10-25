CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lpthread
SRC = src/main.c src/http.c src/cpu.c src/mem.c src/net.c
OBJ = $(SRC:.c=.o)
TARGET = ophanim

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)
