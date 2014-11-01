CC      := gcc
CFLAGS  := -Wall -Werror -g
LDFLAGS := 
LIBS    := 
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	LIBS += -luuid
endif

SOURCES = udp.c udpping.c beacon.c peer.c
OBJS    = $(SOURCES:%.c=%.o)

all: udpping

udpping: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) *.o udpping
