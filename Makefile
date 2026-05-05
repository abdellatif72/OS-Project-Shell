TARGET = myShell

CC = gcc
CFLAGS = -Wall -g


all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) -Iinclude src/*.c -o $(TARGET)

clean:
	rm -f $(TARGET)

memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)
