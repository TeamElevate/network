CC      := gcc
CFLAGS  := -Wall -Werror
LDFLAGS := 
LIBS    :=

SOURCES = udp.c
OBJS    = $(SOURCES:%.c=%.o)

all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<
