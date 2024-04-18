#  Řešení IOS-DU2, 18.4.2024
#  Autor: Tomáš Lajda, FIT

CC = gcc

CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic

SRCS = skibus.c

TARGET = proj2

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

test: kontrola-vystupu.sh
	./kontrola-vystupu.sh 

clean:
	rm -f $(OBJS) $(TARGET)
