TARGET = myShell

CC = gcc
CFLAGS = -Wall -g


all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) *.c -o $(TARGET)

clean:
	rm -f $(TARGET)