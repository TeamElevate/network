CC      := gcc
CFLAGS  := -Wall -Werror -g
LDFLAGS := 
LIBS    :=

SOURCES = udp.c udpping.c beacon.c
OBJS    = $(SOURCES:%.c=%.o)

all: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o udpping $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o udpping
