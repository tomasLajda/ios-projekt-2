CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LDFLAGS = -lrt

SRCS = skibus.c shared_memory.c
OBJS = $(SRCS:.c=.o)
TARGET = proj2

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c shared_memory.h
	$(CC) $(CFLAGS) -c -o $@ $<

test: kontrola-vystupu.sh
	./kontrola-vystupu.sh

clean:
	rm -f $(OBJS) $(TARGET)

zip:
	zip proj2.zip *.c *.h Makefile